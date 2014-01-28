#include "image.h"
#include <cmath>
#include <cstring>
#include <cfloat> // for FLT_EPSILON;
#include <utility> // for std::swap;
#include <ocidl.h>
#include <olectl.h>
#include "../libpng/png.h"
#include "../libpng/pngstruct.h"
#include "../libpng/pnginfo.h"
#include "ege/gdi.h"
#include "ege/text.h"
#include "global.h"
#include "ege/gapi.h"
#include "ege/gapi_aa.h"
#include "ege/img.h"

namespace ege
{

namespace
{

::HBITMAP g_hbmp_def;
::HBRUSH g_hbr_def;
::HPEN g_pen_def;
::HFONT g_font_def;

} // unnamed namespace;

IMAGE::IMAGE()
	: IMAGE(1, 1)
{}

IMAGE::IMAGE(int width, int height)
	: IMAGE(get_pages().get_image_context(), width, height)
{}

IMAGE::IMAGE(::HDC hdc, int width, int height)
	: m_hDC([hdc]{assert(hdc); return ::CreateCompatibleDC(hdc);}())
{
	if(m_hDC)
	{
		resize(width, height);
		setcolor(LIGHTGRAY, this);
		setbkcolor(BLACK, this);
		::SetBkMode(hdc, OPAQUE); //TRANSPARENT);
		setfillstyle(SOLID_FILL, 0, this);
		setlinestyle(PS_SOLID, 0, 1, this);
		settextjustify(LEFT_TEXT, TOP_TEXT, this);
		m_aa = {};
	}
//	else
//		throw ::GetLastError();
}

IMAGE::IMAGE(const IMAGE& img)
	: IMAGE(img.m_hDC, img.m_width, img.m_height)
{
	::BitBlt(m_hDC, 0, 0, img.m_width, img.m_height, img.m_hDC, 0, 0, SRCCOPY);
}
IMAGE::IMAGE(IMAGE&& img) ynothrow
{
	img.swap(*this);
}

IMAGE::~IMAGE()
{
	::DeleteObject(::SelectObject(m_hDC, g_hbmp_def));
	::DeleteObject(::SelectObject(m_hDC, g_hbr_def));
	::DeleteObject(::SelectObject(m_hDC, g_pen_def));
	::DeleteObject(::SelectObject(m_hDC, g_font_def));
	::DeleteDC(m_hDC);
}

IMAGE&
IMAGE::operator=(IMAGE img) ynothrow
{
	img.swap(*this);
	return *this;
}

void
IMAGE::swap(IMAGE& img) ynothrow
{
	std::swap(m_hDC, img.m_hDC);
	std::swap(m_hBmp, img.m_hBmp);
	std::swap(m_width, img.m_width);
	std::swap(m_height, img.m_height);
	std::swap(m_pBuffer, img.m_pBuffer);
	std::swap(m_color, img.m_color);
	std::swap(m_fillcolor, img.m_fillcolor);
	std::swap(m_aa, img.m_aa);
	std::swap(m_vpt, img.m_vpt);
	std::swap(m_texttype, img.m_texttype);
	std::swap(m_linestyle, img.m_linestyle);
	std::swap(m_linewidth, img.m_linewidth);
	std::swap(m_bk_color, img.m_bk_color);
	std::swap(m_pattern, img.m_pattern);
	std::swap(m_texture, img.m_texture);
}

void
IMAGE::gentexture(bool gen)
{
	m_texture.reset(gen ? new Gdiplus::Bitmap(getwidth(), getheight(),
		getwidth() * 4, PixelFormat32bppARGB, (BYTE*)getbuffer()) : nullptr);
}

int
IMAGE::resize(int width, int height)
{
	std::memset(&m_vpt, 0, sizeof(m_vpt));

	assert(m_hDC);

	VOID* p_bmp_buf;
	auto bmi = ::BITMAPINFO();

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height - 1;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biSizeImage = width * height * 4;

	if(::HBITMAP bitmap = ::CreateDIBSection(
		{},
		&bmi,
		DIB_RGB_COLORS,
		(VOID**)&p_bmp_buf,
		{},
		0
	))
	{
		::HBITMAP hbmp_def = (::HBITMAP)::SelectObject(m_hDC, bitmap);
		if(!g_hbmp_def)
		{
			g_hbmp_def = hbmp_def;
			g_hbr_def  = (::HBRUSH)::GetCurrentObject(m_hDC, OBJ_BRUSH);
			g_pen_def  = (::HPEN)::GetCurrentObject(m_hDC, OBJ_PEN);
			g_font_def = (::HFONT)::GetCurrentObject(m_hDC, OBJ_FONT);
		}
		::DeleteObject(hbmp_def);
		m_hBmp = bitmap;
		m_width = width;
		m_height = height;
		m_pBuffer = (::DWORD*)p_bmp_buf;
		setviewport(0, 0, m_width, m_height, 1, this);
		cleardevice(this);
	}
	else
	{
		int err = ::GetLastError();
		::DeleteDC(m_hDC);
		return err;
	}
	return 0;
}

void
IMAGE::getimage(IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight)
{
	if(resize(srcWidth, srcHeight) == 0)
	{
		const auto img = CONVERT_IMAGE_CONST(pSrcImg);

		::BitBlt(m_hDC, 0, 0, srcWidth, srcHeight, img->m_hDC, srcX, srcY, SRCCOPY);
	}
}

int
IMAGE::getimage(const char* filename, int zoomWidth, int zoomHeight)
{
	if(getimage_pngfile(this, filename) == 0)
		return 0;

	wchar_t wszPath[MAX_PATH * 2 + 1];

	::MultiByteToWideChar(CP_ACP, 0, filename, -1, wszPath, MAX_PATH * 2);
	return getimage(wszPath, zoomWidth, zoomHeight);
}

int
IMAGE::getimage(const wchar_t* filename, int, int)
{
	if(getimage_pngfile(this, filename) == 0)
		return 0;

	struct IPicture* pPicture;
	::OLECHAR wszPath[MAX_PATH * 2 + 1];
	wchar_t szPath[MAX_PATH * 2 + 1] = L"";
	long lWidth, lHeight;
	long lWidthPixels, lHeightPixels;
	::HRESULT hr;

	if(wcsstr(filename, L"http://"))
		::lstrcpyW(szPath, filename);
	else if(filename[1] == ':')
		::lstrcpyW(szPath, filename);
	else
	{
		::GetCurrentDirectoryW(MAX_PATH * 2, szPath);
		::lstrcatW(szPath, L"\\");
		::lstrcatW(szPath, filename);
	}

	::lstrcpyW(wszPath, szPath);
	hr = ::OleLoadPicturePath(wszPath, {}, 0, 0, IID_IPicture,
		(void**)&pPicture);
	if(FAILED(hr))
		return grIOerror;

	const auto img = get_pages().imgtarget;

	pPicture->get_Width(&lWidth);
	lWidthPixels
		= ::MulDiv(lWidth, ::GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);
	lHeightPixels
		= ::MulDiv(lHeight, ::GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
	resize(lWidthPixels, lHeightPixels);
	pPicture->Render(m_hDC, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight,
		lWidth, -lHeight, {});
	pPicture->Release();
	return grOk;
}

void
IMAGE::putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	int srcX, int srcY, ::DWORD dwRop) const
{
	const auto img = CONVERT_IMAGE(pDstImg);

	::BitBlt(img->m_hDC, dstX, dstY, dstWidth, dstHeight, m_hDC, srcX, srcY,
		dwRop);
}

void
IMAGE::putimage(IMAGE* pDstImg, int dstX, int dstY, ::DWORD dwRop) const
{
	putimage(pDstImg, dstX, dstY, m_width, m_height, 0, 0, dwRop);
}

void
IMAGE::putimage(int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, ::DWORD dwRop) const
{
	const auto img = CONVERT_IMAGE(nullptr);
	putimage(img, dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
}

void
IMAGE::putimage(int dstX, int dstY, ::DWORD dwRop) const
{
	const auto img = CONVERT_IMAGE(nullptr);

	putimage(img, dstX, dstY, dwRop);
}

namespace
{

int
saveimagetofile(IMAGE* img, std::FILE * fp)
{
	auto bmpfHead = ::BITMAPFILEHEADER();
	auto bmpinfo = ::BITMAPINFOHEADER();
	int pitch = img->getwidth() * 3, addbit, y, x, zero = 0;

	addbit = 4 - (pitch & 3);
	if(pitch & 3)
		pitch = ((pitch + 4) & ~3);
	else addbit = 0;
	bmpfHead.bfType = *(WORD*)"BM";
	bmpfHead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpfHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
		+ pitch * img->getheight();
	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biBitCount = 24;
	bmpinfo.biHeight = img->getheight();
	bmpinfo.biWidth = img->getwidth();
	bmpinfo.biPlanes = 1;
	bmpinfo.biSizeImage = pitch * img->getheight();
	//bmpinfo.biXPelsPerMeter
	std::fwrite(&bmpfHead, sizeof(bmpfHead), 1, fp);
	std::fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);
	for(y = img->getheight() - 1; y >= 0; --y)
	{
		for(x = 0; x < img->getwidth(); ++x)
		{
			::DWORD col = img->getbuffer()[y * img->getwidth() + x];
			//col = RGBTOBGR(col);
			if(std::fwrite(&col, 3, 1, fp) < 1)
				goto ERROR_BREAK;
		}
		if(addbit > 0)
			std::fwrite(&zero, addbit, 1, fp);
	}
	return 0;
ERROR_BREAK:
	return grIOerror;
}

} // unnamed namespace;

int
IMAGE::saveimage(const char*  filename)
{
	std::FILE* fp = std::fopen(filename, "wb");

	if(!fp)
		return grIOerror;

	int ret = saveimagetofile(this, fp);

	std::fclose(fp);
	return ret;
}

int
IMAGE::saveimage(const wchar_t* filename)
{
	std::FILE* fp = ::_wfopen(filename, L"wb");

	if(!fp)
		return grIOerror;

	int ret = saveimagetofile(this, fp);

	std::fclose(fp);
	return ret;
}

int
IMAGE::getpngimg(std::FILE * fp)
{
	::png_structp pic_ptr;
	::png_infop info_ptr;
	std::uint32_t width, height, depth;

	{
		char header[16];
		std::uint32_t number = 8;
		std::fread(header, 1, number, fp);
		int isn_png = ::png_sig_cmp((::png_const_bytep)header, 0, number);

		if(isn_png)
			return grIOerror;
		fseek(fp, 0, SEEK_SET);
	}

	pic_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, {}, {}, {});
	if(!pic_ptr)
		return -1;
	info_ptr = ::png_create_info_struct(pic_ptr);
	if(!info_ptr)
	{
		::png_destroy_write_struct(&pic_ptr, {});
		return -1;
	}
	::png_init_io(pic_ptr, fp);
	::png_read_png(pic_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_EXPAND, {});
	::png_set_expand(pic_ptr);

	resize((int)(info_ptr->width), (int)(info_ptr->height)); //::png_get_IHDR
	width = info_ptr->width;
	height = info_ptr->height;
	depth = info_ptr->pixel_depth;

	::png_bytepp row_pointers = ::png_get_rows(pic_ptr, info_ptr);
	for(std::uint32_t i = 0; i < height; ++i)
	{
		if(depth == 24)
			for(std::uint32_t j = 0; j < width; ++j)
				m_pBuffer[i * width + j] = 0xFFFFFF & (::DWORD&)row_pointers[i][j * 3];
		else if(depth == 32)
			for(std::uint32_t j = 0; j < width; ++j)
			{
				m_pBuffer[i * width + j] = ((::DWORD*)(row_pointers[i]))[j];
				if((m_pBuffer[i * width + j] & 0xFF000000) == 0)
				{
					m_pBuffer[i * width + j] = 0;
				}
			}
	}
	::png_destroy_read_struct(&pic_ptr, &info_ptr, {});
	return 0;
}

int
IMAGE::savepngimg(std::FILE * fp, int bAlpha)
{
	unsigned long i, j;
	::png_structp pic_ptr;
	::png_infop info_ptr;
	::png_colorp palette;
	::png_byte* image;
	::png_bytep* row_pointers;
	pic_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, {}, {}, {});
	std::uint32_t pixelsize = bAlpha ? 4 : 3;
	std::uint32_t width = m_width, height = m_height;
	if(!pic_ptr)
	{
		return -1;
	}
	info_ptr = ::png_create_info_struct(pic_ptr);
	if(!info_ptr)
	{
		::png_destroy_write_struct(&pic_ptr, {});
		return -1;
	}

	/*if(setjmp(::png_jmpbuf(pic_ptr)))
	{
	::png_destroy_write_struct(&pic_ptr, &info_ptr);
	return -1;
	}// */

	::png_init_io(pic_ptr, fp);
	::png_set_IHDR(pic_ptr, info_ptr, width, height, 8, bAlpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	palette = (::png_colorp)::png_malloc(pic_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(::png_color));
	::png_set_PLTE(pic_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
	::png_write_info(pic_ptr, info_ptr);
	::png_set_packing(pic_ptr);
	image = (::png_byte*)malloc(width * height * pixelsize * sizeof(::png_byte) + 4);
	if(!image)
	{
		::png_destroy_write_struct(&pic_ptr, &info_ptr);
		return -1;
	}
	row_pointers = (::png_bytep*)malloc(height * sizeof(::png_bytep));
	if(!row_pointers)
	{
		::png_destroy_write_struct(&pic_ptr, &info_ptr);
		free(image);
		image = {};
		return -1;
	}
	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; ++j)\
			(::DWORD&)image[(i * width  + j) * pixelsize] = RGBTOBGR(m_pBuffer[i * width + j]);
		row_pointers[i] = (::png_bytep)image + i * width * pixelsize;
	}
	::png_write_image(pic_ptr, row_pointers);
	::png_write_end(pic_ptr, info_ptr);
	::png_free(pic_ptr, palette);
	palette = {};
	::png_destroy_write_struct(&pic_ptr, &info_ptr);
	free(row_pointers);
	row_pointers = {};
	free(image);
	image = {};
	return 0;
}

int
IMAGE::getimage(const char* pResType, const char* pResName, int, int)
{
	if(const auto hrsrc = ::FindResourceA(_graph_setting::get_instance(),
		pResName, pResType))
	{
		auto hg(::LoadResource({}, hrsrc));
		auto dwSize(::SizeofResource({}, hrsrc));
		auto hGlobal(::GlobalAlloc(GMEM_MOVEABLE, dwSize));
		auto pvRes(::LockResource(hg));
		LPVOID pvData;
		IPicture* pPicture;
		IStream* pStm;
		long lWidth, lHeight;
		long lWidthPixels, lHeightPixels;
		::HRESULT hr;

		if(!hGlobal || !(pvData = ::GlobalLock(hGlobal)))
			return grAllocError;
		std::memcpy(pvData, pvRes, dwSize);
		::GlobalUnlock(hGlobal);
		if(S_OK != ::CreateStreamOnHGlobal(hGlobal, TRUE, &pStm))
			return grNullPointer;

		hr = OleLoadPicture(pStm, ::LONG(dwSize), TRUE, IID_IPicture,
							(void**)&pPicture);
		::GlobalFree(hGlobal);
		if(FAILED(hr))
			return grIOerror;

		IMAGE* img(get_pages().imgtarget);

		pPicture->get_Width(&lWidth);
		lWidthPixels = ::MulDiv(lWidth,
			::GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
		pPicture->get_Height(&lHeight);
		lHeightPixels = ::MulDiv(lHeight,
			::GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
		resize(lWidthPixels, lHeightPixels);
		pPicture->Render(m_hDC, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight,
			lWidth, -lHeight, {});
		pPicture->Release();
		return grOk;
	}
	return grIOerror;
}
int
IMAGE::getimage(const wchar_t* pResType, const wchar_t* pResName, int, int)
{
	if(const auto hrsrc = ::FindResourceW(_graph_setting::get_instance(),
		pResName, pResType))
	{
		auto hg = ::LoadResource({}, hrsrc);
		auto dwSize = ::SizeofResource({}, hrsrc);
		auto hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		auto pvRes = ::LockResource(hg);
		LPVOID pvData;
		IPicture* pPicture;
		IStream* pStm;
		long lWidth, lHeight;
		long lWidthPixels, lHeightPixels;

		if(!hGlobal || !(pvData = ::GlobalLock(hGlobal)))
			return grAllocError;
		std::memcpy(pvData, pvRes, dwSize);
		::GlobalUnlock(hGlobal);
		if(S_OK != ::CreateStreamOnHGlobal(hGlobal, TRUE, &pStm))
			return grNullPointer;

		auto hr(OleLoadPicture(pStm, (::LONG)dwSize, TRUE, IID_IPicture,
							   (void**)&pPicture));

		::GlobalFree(hGlobal);
		if(FAILED(hr))
			return grIOerror;

		const auto img = get_pages().imgtarget;

		pPicture->get_Width(&lWidth);
		lWidthPixels = ::MulDiv(lWidth, ::GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
		pPicture->get_Height(&lHeight);
		lHeightPixels = ::MulDiv(lHeight, ::GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
		resize(lWidthPixels, lHeightPixels);
		pPicture->Render(m_hDC, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight,
			lWidth, -lHeight, {});
		pPicture->Release();
		return grOk;
	}
	return grIOerror;
}
int
IMAGE::getimage(void * pMem, long size)
{
	if(pMem)
	{
		::DWORD dwSize = size;
		::HGLOBAL hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		LPVOID pvData;
		struct IPicture* pPicture;
		IStream* pStm;
		long lWidth, lHeight;
		long lWidthPixels, lHeightPixels;
		::HRESULT hr;

		if(!hGlobal || !(pvData = ::GlobalLock(hGlobal)))
		{
			return grAllocError;
		}
		std::memcpy(pvData, pMem, dwSize);
		::GlobalUnlock(hGlobal);
		if(S_OK != ::CreateStreamOnHGlobal(hGlobal, TRUE, &pStm))
		{
			return grNullPointer;
		}

		hr = OleLoadPicture(
				 pStm,
				 (::LONG)dwSize,
				 TRUE,
				 IID_IPicture,
				 (void**)&pPicture
			 );

		::GlobalFree(hGlobal);

		if(FAILED(hr))
		{
			return grIOerror;
		}


		const auto img = get_pages().imgtarget;
		pPicture->get_Width(&lWidth);
		lWidthPixels = ::MulDiv(lWidth,
			::GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
		pPicture->get_Height(&lHeight);
		lHeightPixels = ::MulDiv(lHeight,
			::GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
		resize(lWidthPixels, lHeightPixels);
		pPicture->Render(m_hDC, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight,
			lWidth, -lHeight, {});
		pPicture->Release();
		return grOk;
	}
	return grIOerror;
}

void
IMAGE::putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop) const
{
	const auto img = CONVERT_IMAGE(pDstImg);

	if(img)
	{
		SetStretchBltMode(img->m_hDC, COLORONCOLOR);
		StretchBlt(img->m_hDC, dstX, dstY, dstWidth, dstHeight, m_hDC,
			srcX, srcY, srcWidth, srcHeight, dwRop);
	}
}

namespace
{

void
fix_rect_1size(IMAGE* pdest, IMAGE* psrc,
	int * nXOriginDest,  // x-coord of destination upper-left corner
	int * nYOriginDest,  // y-coord of destination upper-left corner
	int * nXOriginSrc,   // x-coord of source upper-left corner
	int * nYOriginSrc,   // y-coord of source upper-left corner
	int * nWidthSrc,     // width of source rectangle
	int* nHeightSrc      // height of source rectangle
)
{
	viewporttype _vpt{0, 0, pdest->getwidth(), pdest->getheight(), 0};
	/* default value proc */
	if(*nWidthSrc == 0)
	{
		*nWidthSrc  = psrc->getwidth();
		*nHeightSrc = psrc->getheight();
	}
	/* fix src rect */
	if(*nWidthSrc > psrc->getwidth())
	{
		*nWidthSrc -= *nWidthSrc - psrc->getwidth();
		*nWidthSrc = psrc->getwidth();
	}
	if(*nHeightSrc > psrc->getheight())
	{
		*nHeightSrc -= *nHeightSrc - psrc->getheight();
		*nHeightSrc = psrc->getheight();
	}
	if(*nXOriginSrc < 0)
	{
		*nWidthSrc += *nXOriginSrc;
		*nXOriginDest += *nXOriginSrc;
		*nXOriginSrc = 0;
	}
	if(*nYOriginSrc < 0)
	{
		*nHeightSrc += *nYOriginSrc;
		*nYOriginDest += *nYOriginSrc;
		*nYOriginSrc = 0;
	}
	/* fix dest vpt rect */
	if(*nXOriginDest < _vpt.left)
	{
		int dx = _vpt.left - *nXOriginDest;
		*nXOriginDest += dx;
		*nXOriginSrc += dx;
		*nWidthSrc -= dx;
	}
	if(*nYOriginDest < _vpt.top)
	{
		int dy = _vpt.top - *nYOriginDest;
		*nYOriginDest += dy;
		*nYOriginSrc += dy;
		*nHeightSrc -= dy;
	}
	if(*nXOriginDest + *nWidthSrc > _vpt.right)
	{
		int dx = *nXOriginDest + *nWidthSrc - _vpt.right + 1;

		*nWidthSrc -= dx;
	}
	if(*nYOriginDest + *nHeightSrc > _vpt.bottom)
	{
		int dy = *nYOriginDest + *nHeightSrc - _vpt.bottom + 1;

		*nHeightSrc -= dy;
	}
}

} // unnamed namespace;

int
IMAGE::putimage_transparent(
	IMAGE* imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	if(const auto img = CONVERT_IMAGE(imgdest))
	{
		IMAGE* imgsrc = this;
		int y, x;
		::DWORD ddx, dsx;
		::DWORD* pdp, *psp, cr;
		// fix rect
		fix_rect_1size(img, imgsrc, &nXOriginDest, &nYOriginDest, &nXOriginSrc,
			&nYOriginSrc, &nWidthSrc, &nHeightSrc);
		// draw
		pdp = img->m_pBuffer + nYOriginDest * img->m_width + nXOriginDest;
		psp = imgsrc->m_pBuffer + nYOriginSrc  * imgsrc->m_width + nXOriginSrc;
		ddx = img->m_width - nWidthSrc;
		dsx = imgsrc->m_width - nWidthSrc;
		cr = crTransparent;
		for(y = 0; y < nHeightSrc; ++y)
		{
			for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
				if(*psp != cr)
					*pdp = *psp;
			pdp += ddx;
			psp += dsx;
		}
	}
	return grOk;
}

int
IMAGE::putimage_alphablend(
	IMAGE* imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	if(const auto img = CONVERT_IMAGE(imgdest))
	{
		IMAGE* imgsrc = this;
		int y, x;
		::DWORD ddx, dsx;
		::DWORD* pdp, *psp;
		::DWORD sa = alpha + 1, da = 0xFF - alpha;
		// fix rect
		fix_rect_1size(
			img,
			imgsrc,
			&nXOriginDest,
			&nYOriginDest,
			&nXOriginSrc,
			&nYOriginSrc,
			&nWidthSrc,
			&nHeightSrc
		);
		// draw
		pdp = img->m_pBuffer + nYOriginDest * img->m_width + nXOriginDest;
		psp = imgsrc->m_pBuffer + nYOriginSrc  * imgsrc->m_width + nXOriginSrc;
		ddx = img->m_width - nWidthSrc;
		dsx = imgsrc->m_width - nWidthSrc;
		for(y = 0; y < nHeightSrc; ++y)
		{
			for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
			{
				::DWORD d = *pdp, s = *psp;
				d = ((d & 0xFF00FF) * da & 0xFF00FF00) | ((d & 0xFF00) * da >> 16 << 16);
				s = ((s & 0xFF00FF) * sa & 0xFF00FF00) | ((s & 0xFF00) * sa >> 16 << 16);
				*pdp = (d + s) >> 8;
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	return grOk;
}

int
IMAGE::putimage_alphatransparent(
	IMAGE* imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	if(const auto img = CONVERT_IMAGE(imgdest))
	{
		IMAGE* imgsrc = this;
		int y, x;
		::DWORD ddx, dsx;
		::DWORD* pdp, *psp, cr;
		::DWORD sa = alpha + 1, da = 0xFF - alpha;
		// fix rect
		fix_rect_1size(
			img,
			imgsrc,
			&nXOriginDest,
			&nYOriginDest,
			&nXOriginSrc,
			&nYOriginSrc,
			&nWidthSrc,
			&nHeightSrc
		);
		// draw
		pdp = img->m_pBuffer + nYOriginDest * img->m_width + nXOriginDest;
		psp = imgsrc->m_pBuffer + nYOriginSrc  * imgsrc->m_width + nXOriginSrc;
		ddx = img->m_width - nWidthSrc;
		dsx = imgsrc->m_width - nWidthSrc;
		cr = crTransparent;
		for(y = 0; y < nHeightSrc; ++y)
		{
			for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
			{
				if(*psp != cr)
				{
					::DWORD d = *pdp, s = *psp;
					d = ((d & 0xFF00FF) * da & 0xFF00FF00)
						| ((d & 0xFF00) * da >> 16 << 16);
					s = ((s & 0xFF00FF) * sa & 0xFF00FF00)
						| ((s & 0xFF00) * sa >> 16 << 16);
					*pdp = (d + s) >> 8;
				}
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	return grOk;
}

int
IMAGE::putimage_withalpha(
	IMAGE* imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	if(const auto img = CONVERT_IMAGE(imgdest))
	{
		IMAGE* imgsrc = this;
		int y, x;
		::DWORD ddx, dsx;
		::DWORD* pdp, *psp;
		// fix rect
		fix_rect_1size(
			img,
			imgsrc,
			&nXOriginDest,
			&nYOriginDest,
			&nXOriginSrc,
			&nYOriginSrc,
			&nWidthSrc,
			&nHeightSrc
		);
		// draw
		pdp = img->m_pBuffer + nYOriginDest * img->m_width + nXOriginDest;
		psp = imgsrc->m_pBuffer + nYOriginSrc  * imgsrc->m_width + nXOriginSrc;
		ddx = img->m_width - nWidthSrc;
		dsx = imgsrc->m_width - nWidthSrc;
		for(y = 0; y < nHeightSrc; ++y)
		{
			for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
			{
				::DWORD alpha = *psp >> 24;
				//if(*psp != cr)
				{
					::DWORD sa = alpha + 1, da = 0xFF - alpha;
					::DWORD d = *pdp, s = *psp;
					d = ((d & 0xFF00FF) * da & 0xFF00FF00)
						| ((d & 0xFF00) * da >> 16 << 16);
					s = ((s & 0xFF00FF) * sa & 0xFF00FF00)
						| ((s & 0xFF00) * sa >> 16 << 16);
					*pdp = (d + s) >> 8;
				}
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	return grOk;
}

int
IMAGE::putimage_alphafilter(
	IMAGE* imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	IMAGE* imgalpha,        // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	if(const auto img = CONVERT_IMAGE(imgdest))
	{
		IMAGE* imgsrc = this;
		int y, x;
		::DWORD ddx, dsx;
		::DWORD* pdp, *psp, *pap;
		//::DWORD sa = alpha + 1, da = 0xFF - alpha;
		// fix rect
		fix_rect_1size(
			img,
			imgsrc,
			&nXOriginDest,
			&nYOriginDest,
			&nXOriginSrc,
			&nYOriginSrc,
			&nWidthSrc,
			&nHeightSrc
		);
		// draw
		pdp = img->m_pBuffer + nYOriginDest * img->m_width + nXOriginDest;
		psp = imgsrc->m_pBuffer + nYOriginSrc  * imgsrc->m_width + nXOriginSrc;
		pap = imgalpha->m_pBuffer + nYOriginSrc  * imgalpha->m_width + nXOriginSrc;
		ddx = img->m_width - nWidthSrc;
		dsx = imgsrc->m_width - nWidthSrc;
		for(y = 0; y < nHeightSrc; ++y)
		{
			for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp, ++pap)
			{
				::DWORD d = *pdp, s = *psp;
				if(*pap)
				{
					::DWORD sa = (*pap & 0xFF) + 1, da = 0xFF - (*pap & 0xFF);
					d = ((d & 0xFF00FF) * da & 0xFF00FF00) | ((d & 0xFF00) * da >> 16 << 16);
					s = ((s & 0xFF00FF) * sa & 0xFF00FF00) | ((s & 0xFF00) * sa >> 16 << 16);
					*pdp = (d + s) >> 8;
				}
			}
			pdp += ddx;
			psp += dsx;
			pap += dsx;
		}
	}
	return grOk;
}


namespace
{

void
fix_rect_0size(IMAGE* pdest,
			   int * nXOriginDest,  // x-coord of destination upper-left corner
			   int * nYOriginDest,  // y-coord of destination upper-left corner
			   int * nWidthDest,     // width of destination rectangle
			   int* nHeightDest      // height of destination rectangle
			  )
{
	viewporttype _vpt
	{
		0, 0, pdest->getwidth(), pdest->getheight(), 0
	};

	if(*nWidthDest == 0)
		*nWidthDest = pdest->getwidth();
	if(*nHeightDest == 0)
		*nHeightDest = pdest->getheight();
	if(*nXOriginDest < _vpt.left)
		*nXOriginDest += _vpt.left - *nXOriginDest;
	if(*nYOriginDest < _vpt.top)
		*nYOriginDest += _vpt.top - *nYOriginDest;
	if(*nXOriginDest + *nWidthDest > _vpt.right)
		*nWidthDest -= *nXOriginDest + *nWidthDest - _vpt.right;
	if(*nYOriginDest + *nHeightDest > _vpt.bottom)
		*nHeightDest -= *nYOriginDest + *nHeightDest - _vpt.bottom;
}

} // unnamed namespace;

int
IMAGE::imagefilter_blurring_4(int intensity, int alpha, int nXOriginDest,
							  int nYOriginDest, int nWidthDest, int nHeightDest)
{
	std::unique_ptr< ::DWORD[]> buff(new ::DWORD[8 << 10]);
	int x2, y2, ix, iy;
	::DWORD* pdp, lsum, sumRB, sumG;
	int ddx, dldx;
	int centerintensity;
	int intensity2, intensity3, intensity4;
	int intensity2f, intensity3f, intensity4f;
	IMAGE* imgdest = this;

	x2 = nXOriginDest + nWidthDest - 1;
	y2 = nYOriginDest + nHeightDest - 1;
	pdp = imgdest->m_pBuffer + nYOriginDest * imgdest->m_width + nXOriginDest;
	ddx = imgdest->m_width - nWidthDest;
	dldx = imgdest->m_width;
	centerintensity = (0xFF - intensity) * alpha >> 8;
	intensity2 = intensity * alpha / 2 >> 8;
	intensity3 = intensity * alpha / 3 >> 8;
	intensity4 = intensity * alpha / 4 >> 8;
	intensity2f = (intensity * alpha % (2 * alpha)) / 2;
	intensity3f = (intensity * alpha % (3 * alpha)) / 3;
	intensity4f = (intensity * alpha % (4 * alpha)) / 4;
	{
		ix = nXOriginDest;
		{
			sumRB = (pdp[dldx] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (pdp[dldx] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (pdp[dldx] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (pdp[dldx] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF) + (pdp[dldx] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (pdp[dldx] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	for(iy = nYOriginDest + 1; iy < y2; ++iy)
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[ix] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (buff[ix] & 0xFF00) + (pdp[dldx] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (buff[ix] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[ix] & 0xFF00) + (pdp[dldx] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity4 + ((sumRB * intensity4f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity4 + ((sumG * intensity4f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF) + (buff[ix] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[ix] & 0xFF00) + (pdp[dldx] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[ix] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (buff[ix] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (buff[ix] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[ix] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF) + (buff[ix] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[ix] & 0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		//pdp += ddx;
	}
	return grOk;
}

int
IMAGE::imagefilter_blurring_8(
	int intensity,
	int alpha,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest
)
{
	std::unique_ptr< ::DWORD[]> buff(new ::DWORD[8 << 10]);
	int x2, y2, ix, iy;
	::DWORD* pdp, lsum, sumRB, sumG, lbuf;
	int ddx, dldx;
	int centerintensity;
	int intensity2, intensity3, intensity4;
	int intensity2f, intensity3f, intensity4f;

	IMAGE* imgdest = this;
	x2 = nXOriginDest + nWidthDest - 1;
	y2 = nYOriginDest + nHeightDest - 1;
	pdp = imgdest->m_pBuffer + nYOriginDest * imgdest->m_width + nXOriginDest;
	ddx = imgdest->m_width - nWidthDest;
	dldx = imgdest->m_width;

	centerintensity = (0xFF - intensity) * alpha >> 8;
	intensity2 = intensity * alpha / 3 >> 8;
	intensity3 = intensity * alpha / 5 >> 8;
	intensity4 = intensity * alpha / 8 >> 8;
	intensity2f = (intensity * alpha % (3 * alpha)) / 3;
	intensity3f = (intensity * alpha % (5 * alpha)) / 5;
	intensity4f = (intensity * alpha % (8 * alpha)) / 8;
	{
		ix = nXOriginDest;
		{
			sumRB = (pdp[1] & 0xFF00FF)
					+ (pdp[dldx] & 0xFF00FF) + (pdp[dldx + 1] & 0xFF00FF);
			sumG = + (pdp[1] & 0xFF00)
				   + (pdp[dldx] & 0xFF00) + (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (pdp[1] & 0xFF00FF)
					+ (pdp[dldx - 1] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF) + (pdp[dldx + 1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (pdp[1] & 0xFF00)
				   + (pdp[dldx - 1] & 0xFF00) + (pdp[dldx] & 0xFF00) + (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF)
					+ (pdp[dldx - 1] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF);
			sumG = (lsum & 0xFF00)
				   + (pdp[dldx - 1] & 0xFF00) + (pdp[dldx] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	for(iy = nYOriginDest + 1; iy < y2; ++iy)
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[ix] & 0xFF00FF) + (buff[ix + 1] & 0xFF00FF)
					+ (pdp[1] & 0xFF00FF)
					+ (pdp[dldx] & 0xFF00FF) + (pdp[dldx + 1] & 0xFF00FF);
			sumG = (buff[ix] & 0xFF00) + (buff[ix + 1] & 0xFF00)
				   + (pdp[1] & 0xFF00)
				   + (pdp[dldx] & 0xFF00) + (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[ix] & 0xFF00FF) + (buff[ix + 1] & 0xFF00FF)
					+ (lsum & 0xFF00FF) + (pdp[1] & 0xFF00FF)
					+ (pdp[dldx - 1] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF) + (pdp[dldx + 1] & 0xFF00FF);
			sumG = (lbuf & 0xFF00) + (buff[ix] & 0xFF00) + (buff[ix + 1] & 0xFF00)
				   + (lsum & 0xFF00) + (pdp[1] & 0xFF00)
				   + (pdp[dldx - 1] & 0xFF00) + (pdp[dldx] & 0xFF00) + (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity4 + ((sumRB * intensity4f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity4 + ((sumG * intensity4f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[ix] & 0xFF00FF)
					+ (lsum & 0xFF00FF)
					+ (lbuf & 0xFF00FF) + (pdp[dldx] & 0xFF00FF);
			sumG = (buff[ix - 1] & 0xFF00) + (buff[ix] & 0xFF00)
				   + (lsum & 0xFF00)
				   + (pdp[dldx - 1] & 0xFF00) + (pdp[dldx] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[ix] & 0xFF00FF) + (buff[ix + 1] & 0xFF00FF)
					+ (pdp[1] & 0xFF00FF);
			sumG = (buff[ix] & 0xFF00) + (buff[ix + 1] & 0xFF00)
				   + (pdp[1] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[ix] & 0xFF00FF) + (buff[ix + 1] & 0xFF00FF)
					+ (lsum & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (lbuf & 0xFF00) + (buff[ix] & 0xFF00) + (buff[ix + 1] & 0xFF00)
				   + (lsum & 0xFF00) + (pdp[1] & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[ix] & 0xFF00FF)
					+ (lsum & 0xFF00FF);
			sumG = (lbuf & 0xFF00) + (buff[ix] & 0xFF00)
				   + (lsum & 0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum & 0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum & 0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		//pdp += ddx;
	}
	return grOk;
}

int
IMAGE::imagefilter_blurring(
	int intensity,
	int alpha,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest
)
{
	IMAGE* imgdest = this;

	fix_rect_0size(imgdest, &nXOriginDest, &nYOriginDest, &nWidthDest, &nHeightDest);
	if(nWidthDest <= 0 || nHeightDest <= 0)
		return grInvalidRegion;
	if(alpha < 0 || alpha > 0x100)
		alpha = 0x100;
	if(intensity <= 0x80)
	{
		imagefilter_blurring_4(
			intensity * 2,
			alpha,
			nXOriginDest,
			nYOriginDest,
			nWidthDest,
			nHeightDest
		);
	}
	else
	{
		imagefilter_blurring_8(
			(intensity - 0x80) * 2,
			alpha,
			nXOriginDest,
			nYOriginDest,
			nWidthDest,
			nHeightDest
		);
	}
	return grOk;
}

int
IMAGE::putimage_rotate(
	IMAGE* imgtexture,
	int nXOriginDest,
	int nYOriginDest,
	float centerx,
	float centery,
	float radian,
	int btransparent,           // transparent (1) or not (0)
	int alpha,                  // in range[0, 256], alpha== -1 means no alpha
	int smooth
)
{
	return ege::putimage_rotate(this, imgtexture, nXOriginDest, nYOriginDest, centerx, centery, radian, btransparent, alpha, smooth);
}

int
IMAGE::putimage_rotatezoom(
	IMAGE* imgtexture,
	int nXOriginDest,
	int nYOriginDest,
	float centerx,
	float centery,
	float radian,
	float zoom,
	int btransparent,           // transparent (1) or not (0)
	int alpha,                  // in range[0, 256], alpha== -1 means no alpha
	int smooth
)
{
	return ege::putimage_rotatezoom(this, imgtexture, nXOriginDest, nYOriginDest, centerx, centery, radian, zoom, btransparent, alpha, smooth);
}


#define BILINEAR_INTERPOLATION(s, LT, RT, LB, RB, x, y) \
	{\
		alphaA = (int)(x*0x100);\
		alphaB = 0xFF - alphaA;\
		Trb = (((LT & 0xFF00FF) * alphaB + (RT & 0xFF00FF) * alphaA) & 0xFF00FF00) >> 8;\
		Tg =  (((LT & 0xFF00) * alphaB + (RT & 0xFF00) * alphaA) & 0xFF0000) >> 8;\
		Brb = (((LB & 0xFF00FF) * alphaB + (RB & 0xFF00FF) * alphaA) & 0xFF00FF00) >> 8;\
		Bg =  (((LB & 0xFF00) * alphaB + (RB & 0xFF00) * alphaA) & 0xFF0000) >> 8;\
		alphaA = (int)(y*0x100);\
		alphaB = 0xFF - alphaA;\
		crb = ((Trb * alphaB + Brb * alphaA) & 0xFF00FF00);\
		cg =  ((Tg * alphaB + Bg * alphaA) & 0xFF0000);\
		s = (crb | cg) >> 8;\
	}

struct point2d
{
	float x;
	float y;
};

struct point3d
{
	float x;
	float y;
	float z;
};

struct vector2d
{
	point2d p[2];
};

struct vector3d
{
	point3d p[2];
};

struct triangle2d
{
	point2d p[3];
	int color;
};

struct triangle3d
{
	point3d p[3];
	int color;
};

namespace
{

int
float2int(float f)
{
	return f >= 0 ? (int)(f + .5) : (int)(f - .5);
}

void
draw_flat_scanline(IMAGE* dc_dest, const vector2d * vt, IMAGE* dc_src, const vector2d * svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				lp_dest_bmp_byte[dest_w * y + i] = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
			}
		}
	}
}

void
draw_flat_scanline_transparent(IMAGE* dc_dest, const vector2d * vt, IMAGE* dc_src, const vector2d * svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = (int)(vt->p[0].x + .5), e = (int)(vt->p[1].x + .5), y = (int)(vt->p[0].y + .5), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD  col;
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				col = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
				if(col)
				{
					lp_dest_bmp_byte[dest_w * y + i] = col;
				}
			}
		}
	}
}

void
draw_flat_scanline_alpha(IMAGE* dc_dest, const vector2d * vt, IMAGE* dc_src, const vector2d * svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				::DWORD d = lp_dest_bmp_byte[dest_w * y + i], s = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
				d = ((d & 0xFF00FF) * da & 0xFF00FF00) | ((d & 0xFF00) * da >> 16 << 16);
				s = ((s & 0xFF00FF) * sa & 0xFF00FF00) | ((s & 0xFF00) * sa >> 16 << 16);
				lp_dest_bmp_byte[dest_w * y + i] = (d + s) >> 8;
			}
		}
	}
}

void
draw_flat_scanline_alphatrans(IMAGE* dc_dest, const vector2d * vt, IMAGE* dc_src, const vector2d * svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = (int)(vt->p[0].x + .5), e = (int)(vt->p[1].x + .5), y = (int)(vt->p[0].y + .5), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				::DWORD s = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
				if(s)
				{
					::DWORD d = lp_dest_bmp_byte[dest_w * y + i];
					d = ((d & 0xFF00FF) * da & 0xFF00FF00)
						| ((d & 0xFF00) * da >> 16 << 16);
					s = ((s & 0xFF00FF) * sa & 0xFF00FF00)
						| ((s & 0xFF00) * sa >> 16 << 16);
					lp_dest_bmp_byte[dest_w * y + i] = (d + s) >> 8;
				}
			}
		}
	}
}

#if 0
color_t bilinear_interpolation(color_t LT, color_t RT, color_t LB, color_t RB,
	double x, double y)
{
	color_t Trb, Tg, Brb, Bg, crb, cg;
	color_t alphaA, alphaB;
	alphaA = (color_t)(x * 0x100);
	alphaB = 0xFF - alphaA;
	Trb = (((LT & 0xFF00FF) * alphaB + (RT & 0xFF00FF) * alphaA) & 0xFF00FF00) >> 8;
	Tg =  (((LT & 0xFF00) * alphaB + (RT & 0xFF00) * alphaA) & 0xFF0000) >> 8;
	Brb = (((LB & 0xFF00FF) * alphaB + (RB & 0xFF00FF) * alphaA) & 0xFF00FF00) >> 8;
	Bg =  (((LB & 0xFF00) * alphaB + (RB & 0xFF00) * alphaA) & 0xFF0000) >> 8;
	alphaA = (color_t)(y * 0x100);
	alphaB = 0xFF - alphaA;
	crb = ((Trb * alphaB + Brb * alphaA) & 0xFF00FF00);
	cg =  ((Tg * alphaB + Bg * alphaA) & 0xFF0000);
	return (crb | cg) >> 8;
}
#endif

void
draw_flat_scanline_s(IMAGE* dc_dest, const vector2d * vt, IMAGE* dc_src,
	const vector2d* svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x),
		y = float2int((float)vt->p[0].y), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD Trb, Tg, Brb, Bg, crb, cg;
	::DWORD alphaA, alphaB;
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				int ix = (int)curx, iy = (int)cury;
				double fx = curx - ix, fy = cury - iy;
				::DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				::DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w + 1],
					fx,
					fy
				);
				lp_dest_bmp_byte[dest_w * y + i] = col;
			}
		}
	}
}

void
draw_flat_scanline_transparent_s(IMAGE* dc_dest, const vector2d * vt,
	IMAGE* dc_src, const vector2d * svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x),
		y = float2int((float)vt->p[0].y), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD Trb, Tg, Brb, Bg, crb, cg;
	::DWORD alphaA, alphaB;
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				int ix = (int)curx, iy = (int)cury;
				float fx = curx - ix, fy = cury - iy;
				::DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				::DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w + 1],
					fx,
					fy
				);
				if(col)
				{
					lp_dest_bmp_byte[dest_w * y + i] = col;
				}
			}
		}
	}
}

void
draw_flat_scanline_alpha_s(IMAGE* dc_dest, const vector2d * vt, IMAGE* dc_src,
	const vector2d * svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x),
		y = float2int((float)vt->p[0].y), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD Trb, Tg, Brb, Bg, crb, cg;
	::DWORD alphaA, alphaB;
	::DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				int ix = (int)curx, iy = (int)cury;
				float fx = curx - ix, fy = cury - iy;
				::DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				::DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w + 1],
					fx,
					fy
				);
				{
					::DWORD d = lp_dest_bmp_byte[dest_w * y + i];
					d = (((d & 0xFF00FF) * da & 0xFF00FF00)
						| ((d & 0xFF00) * da & 0xFF0000)) >> 8;
					col = (((col & 0xFF00FF) * sa & 0xFF00FF00)
						| ((col & 0xFF00) * sa & 0xFF0000)) >> 8;
					lp_dest_bmp_byte[dest_w * y + i] = d + col;
				}
			}
		}
	}
}

void
draw_flat_scanline_alphatrans_s(IMAGE* dc_dest, const vector2d * vt,
	IMAGE* dc_src, const vector2d * svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x),
		y = float2int((float)vt->p[0].y), w = e - s;
	::DWORD* lp_dest_bmp_byte = (::DWORD*)dc_dest->getbuffer();
	::DWORD* lp_src_bmp_byte = (::DWORD*)dc_src->getbuffer();
	::DWORD Trb, Tg, Brb, Bg, crb, cg;
	::DWORD alphaA, alphaB;
	::DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if(s < x1)
			{
				s = x1;
			}
			if(e > x2)
			{
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for(i = s; i < e; ++i, curx += dx, cury += dy)
			{
				int ix = (int)curx, iy = (int)cury;
				float fx = curx - ix, fy = cury - iy;
				::DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				::DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w + 1],
					fx,
					fy
				);
				if(col)
				{
					::DWORD d = lp_dest_bmp_byte[dest_w * y + i];
					d = (((d & 0xFF00FF) * da & 0xFF00FF00)
						| ((d & 0xFF00) * da & 0xFF0000)) >> 8;
					col = (((col & 0xFF00FF) * sa & 0xFF00FF00)
						| ((col & 0xFF00) * sa & 0xFF0000)) >> 8;
					lp_dest_bmp_byte[dest_w * y + i] = d + col;
				}
			}
		}
	}
}

void
draw_flat_trangle_alpha(IMAGE* dc_dest, const triangle2d * dt, IMAGE* dc_src, const triangle2d * tt, int x1, int y1, int x2, int y2, int transparent, int alpha)
{
	using std::swap;

	triangle2d t2d = *dt;
	triangle2d t3d = *tt;
	bool b_alpha(alpha >= 0 && alpha < 0x100);
	//int width = x2 - x1, height = y2 - y1;

	if(t2d.p[1].y > t2d.p[2].y)
	{
		swap(t2d.p[1], t2d.p[2]);
		swap(t3d.p[1], t3d.p[2]);
	}
	if(t2d.p[0].y > t2d.p[1].y)
	{
		swap(t2d.p[0], t2d.p[1]);
		swap(t3d.p[0], t3d.p[1]);
	}
	if(t2d.p[1].y > t2d.p[2].y)
	{
		swap(t2d.p[1], t2d.p[2]);
		swap(t3d.p[1], t3d.p[2]);
	}
	{
		float dd;
		int s = float2int((float)t2d.p[0].y), e = float2int((float)t2d.p[2].y),
			h, m = float2int((float)t2d.p[1].y);
		int rs, re;
		int i, lh, rh;
		//	float dm = t2d.p[1].y;
		point2d pl, pr;
		point2d spl, spr;

		pl.x = t2d.p[1].x - t2d.p[0].x;
		pr.x = t2d.p[2].x - t2d.p[0].x;
		pl.y = t2d.p[1].y - t2d.p[0].y;
		pr.y = t2d.p[2].y - t2d.p[0].y;
		spl.x = t3d.p[1].x - t3d.p[0].x;
		spr.x = t3d.p[2].x - t3d.p[0].x;
		spl.y = t3d.p[1].y - t3d.p[0].y;
		spr.y = t3d.p[2].y - t3d.p[0].y;
		//	float dh = dm - s;
		h = m - s;
		rs = s;
		if(s < y1)
			s = y1;
		if(m >= y2)
			m = y2;
		/*	if(pl.y > pr.y)
			{
				dd = pr.y / pl.y;
				pl.x *= dd;
				spl.x *= dd;
				spl.y *= dd;
			}
			else
			{
				dd = pl.y / pr.y;
				pr.x *= dd;
				spr.x *= dd;
				spr.y *= dd;
			}
		*/
		if(pl.x > pr.x)
		{
			swap(pl, pr);
			swap(spl, spr);
		}
		lh = float2int((float)(pl.y + t2d.p[0].y))
			 - float2int((float)(t2d.p[0].y));
		rh = float2int((float)(pr.y + t2d.p[0].y))
			 - float2int((float)(t2d.p[0].y));
		if(h > 0)
			for(i = s; i < m; ++i)
			{
				vector2d vt;
				vector2d svt;
				//float dt = (float)(i - rs) / h;
				float dlt = (float)(i - rs) / lh;
				float drt = (float)(i - rs) / rh;

				/*
				vt.p[0].x = t2d.p[0].x + pl.x * dt;
				vt.p[1].x = t2d.p[0].x + pr.x * dt;
				vt.p[0].y = vt.p[1].y = i;

				svt.p[0].x = t3d.p[0].x + spl.x * dt;
				svt.p[1].x = t3d.p[0].x + spr.x * dt;
				svt.p[0].y = t3d.p[0].y + spl.y * dt;
				svt.p[1].y = t3d.p[0].y + spr.y * dt;
				// */
				vt.p[0].x = t2d.p[0].x + pl.x * dlt;
				vt.p[1].x = t2d.p[0].x + pr.x * drt;
				vt.p[0].y = vt.p[1].y = (float)(i);
				svt.p[0].x = t3d.p[0].x + spl.x * dlt;
				svt.p[1].x = t3d.p[0].x + spr.x * drt;
				svt.p[0].y = t3d.p[0].y + spl.y * dlt;
				svt.p[1].y = t3d.p[0].y + spr.y * drt;
				if(b_alpha)
				{
					if(transparent)
						draw_flat_scanline_alphatrans(dc_dest, &vt, dc_src,
							&svt, x1, x2, alpha);
					else
						draw_flat_scanline_alpha(dc_dest, &vt, dc_src, &svt,
							x1, x2, alpha);
				}
				else
				{
					if(transparent)
						draw_flat_scanline_transparent(dc_dest, &vt, dc_src,
							&svt, x1, x2);
					else
						draw_flat_scanline(dc_dest, &vt, dc_src, &svt, x1, x2);
				}
			}
		if(pl.y > pr.y)
		{
			dd = pr.y / pl.y;
			pl.x *= dd;
			spl.x *= dd;
			spl.y *= dd;
		}
		else
		{
			dd = pl.y / pr.y;
			pr.x *= dd;
			spr.x *= dd;
			spr.y *= dd;
		}
		if(m >= y1 && m < y2 && m < e)
		{
			vector2d vt;
			vector2d svt;

			vt.p[0].x = t2d.p[0].x + pl.x;
			vt.p[1].x = t2d.p[0].x + pr.x;
			vt.p[0].y = vt.p[1].y = (float)(m);
			svt.p[0].x = t3d.p[0].x + spl.x;
			svt.p[1].x = t3d.p[0].x + spr.x;
			svt.p[0].y = t3d.p[0].y + spl.y;
			svt.p[1].y = t3d.p[0].y + spr.y;
			if(b_alpha)
			{
				if(transparent)
					draw_flat_scanline_alphatrans(dc_dest, &vt, dc_src, &svt,
						x1, x2, alpha);
				else
					draw_flat_scanline_alpha(dc_dest, &vt, dc_src, &svt,
						x1, x2, alpha);
			}
			else
			{
				if(transparent)
					draw_flat_scanline_transparent(dc_dest, &vt, dc_src, &svt,
						x1, x2);
				else
					draw_flat_scanline(dc_dest, &vt, dc_src, &svt, x1, x2);
			}
		}
		pl.x = t2d.p[0].x - t2d.p[2].x;
		pr.x = t2d.p[1].x - t2d.p[2].x;
		pl.y = t2d.p[0].y - t2d.p[2].y;
		pr.y = t2d.p[1].y - t2d.p[2].y;
		spl.x = t3d.p[0].x - t3d.p[2].x;
		spr.x = t3d.p[1].x - t3d.p[2].x;
		spl.y = t3d.p[0].y - t3d.p[2].y;
		spr.y = t3d.p[1].y - t3d.p[2].y;
		//	dh = e - dm;
		h = e - m;
		re = e;
		if(m < y1)
			m = y1 - 1;
		if(e >= y2)
			e = y2 - 1;
		/*	if(pl.y < pr.y)
			{
				dd = pr.y / pl.y;
				pl.x *= dd;
				spl.x *= dd;
				spl.y *= dd;
			}
			else
			{
				dd = pl.y / pr.y;
				pr.x *= dd;
				spr.x *= dd;
				spr.y *= dd;
			}
		*/
		if(pl.x > pr.x)
		{
			swap(pl, pr);
			swap(spl, spr);
		}
		lh = float2int((float)(t2d.p[2].y))
			 - float2int((float)(pl.y + t2d.p[2].y));
		rh = float2int((float)(t2d.p[2].y))
			 - float2int((float)(pr.y + t2d.p[2].y));
		if(h > 0)
			for(i = e; i > m; --i)
			{
				vector2d vt;
				vector2d svt;
				//float dt = (float)(re - i) / h;
				float dlt = (float)(re - i) / lh;
				float drt = (float)(re - i) / rh;
				/*
				vt.p[0].x = t2d.p[2].x + pl.x * dt;
				vt.p[1].x = t2d.p[2].x + pr.x * dt;
				vt.p[0].y = vt.p[1].y = i;

				svt.p[0].x = t3d.p[2].x + spl.x * dt;
				svt.p[1].x = t3d.p[2].x + spr.x * dt;
				svt.p[0].y = t3d.p[2].y + spl.y * dt;
				svt.p[1].y = t3d.p[2].y + spr.y * dt;
				// */
				vt.p[0].x = t2d.p[2].x + pl.x * dlt;
				vt.p[1].x = t2d.p[2].x + pr.x * drt;
				vt.p[0].y = vt.p[1].y = (float)(i);
				svt.p[0].x = t3d.p[2].x + spl.x * dlt;
				svt.p[1].x = t3d.p[2].x + spr.x * drt;
				svt.p[0].y = t3d.p[2].y + spl.y * dlt;
				svt.p[1].y = t3d.p[2].y + spr.y * drt;
				if(b_alpha)
				{
					if(transparent)
						draw_flat_scanline_alphatrans(dc_dest, &vt, dc_src,
							&svt, x1, x2, alpha);
					else
						draw_flat_scanline_alpha(dc_dest, &vt, dc_src, &svt, x1,
							x2, alpha);
				}
				else
				{
					if(transparent)
						draw_flat_scanline_transparent(dc_dest, &vt, dc_src,
							&svt, x1, x2);
					else
						draw_flat_scanline(dc_dest, &vt, dc_src, &svt, x1, x2);
				}
			}
	}
}

void
draw_flat_trangle_alpha_s(IMAGE* dc_dest, const triangle2d * dt, IMAGE* dc_src,
						  const triangle2d * tt, int x1, int y1, int x2, int y2, int transparent,
						  int alpha)
{
	using std::swap;

	triangle2d t2d = *dt;
	triangle2d t3d = *tt;
	bool b_alpha(alpha >= 0 && alpha < 0x100);
	//int width = x2 - x1, height = y2 - y1;

	if(t2d.p[1].y > t2d.p[2].y)
	{
		swap(t2d.p[1], t2d.p[2]);
		swap(t3d.p[1], t3d.p[2]);
	}
	if(t2d.p[0].y > t2d.p[1].y)
	{
		swap(t2d.p[0], t2d.p[1]);
		swap(t3d.p[0], t3d.p[1]);
	}
	if(t2d.p[1].y > t2d.p[2].y)
	{
		swap(t2d.p[1], t2d.p[2]);
		swap(t3d.p[1], t3d.p[2]);
	}
	{
		float dd;
		int s = float2int((float)t2d.p[0].y), e = float2int((float)t2d.p[2].y),
			h, m = float2int((float)t2d.p[1].y);
		int rs, re;
		int i, lh, rh;
		//	float dm = t2d.p[1].y;
		//	float dh;
		point2d pl, pr;
		point2d spl, spr;

		pl.x = t2d.p[1].x - t2d.p[0].x;
		pr.x = t2d.p[2].x - t2d.p[0].x;
		pl.y = t2d.p[1].y - t2d.p[0].y;
		pr.y = t2d.p[2].y - t2d.p[0].y;
		spl.x = t3d.p[1].x - t3d.p[0].x;
		spr.x = t3d.p[2].x - t3d.p[0].x;
		spl.y = t3d.p[1].y - t3d.p[0].y;
		spr.y = t3d.p[2].y - t3d.p[0].y;
		//	dh = dm - s;
		h = m - s;
		rs = s;
		if(s < y1)
			s = y1;
		if(m >= y2)
			m = y2;
		/*	if(pl.y > pr.y)
			{
				dd = pr.y / pl.y;
				pl.x *= dd;
				spl.x *= dd;
				spl.y *= dd;
			}
			else
			{
				dd = pl.y / pr.y;
				pr.x *= dd;
				spr.x *= dd;
				spr.y *= dd;
			}
		*/
		if(pl.x > pr.x)
		{
			swap(pl, pr);
			swap(spl, spr);
		}
		lh = float2int((float)(pl.y + t2d.p[0].y))
			 - float2int((float)(t2d.p[0].y));
		rh = float2int((float)(pr.y + t2d.p[0].y))
			 - float2int((float)(t2d.p[0].y));
		if(h > 0)
		{
			for(i = s; i < m; ++i)
			{
				vector2d vt;
				vector2d svt;
				//float dt = (float)(i - rs) / h;
				float dlt = (float)(i - rs) / lh;
				float drt = (float)(i - rs) / rh;

				/*
				vt.p[0].x = t2d.p[0].x + pl.x * dt;
				vt.p[1].x = t2d.p[0].x + pr.x * dt;
				vt.p[0].y = vt.p[1].y = i;

				svt.p[0].x = t3d.p[0].x + spl.x * dt;
				svt.p[1].x = t3d.p[0].x + spr.x * dt;
				svt.p[0].y = t3d.p[0].y + spl.y * dt;
				svt.p[1].y = t3d.p[0].y + spr.y * dt;
				// */
				vt.p[0].x = t2d.p[0].x + pl.x * dlt;
				vt.p[1].x = t2d.p[0].x + pr.x * drt;
				vt.p[0].y = vt.p[1].y = (float)(i);

				svt.p[0].x = t3d.p[0].x + spl.x * dlt;
				svt.p[1].x = t3d.p[0].x + spr.x * drt;
				svt.p[0].y = t3d.p[0].y + spl.y * dlt;
				svt.p[1].y = t3d.p[0].y + spr.y * drt;

				if(b_alpha)
				{
					if(transparent)
						draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					else
						draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				}
				else
				{
					if(transparent)
						draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt, x1, x2);
					else
						draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1, x2);
				}
			}
		}
		if(pl.y > pr.y)
		{
			dd = pr.y / pl.y;
			pl.x *= dd;
			spl.x *= dd;
			spl.y *= dd;
		}
		else
		{
			dd = pl.y / pr.y;
			pr.x *= dd;
			spr.x *= dd;
			spr.y *= dd;
		}
		if(m >= y1 && m < y2 && m < e)
		{
			vector2d vt;
			vector2d svt;

			vt.p[0].x = t2d.p[0].x + pl.x;
			vt.p[1].x = t2d.p[0].x + pr.x;
			vt.p[0].y = vt.p[1].y = (float)(m);

			svt.p[0].x = t3d.p[0].x + spl.x;
			svt.p[1].x = t3d.p[0].x + spr.x;
			svt.p[0].y = t3d.p[0].y + spl.y;
			svt.p[1].y = t3d.p[0].y + spr.y;

			if(b_alpha)
			{
				if(transparent)
					draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				else
					draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
			}
			else
			{
				if(transparent)
					draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt, x1, x2);
				else
					draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1, x2);
			}
		}
		pl.x = t2d.p[0].x - t2d.p[2].x;
		pr.x = t2d.p[1].x - t2d.p[2].x;
		pl.y = t2d.p[0].y - t2d.p[2].y;
		pr.y = t2d.p[1].y - t2d.p[2].y;
		spl.x = t3d.p[0].x - t3d.p[2].x;
		spr.x = t3d.p[1].x - t3d.p[2].x;
		spl.y = t3d.p[0].y - t3d.p[2].y;
		spr.y = t3d.p[1].y - t3d.p[2].y;

		//	dh = e - dm;
		h = e - m;
		re = e;
		if(m < y1)
		{
			m = y1 - 1;
		}
		if(e >= y2)
		{
			e = y2 - 1;
		}
	/*	if(pl.y < pr.y)
		{
			dd = pr.y / pl.y;
			pl.x *= dd;
			spl.x *= dd;
			spl.y *= dd;
		}
		else
		{
			dd = pl.y / pr.y;
			pr.x *= dd;
			spr.x *= dd;
			spr.y *= dd;
		}
	*/
		if(pl.x > pr.x)
		{
			swap(pl, pr);
			swap(spl, spr);
		}
		lh = float2int(float(t2d.p[2].y))
			- float2int(float(pl.y + t2d.p[2].y));
		rh = float2int(float(t2d.p[2].y))
			- float2int(float(pr.y + t2d.p[2].y));
		if(h > 0)
		{
			for(i = e; i > m; --i)
			{
				vector2d vt;
				vector2d svt;
				//float dt = (float)(re - i) / h;
				float dlt = (float)(re - i) / lh;
				float drt = (float)(re - i) / rh;

				/*
				vt.p[0].x = t2d.p[2].x + pl.x * dt;
				vt.p[1].x = t2d.p[2].x + pr.x * dt;
				vt.p[0].y = vt.p[1].y = i;

				svt.p[0].x = t3d.p[2].x + spl.x * dt;
				svt.p[1].x = t3d.p[2].x + spr.x * dt;
				svt.p[0].y = t3d.p[2].y + spl.y * dt;
				svt.p[1].y = t3d.p[2].y + spr.y * dt;
				// */
				vt.p[0].x = t2d.p[2].x + pl.x * dlt;
				vt.p[1].x = t2d.p[2].x + pr.x * drt;
				vt.p[0].y = vt.p[1].y = (float)(i);

				svt.p[0].x = t3d.p[2].x + spl.x * dlt;
				svt.p[1].x = t3d.p[2].x + spr.x * drt;
				svt.p[0].y = t3d.p[2].y + spl.y * dlt;
				svt.p[1].y = t3d.p[2].y + spr.y * drt;


				if(b_alpha)
				{
					if(transparent)
						draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					else
						draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				}
				else
				{
					if(transparent)
						draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt, x1, x2);
					else
						draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1, x2);
				}
			}
		}
	}
}

} // unnamed namespace;

int
putimage_trangle(
	IMAGE* imgdest,
	IMAGE* imgtexture,
	const triangle2d * dt,// dest trangle, original
	const triangle2d * tt,// textture trangle uv 0.0 - 1.0
	int btransparent,
	int alpha,
	int smooth
)
{
	IMAGE* dc_dest = imgdest;
	IMAGE* dc_src  = imgtexture;

	if(dc_dest)
	{
		triangle2d _dt = *dt;
		triangle2d _tt = *tt;
		int x1 = 0, y1 = 0, x2 = dc_dest->getwidth(),
			y2 = dc_dest->getheight(), i;

		if(smooth)
		{
			for(i = 0; i < 3; ++i)
			{
				_tt.p[i].x = (float)float2int((float)(_tt.p[i].x
					* (dc_src->getwidth() - 2)));
				_tt.p[i].y = (float)float2int((float)(_tt.p[i].y
					* (dc_src->getheight() - 2)));
			}
		}
		else
			for(i = 0; i < 3; ++i)
			{
				_tt.p[i].x = (float)float2int((float)(_tt.p[i].x
					* (dc_src->getwidth() - 1)));
				_tt.p[i].y = (float)float2int((float)(_tt.p[i].y
					* (dc_src->getheight() - 1)));
			}
		if(smooth)
		{
			if(dc_src->getwidth() > 1 && dc_src->getheight() > 1)
				draw_flat_trangle_alpha_s(dc_dest, &_dt, dc_src, &_tt, x1, y1,
					x2, y2, btransparent, alpha);
		}
		else
			draw_flat_trangle_alpha(dc_dest, &_dt, dc_src, &_tt, x1, y1, x2, y2,
				btransparent, alpha);
	}
	return grOk;
}

int
putimage_rotate(IMAGE* imgdest, IMAGE* imgtexture, int nXOriginDest,
	int nYOriginDest, float centerx, float centery, float radian,
	int btransparent, // transparent (1) or not (0)
	int alpha, // in range[0, 256], alpha==256 means no alpha
	int smooth)
{
	auto dc_dest(CONVERT_IMAGE(imgdest));
	auto dc_src(imgtexture);

	if(dc_dest)
	{
		triangle2d _tt[2];
		triangle2d _dt[2];
		double dx, dy, cr = cos(radian), sr = -sin(radian);
		int i, j;

		_tt[0].p[0].x = 0;
		_tt[0].p[0].y = 0;
		_tt[0].p[1].x = 0;
		_tt[0].p[1].y = 1;
		_tt[0].p[2].x = 1;
		_tt[0].p[2].y = 1;
		_tt[1].p[0] = _tt[0].p[2];
		_tt[1].p[1].x = 1;
		_tt[1].p[1].y = 0;
		_tt[1].p[2] = _tt[0].p[0];
		std::memcpy(&_dt, &_tt, sizeof(triangle2d) * 2);
		for(j = 0; j < 2; ++j)
			for(i = 0; i < 3; ++i)
			{
				_dt[j].p[i].x = (_dt[j].p[i].x - centerx)
					* (dc_src->getwidth());
				_dt[j].p[i].y = (_dt[j].p[i].y - centery)
					* (dc_src->getheight());
				dx = cr * _dt[j].p[i].x - sr * _dt[j].p[i].y;
				dy = sr * _dt[j].p[i].x + cr * _dt[j].p[i].y;
				_dt[j].p[i].x = float(float2int(float((dx + nXOriginDest)
					+ FLT_EPSILON)));
				_dt[j].p[i].y = float(float2int(float((dy + nYOriginDest)
					+ FLT_EPSILON)));
			}
		putimage_trangle(dc_dest, imgtexture, &_dt[0], &_tt[0], btransparent,
			alpha, smooth);
		putimage_trangle(dc_dest, imgtexture, &_dt[1], &_tt[1], btransparent,
			alpha, smooth);
	}
	return grOk;
}

int
putimage_rotatezoom(IMAGE* imgdest, IMAGE* imgtexture, int nXOriginDest,
	int nYOriginDest, float centerx, float centery, float radian,
	float zoom, int btransparent, // transparent (1) or not (0)
	int alpha, // in range[0, 256], alpha==256 means no alpha
	int smooth)
{
	auto dc_dest = CONVERT_IMAGE(imgdest);
	auto dc_src = imgtexture;

	if(dc_dest)
	{
		triangle2d _tt[2];
		triangle2d _dt[2];
		double dx, dy, cr = cos(radian), sr = -sin(radian);
		int i, j;
		_tt[0].p[0].x = 0;
		_tt[0].p[0].y = 0;
		_tt[0].p[1].x = 0;
		_tt[0].p[1].y = 1;
		_tt[0].p[2].x = 1;
		_tt[0].p[2].y = 1;
		_tt[1].p[0] = _tt[0].p[2];
		_tt[1].p[1].x = 1;
		_tt[1].p[1].y = 0;
		_tt[1].p[2] = _tt[0].p[0];
		std::memcpy(&_dt, &_tt, sizeof(triangle2d) * 2);
		for(j = 0; j < 2; ++j)
			for(i = 0; i < 3; ++i)
			{
				_dt[j].p[i].x = (_dt[j].p[i].x - centerx) * (dc_src->getwidth());
				_dt[j].p[i].y = (_dt[j].p[i].y - centery) * (dc_src->getheight());
				dx = cr * _dt[j].p[i].x - sr * _dt[j].p[i].y;
				dy = sr * _dt[j].p[i].x + cr * _dt[j].p[i].y;
				_dt[j].p[i].x = float(float2int(float((dx * zoom + nXOriginDest) + FLT_EPSILON)));
				_dt[j].p[i].y = float(float2int(float((dy * zoom + nYOriginDest) + FLT_EPSILON)));
			}
		putimage_trangle(dc_dest, imgtexture, &_dt[0], &_tt[0], btransparent,
			alpha, smooth);
		putimage_trangle(
			dc_dest,
			imgtexture,
			&_dt[1],
			&_tt[1],
			btransparent,
			alpha,
			smooth
		);
	}
	return grOk;
}

} // namespace ege;
