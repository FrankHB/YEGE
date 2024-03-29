﻿#include "image.h"
#include <cmath>
#include <cstring>
#include <cfloat> // for FLT_EPSILON;
#include "ege/gdi.h"
#include "ege/text.h"
#include "global.h"
#include "ege/gapi.h"
#include "ege/gapi_aa.h"
#include "ege/img.h"
#include "ege/windows.h" // for ege::MBCSToWCS;
#if !YEGE_Use_YSLib
#	include <ocidl.h>
#	include <olectl.h>
#	include <png.h>
#	include <vector>
#	include <cstring> // for std::wcsstr;
#endif

namespace ege
{

#if !YEGE_Use_YSLib
using std::vector;
#else
using YSLib::vector;
#endif

namespace
{

::HBITMAP g_hbmp_def;
::HBRUSH g_hbr_def;
::HPEN g_pen_def;
::HFONT g_font_def;


struct WDC
{
	::HDC dc;

	WDC()
		: dc(::GetDC({}))
	{}
	~WDC()
	{
		::ReleaseDC({}, dc);
	}
};

#define YEGE_GetDCPrototype() WDC().dc
//	get_pages().get_image_context();

} // unnamed namespace;

IMAGE::IMAGE()
	: IMAGE(1, 1)
{}
IMAGE::IMAGE(int width, int height)
	: IMAGE(ToSize(width, height))
{}
IMAGE::IMAGE(const Size& size)
	: IMAGE(YEGE_GetDCPrototype(), size)
{}
IMAGE::IMAGE(::HDC hdc, int width, int height)
	: IMAGE(hdc, ToSize(width, height))
{}
IMAGE::IMAGE(::HDC hdc, const Size& size)
	: sbuf(size), m_hDC([hdc]{
		return ::CreateCompatibleDC(Nonnull(hdc));
	}())
{
	if(m_hDC)
	{
		Resize(size);
		setcolor(LIGHTGRAY, this);
		setbkcolor_f(BLACK, this);
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
	: IMAGE(img.m_hDC, img.GetSize())
{
	sbuf.UpdateFrom(img.sbuf.GetBufferPtr());
	::BitBlt(m_hDC, 0, 0, int(img.GetWidth()), int(img.GetHeight()), img.m_hDC,
		0, 0, SRCCOPY);
}
IMAGE::IMAGE(IMAGE&& img) ynothrow
	: sbuf(img.sbuf.GetSize())
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
IMAGE::SetViewport(int left, int top, int right, int bottom, int clip)
{
	::SetViewportOrgEx(getdc(), 0, 0, {});
	m_vpt.left = left;
	m_vpt.top = top;
	m_vpt.right = right;
	m_vpt.bottom = bottom;
	m_vpt.clipflag = clip;
	if(m_vpt.left < 0)
		m_vpt.left = 0;
	if(m_vpt.top < 0)
		m_vpt.top = 0;
	if(m_vpt.right > int(GetWidth()))
		m_vpt.right = int(GetWidth());
	if(m_vpt.bottom > int(GetHeight()))
		m_vpt.bottom = int(GetHeight());

	auto rgn(m_vpt.clipflag ? ::CreateRectRgn(m_vpt.left, m_vpt.top,
		m_vpt.right, m_vpt.bottom) : ::CreateRectRgn(0, 0, int(GetWidth()),
		int(GetHeight())));

	::SelectClipRgn(getdc(), rgn);

	::DeleteObject(rgn);
	//OffsetViewportOrgEx(getdc(), m_vpt.left, m_vpt.top, {});
	::SetViewportOrgEx(getdc(), m_vpt.left, m_vpt.top, {});
}

void
IMAGE::swap(IMAGE& img) ynothrow
{
	std::swap(sbuf, img.sbuf);
	std::swap(m_hDC, img.m_hDC);
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
	m_texture.reset(gen ? new Gdiplus::Bitmap(int(GetWidth()), int(GetHeight()),
		int(GetWidth() * 4), PixelFormat32bppARGB,
		reinterpret_cast<byte*>(getbuffer())) : nullptr);
}

int
IMAGE::Refresh(::HBITMAP bitmap)
{
	if(bitmap)
	{
		const auto hbmp_def = ::HBITMAP(::SelectObject(m_hDC, bitmap));

		if(!g_hbmp_def)
		{
			g_hbmp_def = hbmp_def;
			g_hbr_def = ::HBRUSH(::GetCurrentObject(m_hDC, OBJ_BRUSH));
			g_pen_def = ::HPEN(::GetCurrentObject(m_hDC, OBJ_PEN));
			g_font_def = ::HFONT(::GetCurrentObject(m_hDC, OBJ_FONT));
		}
		::DeleteObject(hbmp_def);
		SetViewport(0, 0, int(GetWidth()), int(GetHeight()), 1);
		cleardevice(this);
	}
	else
	{
		const auto err(::GetLastError());

		::DeleteDC(m_hDC);
		return int(err);
	}
	return 0;
}

int
IMAGE::Resize(const Size& size)
{
	std::memset(&m_vpt, 0, sizeof(m_vpt));

	yassume(m_hDC);
	if(GetSize() != size)
		sbuf = ScreenBuffer(size);
	return Refresh(GetBitmap());
}

int
IMAGE::getpngimg(std::FILE* fp)
{
#if YEGE_Use_YSLib
	std::rewind(fp);
	try
	{
		// XXX: Use encapsulated file size getter.
		const auto fsize(platform::FileDescriptor(::_fileno(fp)).GetSize());

		if(fsize != std::uint32_t(fsize))
			return grIOerror;

		vector<octet> buf(static_cast<size_t>(fsize));
		ystdex::ifile_iterator i(fp);

		buf.assign(i, ystdex::ifile_iterator());

		HBitmap bitmap(ImageMemory(std::move(buf)));
		const Size& s(bitmap.GetSize());

		Resize(s);

		// XXX: Use raw conversion without buffer to improve performance.
		std::copy_n(CompactPixmap(bitmap).GetBufferPtr(),
			s.Width * s.Height, sbuf.GetBufferPtr());
		return grOk;
	}
	catch(std::exception&)
	{}
	return grIOerror;
#else
	::png_structp pic_ptr;
	::png_infop info_ptr;
	std::uint32_t width, height;
	int depth;

	{
		char header[16];
		std::uint32_t number = 8;
		std::fread(header, 1, number, fp);
		int isn_png = ::png_sig_cmp((::png_const_bytep)header, 0, number);

		if(isn_png)
			return grIOerror;
		std::fseek(fp, 0, SEEK_SET);
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
	::png_read_png(pic_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_EXPAND,
		{});
	::png_set_expand(pic_ptr);

	int color_type_;

	::png_get_IHDR(pic_ptr, info_ptr, &width, &height, &depth, &color_type_,
		{}, {}, {});

	Resize(int(width), int(height));

	::png_bytepp row_pointers = ::png_get_rows(pic_ptr, info_ptr);

	for(std::uint32_t i = 0; i < height; ++i)
	{
		const auto m_pBuffer(GetBufferPtr());

		if(depth == 24)
			for(std::uint32_t j = 0; j < width; ++j)
				m_pBuffer[i * width + j] = 0xFFFFFF
					& (unsigned long&)row_pointers[i][j * 3];
		else if(depth == 32)
			for(std::uint32_t j = 0; j < width; ++j)
			{
				m_pBuffer[i * width + j]
					= ((unsigned long*)(row_pointers[i]))[j];
				if((m_pBuffer[i * width + j] & 0xFF000000) == 0)
					m_pBuffer[i * width + j] = 0;
			}
	}
	::png_destroy_read_struct(&pic_ptr, &info_ptr, {});
	return 0;
#endif
}

void
IMAGE::getimage(const IMAGE* pSrcImg, int srcX, int srcY, int srcWidth,
	int srcHeight)
{
	if(Resize(srcWidth, srcHeight) == 0)
		::BitBlt(m_hDC, 0, 0, srcWidth, srcHeight,
			cimg_ref_c(pSrcImg).m_hDC, srcX, srcY, SRCCOPY);
}
graphics_errors
IMAGE::getimage(::HRSRC hrsrc)
{
	if(const auto hg = ::LoadResource({}, hrsrc))
		if(const auto pvRes = ::LockResource(hg))
			return getimage(pvRes, ::SizeofResource({}, hrsrc));
	return grIOerror;
}
graphics_errors
IMAGE::getimage(void* pMem, unsigned long dwSize)
{
	assert(pMem);
#if YEGE_Use_YSLib
	try
	{
		const auto p(static_cast<ystdex::octet*>(pMem));

		return getimage_b(
			HBitmap(ImageMemory(vector<octet>(p, p + size_t(dwSize)))));
	}
	catch(std::bad_alloc&)
	{
		return grAllocError;
	}
	catch(std::exception&)
	{}
	return grIOerror;
#else
	if(pMem)
	{
		auto hGlobal(::GlobalAlloc(GMEM_MOVEABLE, dwSize));
		void* pvData;

		if(!hGlobal || !(pvData = ::GlobalLock(hGlobal)))
			return grAllocError;
		std::memcpy(pvData, pMem, dwSize);
		::GlobalUnlock(hGlobal);

		::IStream* pStm;

		if(S_OK != ::CreateStreamOnHGlobal(hGlobal, TRUE, &pStm))
			return grNullPointer;

		::IPicture* pPicture;

		const auto hr(::OleLoadPicture(pStm, long(dwSize), TRUE,
			::IID_IPicture, reinterpret_cast<void**>(&pPicture)));

		::GlobalFree(hGlobal);
		if(!FAILED(hr))
			return getimage_b(pPicture);
	}
	return grIOerror;
#endif
}
graphics_errors
IMAGE::getimage(const char* filename)
{
	if(getimage_pngfile(this, filename) == 0)
		return grOk;
	return getimage(ege::MBCSToWCS(filename).c_str());
}
#if YEGE_Use_YSLib
graphics_errors
IMAGE::getimage(const wchar_t* filename)
{
	try
	{
		// XXX: URL unsupported.
		return getimage_b(HBitmap(reinterpret_cast<const char16_t*>(filename)));
	}
	catch(std::exception&)
	{}
	return grIOerror;
}
#else
graphics_errors
IMAGE::getimage(const wchar_t* filename)
{
	if(getimage_pngfile(this, filename) == 0)
		return grOk;

	wchar_t szPath[MAX_PATH * 2 + 1] = L"";

	if(std::wcsstr(filename, L"http://"))
		::lstrcpyW(szPath, filename);
	else if(filename[1] == ':')
		::lstrcpyW(szPath, filename);
	else
	{
		::GetCurrentDirectoryW(MAX_PATH * 2, szPath);
		::lstrcatW(szPath, L"\\");
		::lstrcatW(szPath, filename);
	}

	::IPicture* pPicture;
	// XXX: Only 'wchar_t' is supported here.
	::HRESULT hr = ::OleLoadPicturePath(szPath, {}, 0, 0, ::IID_IPicture,
		reinterpret_cast<void**>(&pPicture));

	if(FAILED(hr))
		return grIOerror;
	return getimage_b(pPicture);
}
#endif
graphics_errors
IMAGE::getimage(const char* pResType, const char* pResName)
{
	if(const auto hrsrc = ::FindResourceA(EGEApplication::GetInstance(),
		pResName, pResType))
		return getimage(hrsrc);
	return grIOerror;
}
graphics_errors
IMAGE::getimage(const wchar_t* pResType, const wchar_t* pResName)
{
	if(const auto hrsrc = ::FindResourceW(EGEApplication::GetInstance(),
		pResName, pResType))
		return getimage(hrsrc);
	return grIOerror;
}

#if YEGE_Use_YSLib
graphics_errors
IMAGE::getimage_b(HBitmap&& bitmap)
{
	try
	{
		const Size& s(bitmap.GetSize());

		Resize(s);

		// XXX: Use raw conversion without buffer to improve performance.
		std::copy_n(CompactPixmap(bitmap).GetBufferPtr(),
			s.Width * s.Height, sbuf.GetBufferPtr());
		return grOk;
	}
	catch(std::bad_alloc&)
	{
		return grAllocError;
	}
	catch(std::exception&)
	{}
	return grIOerror;
}
#else
graphics_errors
IMAGE::getimage_b(void* p_pic)
{
	auto& pic(*static_cast<::IPicture*>(p_pic));
	long lWidth, lHeight;

	pic.get_Width(&lWidth);
	pic.get_Height(&lHeight);

	const auto sdc(::GetDC({}));
	const long lWidthPixels(::MulDiv(lWidth, ::GetDeviceCaps(sdc, LOGPIXELSX),
		2540)), lHeightPixels(
		::MulDiv(lHeight, ::GetDeviceCaps(sdc, LOGPIXELSY), 2540));

	::ReleaseDC({}, sdc);
	Resize(lWidthPixels, lHeightPixels);
	pic.Render(m_hDC, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight,
		lWidth, -lHeight, {});
	pic.Release();
	return grOk;
}
#endif

void
IMAGE::putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	int srcX, int srcY, int srcWidth, int srcHeight, unsigned long dwRop) const
{
	const auto img_dc(cimg_ref(pDstImg).m_hDC);

	::SetStretchBltMode(img_dc, COLORONCOLOR);
	::StretchBlt(img_dc, dstX, dstY, dstWidth, dstHeight, m_hDC, srcX, srcY,
		srcWidth, srcHeight, dwRop);
}
void
IMAGE::putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	int srcX, int srcY, unsigned long dwRop) const
{
	::BitBlt(cimg_ref(pDstImg).m_hDC, dstX, dstY, dstWidth, dstHeight,
		m_hDC, srcX, srcY, dwRop);
}
void
IMAGE::putimage(IMAGE* pDstImg, int dstX, int dstY, unsigned long dwRop) const
{
	putimage(pDstImg, dstX, dstY, int(GetWidth()), int(GetHeight()), 0, 0,
		dwRop);
}
void
IMAGE::putimage(int dstX, int dstY, int dstWidth, int dstHeight, int srcX,
	int srcY, unsigned long dwRop) const
{
	putimage(&cimg_ref(static_cast<IMAGE*>(nullptr)), dstX, dstY,
		dstWidth, dstHeight, srcX, srcY, dwRop);
}
void
IMAGE::putimage(int dstX, int dstY, unsigned long dwRop) const
{
	putimage(&cimg_ref(static_cast<IMAGE*>(nullptr)), dstX, dstY, dwRop);
}

#if YEGE_Use_YSLib
int
IMAGE::saveimage(const char* filename, ImageFormat fmt) const
{
	return saveimage(ege::MBCSToWCS(filename).c_str(), fmt);
}
int
IMAGE::saveimage(const wchar_t* filename, ImageFormat fmt) const
{
	const Size s(GetSize());

	try
	{
		HBitmap(HBitmap(sbuf.GetBufferPtr(), sbuf.GetSize()), 24).SaveTo(
			reinterpret_cast<const char16_t*>(filename), fmt);
		return grOk;
	}
	catch(std::exception&)
	{}
	return grIOerror;
}

#else
namespace
{

int
saveimagetofile(const IMAGE* pimg, std::FILE* fp)
{
	auto bmpfHead = ::BITMAPFILEHEADER();
	auto bmpinfo = ::BITMAPINFOHEADER();
	auto& img(Deref(pimg));
	auto pitch = img.GetWidth() * 3;
	int addbit, y, x, zero = 0;

	if((pitch & 3) != 0)
	{
		addbit = 4 - (pitch & 3);
		pitch = (pitch + 4) & unsigned(~3);
	}
	else
		addbit = 0;
	bmpfHead.bfType = *reinterpret_cast<const unsigned short*>("BM");
	bmpfHead.bfOffBits
		= sizeof(::BITMAPFILEHEADER) + sizeof(::BITMAPINFOHEADER);
	bmpfHead.bfSize = sizeof(::BITMAPFILEHEADER) + sizeof(::BITMAPINFOHEADER)
		+ pitch * img.GetHeight();
	bmpinfo.biSize = sizeof(::BITMAPINFOHEADER);
	bmpinfo.biBitCount = 24;
	bmpinfo.biHeight = long(img.GetHeight());
	bmpinfo.biWidth = long(img.GetWidth());
	bmpinfo.biPlanes = 1;
	bmpinfo.biSizeImage = pitch * img.GetHeight();
	//bmpinfo.biXPelsPerMeter
	std::fwrite(&bmpfHead, sizeof(bmpfHead), 1, fp);
	std::fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);

	const SDst height(img.GetHeight()), width(img.GetWidth());

	for(y = SPos(height) - 1; y >= 0; --y)
	{
		for(x = 0; x < SPos(width); ++x)
		{
			const auto col(img.getbuffer()[SDst(y) * width + SDst(x)]);

			if(std::fwrite(&col, 3, 1, fp) < 1)
				goto ERROR_BREAK;
		}
		if(addbit > 0)
			std::fwrite(&zero, size_t(addbit), 1, fp);
	}
	return 0;
ERROR_BREAK:
	return grIOerror;
}

} // unnamed namespace;

int
IMAGE::saveimage(const char* filename) const
{
	std::FILE* fp = std::fopen(filename, "wb");

	if(!fp)
		return grIOerror;

	int ret = saveimagetofile(this, fp);

	std::fclose(fp);
	return ret;
}

int
IMAGE::saveimage(const wchar_t* filename) const
{
	std::FILE* fp = ::_wfopen(filename, L"wb");

	if(!fp)
		return grIOerror;

	int ret = saveimagetofile(this, fp);

	std::fclose(fp);
	return ret;
}

int
IMAGE::savepngimg(std::FILE* fp, int bAlpha) const
{
	unsigned long i, j;
	::png_structp pic_ptr;
	::png_infop info_ptr;
	::png_colorp palette;

	pic_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, {}, {}, {});

	std::uint32_t pixelsize = bAlpha ? 4 : 3;
	const SDst width(GetWidth()), height(GetHeight());

	if(!pic_ptr)
		return -1;
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
	::png_set_IHDR(pic_ptr, info_ptr, width, height, 8, bAlpha
		? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	palette = ::png_colorp(::png_malloc(pic_ptr, PNG_MAX_PALETTE_LENGTH
		* sizeof(::png_color)));
	::png_set_PLTE(pic_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
	::png_write_info(pic_ptr, info_ptr);
	::png_set_packing(pic_ptr);
	try
	{
		const unique_ptr<::png_byte[]> p_image(new
			png_byte[width * height * pixelsize * sizeof(::png_byte) + 4]);
		const unique_ptr<::png_bytep[]> p_row_pointers(new png_bytep[height]);
		const auto image(&p_image[0]);
		const auto row_pointers(&p_row_pointers[0]);
		const auto m_pBuffer(GetBufferPtr());

		for(i = 0; i < height; i++)
		{
			for(j = 0; j < width; ++j)
				// XXX: Assume little endiness. PNG stores RGB by default,
				//	which is BGR in little endian integer representations.
				reinterpret_cast<unsigned long&>(image[(i * width  + j)
					* pixelsize]) = RGBTOBGR(m_pBuffer[i * width + j]);
			row_pointers[i] = (::png_bytep)image + i * width * pixelsize;
		}
		::png_write_image(pic_ptr, row_pointers);
		::png_write_end(pic_ptr, info_ptr);
		::png_free(pic_ptr, palette);
		palette = {};
		::png_destroy_write_struct(&pic_ptr, &info_ptr);
	}
	catch(std::bad_alloc&)
	{
		::png_destroy_write_struct(&pic_ptr, &info_ptr);
		return -1;
	}
	return 0;
}
#endif

namespace
{

void
fix_rect_1size(const IMAGE* pdest, const IMAGE* pSrc,
	int* nXOriginDest,  // x-coord of destination upper-left corner
	int* nYOriginDest,  // y-coord of destination upper-left corner
	int* nXOriginSrc,   // x-coord of source upper-left corner
	int* nYOriginSrc,   // y-coord of source upper-left corner
	int* nWidthSrc,     // width of source rectangle
	int* nHeightSrc      // height of source rectangle
)
{
	const auto& _vpt(pdest->m_vpt);

	yunseq(*nXOriginDest += _vpt.left, *nYOriginDest += _vpt.top);
	/* default value proc */
	if(*nWidthSrc == 0)
	{
		*nWidthSrc = int(pSrc->GetWidth());
		*nHeightSrc = int(pSrc->GetHeight());
	}
	/* fix src rect */
	if(*nWidthSrc > int(pSrc->GetWidth()))
	{
		*nWidthSrc -= int(unsigned(*nWidthSrc) - pSrc->GetWidth());
		*nWidthSrc = int(pSrc->GetWidth());
	}
	if(*nHeightSrc > int(pSrc->GetHeight()))
	{
		*nHeightSrc -= int(unsigned(*nHeightSrc) - pSrc->GetHeight());
		*nHeightSrc = int(pSrc->GetHeight());
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
		int dx = *nXOriginDest + *nWidthSrc - _vpt.right;

		*nWidthSrc -= dx;
	}
	if(*nYOriginDest + *nHeightSrc > _vpt.bottom)
	{
		int dy = *nYOriginDest + *nHeightSrc - _vpt.bottom;

		*nHeightSrc -= dy;
	}
}

color_t
bilinear_interpolation(color_t LT, color_t RT, color_t LB, color_t RB,
	double x, double y)
{
	color_t Trb, Tg, Brb, Bg, crb, cg;
	color_t alphaA, alphaB;
	alphaA = (color_t)(x * 0x100);
	alphaB = 0xFF - alphaA;
	Trb = (((LT & 0xFF00FF) * alphaB + (RT & 0xFF00FF) * alphaA) & 0xFF00FF00)
		>> 8;
	Tg = (((LT & 0xFF00) * alphaB + (RT & 0xFF00) * alphaA) & 0xFF0000) >> 8;
	Brb = (((LB & 0xFF00FF) * alphaB + (RB & 0xFF00FF) * alphaA) & 0xFF00FF00)
		>> 8;
	Bg = (((LB & 0xFF00) * alphaB + (RB & 0xFF00) * alphaA) & 0xFF0000) >> 8;
	alphaA = (color_t)(y * 0x100);
	alphaB = 0xFF - alphaA;
	crb = ((Trb * alphaB + Brb * alphaA) & 0xFF00FF00);
	cg = ((Tg * alphaB + Bg * alphaA) & 0xFF0000);
	return (crb | cg) >> 8;
}

void
EGEALPHABLEND(unsigned d, unsigned s, BitmapPtr pd, unsigned char alpha)
	ynothrow
{
	auto rb(d & 0x00FF00FF), g(d & 0x0000FF00);

	rb += ((s & 0x00FF00FF) - rb) * alpha >> 8;
	g += ((s & 0x0000FF00) -  g) * alpha >> 8;
	*pd = (rb & 0x00FF00FF) | (g & 0x0000FF00) | EGEGET_A(d);
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
) const
{
	auto& img(cimg_ref(imgdest));
	const IMAGE* imgsrc = this;
	int y, x;
	SDst ddx, dsx;
	unsigned long cr;

	fix_rect_1size(&img, imgsrc, &nXOriginDest, &nYOriginDest, &nXOriginSrc,
		&nYOriginSrc, &nWidthSrc, &nHeightSrc);

	auto pdp = img.GetBufferPtr() + unsigned(nYOriginDest) * img.GetWidth()
		+ nXOriginDest;
	auto psp = imgsrc->GetBufferPtr() + unsigned(nYOriginSrc)
		* imgsrc->GetWidth() + nXOriginSrc;

	ddx = img.GetWidth() - unsigned(nWidthSrc);
	dsx = imgsrc->GetWidth() - unsigned(nWidthSrc);
	cr = crTransparent & 0x00FFFFF;
	for(y = 0; y < nHeightSrc; ++y)
	{
		for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
			if((*psp & 0x00FFFFF) != cr)
				*pdp = EGECOLORA(*psp, EGEGET_A(*pdp));
		pdp += ddx;
		psp += dsx;
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
) const
{
	auto& img(cimg_ref(imgdest));
	const IMAGE* imgsrc = this;
	int y, x;
	unsigned long ddx, dsx;

	fix_rect_1size(&img, imgsrc, &nXOriginDest, &nYOriginDest, &nXOriginSrc,
		&nYOriginSrc, &nWidthSrc, &nHeightSrc);

	auto pdp = img.GetBufferPtr() + unsigned(nYOriginDest) * img.GetWidth()
		+ nXOriginDest;
	auto psp = imgsrc->GetBufferPtr() + unsigned(nYOriginSrc)
		* imgsrc->GetWidth() + nXOriginSrc;

	ddx = img.GetWidth() - unsigned(nWidthSrc);
	dsx = imgsrc->GetWidth() - unsigned(nWidthSrc);
	for(y = 0; y < nHeightSrc; ++y)
	{
		for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
			EGEALPHABLEND(*pdp, *psp, pdp, alpha);
		pdp += ddx;
		psp += dsx;
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
) const
{
	auto& img(cimg_ref(imgdest));
	const IMAGE* imgsrc = this;
	int y, x;
	unsigned long ddx, dsx;
	unsigned long cr;

	fix_rect_1size(&img, imgsrc, &nXOriginDest, &nYOriginDest, &nXOriginSrc,
		&nYOriginSrc, &nWidthSrc, &nHeightSrc);

	auto pdp = img.GetBufferPtr() + unsigned(nYOriginDest) * img.GetWidth()
		+ nXOriginDest;
	auto psp = imgsrc->GetBufferPtr()
		+ unsigned(nYOriginSrc) * imgsrc->GetWidth() + nXOriginSrc;

	ddx = img.GetWidth() - unsigned(nWidthSrc);
	dsx = imgsrc->GetWidth() - unsigned(nWidthSrc);
	cr = crTransparent & 0xFFFFFF;
	for(y = 0; y < nHeightSrc; ++y)
	{
		for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
		{
			if((*psp & 0x00FFFFFF) != cr)
				EGEALPHABLEND(*pdp, *psp, pdp, alpha);
		}
		pdp += ddx;
		psp += dsx;
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
) const
{
	auto& img(cimg_ref(imgdest));
	const IMAGE* imgsrc = this;
	int y, x;
	unsigned long ddx, dsx;

	fix_rect_1size(&img, imgsrc, &nXOriginDest, &nYOriginDest, &nXOriginSrc,
		&nYOriginSrc, &nWidthSrc, &nHeightSrc);

	auto pdp = img.GetBufferPtr() + unsigned(nYOriginDest) * img.GetWidth()
		+ nXOriginDest;
	auto psp = imgsrc->GetBufferPtr()
		+ unsigned(nYOriginSrc) * imgsrc->GetWidth() + nXOriginSrc;

	ddx = img.GetWidth() - unsigned(nWidthSrc);
	dsx = imgsrc->GetWidth() - unsigned(nWidthSrc);

	for(y = 0; y < nHeightSrc; ++y)
	{
		for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp)
			EGEALPHABLEND(*pdp, *psp, pdp, EGEGET_A(*psp));
		pdp += ddx;
		psp += dsx;
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
) const
{
	auto& img(cimg_ref(imgdest));
	const IMAGE* imgsrc = this;
	int y, x;
	unsigned long ddx, dsx;
	//unsigned long sa = unsigned(alpha) + 1, da = 0xFF - alpha;

	fix_rect_1size(&img, imgsrc, &nXOriginDest, &nYOriginDest, &nXOriginSrc,
		&nYOriginSrc, &nWidthSrc, &nHeightSrc);

	auto pdp = img.GetBufferPtr() + unsigned(nYOriginDest) * img.GetWidth()
		+ nXOriginDest;
	auto psp = imgsrc->GetBufferPtr()
		+ unsigned(nYOriginSrc) * imgsrc->GetWidth() + nXOriginSrc;
	auto pap = imgalpha->GetBufferPtr()
		+ unsigned(nYOriginSrc) * imgalpha->GetWidth() + nXOriginSrc;

	ddx = img.GetWidth() - unsigned(nWidthSrc);
	dsx = imgsrc->GetWidth() - unsigned(nWidthSrc);
	for(y = 0; y < nHeightSrc; ++y)
	{
		for(x = 0; x < nWidthSrc; ++x, ++psp, ++pdp, ++pap)
			if(*pap != 0)
				EGEALPHABLEND(*pdp, *psp, pdp, *pap & 0xFF);
		pdp += ddx;
		psp += dsx;
		pap += dsx;
	}
	return grOk;
}


namespace
{

void
fix_rect_0size(IMAGE* pdest,
	int* nXOriginDest,  // x-coord of destination upper-left corner
	int* nYOriginDest,  // y-coord of destination upper-left corner
	int* nWidthDest,     // width of destination rectangle
	int* nHeightDest      // height of destination rectangle
)
{
	viewporttype _vpt{0, 0, int(pdest->GetWidth()), int(pdest->GetHeight()), 0};

	if(*nWidthDest == 0)
		*nWidthDest = int(pdest->GetWidth());
	if(*nHeightDest == 0)
		*nHeightDest = int(pdest->GetHeight());
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
	std::unique_ptr<unsigned long[]> buff(new unsigned long[8 << 10]);
	int x2, y2, ix, iy;
	color_int_t lsum;
	unsigned long sumRB, sumG;
	SPos ddx, dldx;
	int centerintensity;
	int intensity2, intensity3, intensity4;
	int intensity2f, intensity3f, intensity4f;
	IMAGE* imgdest = this;

	x2 = nXOriginDest + nWidthDest - 1;
	y2 = nYOriginDest + nHeightDest - 1;
	auto pdp = imgdest->GetBufferPtr()
		+ unsigned(nYOriginDest) * imgdest->GetWidth() + nXOriginDest;
	ddx = SPos(imgdest->GetWidth()) - nWidthDest;
	dldx = SPos(imgdest->GetWidth());
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
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (pdp[dldx] & 0xFF00FF)
				+ (pdp[1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (pdp[dldx] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF) + (pdp[dldx] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (pdp[dldx] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	for(iy = nYOriginDest + 1; iy < y2; ++iy)
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[SDst(ix)] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF)
				+ (pdp[1] & 0xFF00FF);
			sumG = (buff[SDst(ix)] & 0xFF00) + (pdp[dldx] & 0xFF00)
				+ (pdp[1] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
				+ (pdp[dldx] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[SDst(ix)] & 0xFF00)
				+ (pdp[dldx] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity4)
				+ ((sumRB * unsigned(intensity4f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity4)
				+ ((sumG * unsigned(intensity4f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
				+ (pdp[dldx] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[SDst(ix)] & 0xFF00)
				+ (pdp[dldx] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[SDst(ix)] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (buff[SDst(ix)] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
				+ (pdp[1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[SDst(ix)] & 0xFF00) + (pdp[1] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (buff[SDst(ix)] & 0xFF00);
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		//pdp += ddx;
	}
	return grOk;
}

int
IMAGE::imagefilter_blurring_8(int intensity, int alpha, int nXOriginDest,
	int nYOriginDest, int nWidthDest, int nHeightDest)
{
	std::unique_ptr<unsigned long[]> buff(new unsigned long[8 << 10]);
	int x2, y2, ix, iy;
	color_int_t lsum, sumRB, sumG, lbuf;
	SPos ddx, dldx;
	int centerintensity;
	int intensity2, intensity3, intensity4;
	int intensity2f, intensity3f, intensity4f;

	IMAGE* imgdest = this;

	x2 = nXOriginDest + nWidthDest - 1;
	y2 = nYOriginDest + nHeightDest - 1;

	auto pdp = imgdest->GetBufferPtr()
		+ unsigned(nYOriginDest) * imgdest->GetWidth() + nXOriginDest;

	ddx = SPos(imgdest->GetWidth()) - nWidthDest;
	dldx = SPos(imgdest->GetWidth());

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
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lsum & 0xFF00FF) + (pdp[1] & 0xFF00FF)
				+ (pdp[dldx - 1] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF)
				+ (pdp[dldx + 1] & 0xFF00FF);
			sumG = (lsum & 0xFF00) + (pdp[1] & 0xFF00)
				+ (pdp[dldx - 1] & 0xFF00) + (pdp[dldx] & 0xFF00)
				+ (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum & 0xFF00FF)
					+ (pdp[dldx - 1] & 0xFF00FF) + (pdp[dldx] & 0xFF00FF);
			sumG = (lsum & 0xFF00)
				   + (pdp[dldx - 1] & 0xFF00) + (pdp[dldx] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	for(iy = nYOriginDest + 1; iy < y2; ++iy)
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[SDst(ix)] & 0xFF00FF)
				+ (buff[SDst(ix) + 1] & 0xFF00FF) + (pdp[1] & 0xFF00FF)
				+ (pdp[dldx] & 0xFF00FF) + (pdp[dldx + 1] & 0xFF00FF);
			sumG = (buff[SDst(ix)] & 0xFF00) + (buff[SDst(ix) + 1] & 0xFF00)
				+ (pdp[1] & 0xFF00) + (pdp[dldx] & 0xFF00)
				+ (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
				+ (buff[SDst(ix) + 1] & 0xFF00FF) + (lsum & 0xFF00FF)
				+ (pdp[1] & 0xFF00FF) + (pdp[dldx - 1] & 0xFF00FF)
				+ (pdp[dldx] & 0xFF00FF) + (pdp[dldx + 1] & 0xFF00FF);
			sumG = (lbuf & 0xFF00) + (buff[SDst(ix)] & 0xFF00)
				+ (buff[SDst(ix) + 1] & 0xFF00) + (lsum & 0xFF00)
				+ (pdp[1] & 0xFF00) + (pdp[dldx - 1] & 0xFF00)
				+ (pdp[dldx] & 0xFF00) + (pdp[dldx + 1] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity4)
				+ ((sumRB * unsigned(intensity4f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity4)
				+ ((sumG * unsigned(intensity4f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
				+ (lsum & 0xFF00FF) + (lbuf & 0xFF00FF)
				+ (pdp[dldx] & 0xFF00FF);
			sumG = (buff[SDst(ix) - 1] & 0xFF00) + (buff[SDst(ix)] & 0xFF00)
				+ (lsum & 0xFF00) + (pdp[SDst(dldx) - 1] & 0xFF00)
				+ (pdp[dldx] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[SDst(ix)] & 0xFF00FF)
				+ (buff[SDst(ix) + 1] & 0xFF00FF) + (pdp[1] & 0xFF00FF);
			sumG = (buff[SDst(ix)] & 0xFF00) + (buff[SDst(ix) + 1] & 0xFF00)
				   + (pdp[1] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity2)
				+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity2)
				+ ((sumG * unsigned(intensity2f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for(ix = nXOriginDest + 1; ix < x2; ++ix)
		{
			sumRB = (lbuf & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
				+ (buff[SDst(ix) + 1] & 0xFF00FF) + (lsum & 0xFF00FF)
				+ (pdp[1] & 0xFF00FF);
			sumG = (lbuf & 0xFF00) + (buff[SDst(ix)] & 0xFF00)
				+ (buff[SDst(ix) + 1] & 0xFF00) + (lsum & 0xFF00)
				+ (pdp[1] & 0xFF00);
			lbuf = buff[SDst(ix)];
			buff[SDst(ix)] = lsum = *pdp;
			sumRB = sumRB * unsigned(intensity3)
				+ ((sumRB * unsigned(intensity3f) >> 8) & 0xFF00FF)
				+ (lsum & 0xFF00FF) * unsigned(centerintensity);
			sumG = sumG * unsigned(intensity3)
				+ ((sumG * unsigned(intensity3f) >> 8))
				+ (lsum & 0xFF00) * unsigned(centerintensity);
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		sumRB = (lbuf & 0xFF00FF) + (buff[SDst(ix)] & 0xFF00FF)
			+ (lsum & 0xFF00FF);
		sumG = (lbuf & 0xFF00) + (buff[SDst(ix)] & 0xFF00) + (lsum & 0xFF00);
		lbuf = buff[SDst(ix)];
		buff[SDst(ix)] = lsum = *pdp;
		sumRB = sumRB * unsigned(intensity2)
			+ ((sumRB * unsigned(intensity2f) >> 8) & 0xFF00FF)
			+ (lsum & 0xFF00FF) * unsigned(centerintensity);
		sumG = sumG * unsigned(intensity2)
			+ ((sumG * unsigned(intensity2f) >> 8))
			+ (lsum & 0xFF00) * unsigned(centerintensity);
		*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
		++pdp;
		//pdp += ddx;
	}
	return grOk;
}

int
IMAGE::imagefilter_blurring(int intensity, int alpha, int nXOriginDest,
	int nYOriginDest, int nWidthDest, int nHeightDest)
{
	IMAGE* imgdest = this;

	fix_rect_0size(imgdest, &nXOriginDest, &nYOriginDest, &nWidthDest,
		&nHeightDest);
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
IMAGE::putimage_rotate(const IMAGE* imgtexture, int nXOriginDest,
	int nYOriginDest, float centerx, float centery, float radian,
	int btransparent, // transparent (1) or not (0)
	int alpha, // in range[0, 256], alpha== -1 means no alpha
	int smooth)
{
	return ege::putimage_rotate(this, imgtexture, nXOriginDest, nYOriginDest,
		centerx, centery, radian, btransparent, alpha, smooth);
}

int
IMAGE::putimage_rotatezoom(const IMAGE* imgtexture, int nXOriginDest,
	int nYOriginDest, float centerx, float centery, float radian,
	float zoom, int btransparent, // transparent (1) or not (0)
	int alpha, // in range[0, 256], alpha== -1 means no alpha
	int smooth)
{
	return ege::putimage_rotatezoom(this, imgtexture, nXOriginDest,
		nYOriginDest, centerx, centery, radian, zoom, btransparent, alpha,
		smooth);
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
	return f >= 0.F ? int(f + .5F) : int(f - .5F);
}

void
draw_flat_scanline(IMAGE* dc_dest, const vector2d* vt, const IMAGE* dc_src,
	const vector2d* svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int(vt->p[0].x), e = float2int(vt->p[1].x),
		y = float2int(vt->p[0].y), w = e - s;
	auto p_dest_bmp_byte
		= reinterpret_cast<unsigned long*>(dc_dest->getbuffer());
	auto p_src_bmp_byte
		= reinterpret_cast<unsigned long*>(dc_src->getbuffer());
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();

	if(w > 0)
	{
		int i, bx = s;
		vector2d _svt = *svt;

		_svt.p[0].x += (s - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x + .5F, cury = _svt.p[0].y + .5F,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(s < x1)
			s = x1;
		if(e > x2)
			e = x2;
		curx += (s - bx) * dx;
		cury += (s - bx) * dy;
		for(i = s; i < e; ++i, curx += dx, cury += dy)
			p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)]
				= p_src_bmp_byte[src_w * SDst(cury) + SDst(curx)];
	}
}

void
draw_flat_scanline_transparent(IMAGE* dc_dest, const vector2d* vt,
	const IMAGE* dc_src, const vector2d* svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = vt->p[0].x + .5F, e = vt->p[1].x + .5F,
		y = vt->p[0].y + .5F, w = e - b;
	auto p_dest_bmp_byte = dc_dest->getbuffer();
	auto p_src_bmp_byte = dc_src->getbuffer();
	unsigned long col;
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x + .5F, cury = _svt.p[0].y + .5F,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;
		for(i = b; i < e; yunseq(++i, curx += dx, cury += dy))
			if((col = p_src_bmp_byte[src_w * SDst(cury) + SDst(curx)]))
				p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = col;
	}
}

void
draw_flat_scanline_alpha(IMAGE* dc_dest, const vector2d* vt,
	const IMAGE* dc_src, const vector2d* svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = float2int(vt->p[0].x), e = float2int(vt->p[1].x),
		y = float2int(vt->p[0].y), w = e - b;
	unsigned long* p_dest_bmp_byte = dc_dest->getbuffer();
	unsigned long* p_src_bmp_byte = dc_src->getbuffer();
	unsigned long sa = unsigned(alpha), da = 0xFF - sa;
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x + .5F, cury = _svt.p[0].y + .5F,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;

		for(i = b; i < e; ++i, curx += dx, cury += dy)
		{
			unsigned long d = p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)],
				s = p_src_bmp_byte[src_w * SDst(cury) + SDst(curx)];

			d = ((d & 0xFF00FF) * da & 0xFF00FF00)
				| ((d & 0xFF00) * da >> 16 << 16);
			s = ((s & 0xFF00FF) * sa & 0xFF00FF00)
				| ((s & 0xFF00) * sa >> 16 << 16);
			p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = (d + s) >> 8;
		}
	}
}

void
draw_flat_scanline_alphatrans(IMAGE* dc_dest, const vector2d* vt,
	const IMAGE* dc_src, const vector2d* svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = vt->p[0].x + .5F, e = vt->p[1].x + .5F,
		y = vt->p[0].y + .5F, w = e - b;
	unsigned long* p_dest_bmp_byte = dc_dest->getbuffer();
	unsigned long* p_src_bmp_byte = dc_src->getbuffer();
	unsigned long sa = unsigned(alpha), da = 0xFF - sa;
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x + .5F, cury = _svt.p[0].y + .5F,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;
		for(i = b; i < e; ++i, curx += dx, cury += dy)
		{
			unsigned long s = p_src_bmp_byte[src_w * SDst(cury) + SDst(curx)];

			if(s != 0)
			{
				unsigned long d = p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)];
				d = ((d & 0xFF00FF) * da & 0xFF00FF00)
					| ((d & 0xFF00) * da >> 16 << 16);
				s = ((s & 0xFF00FF) * sa & 0xFF00FF00)
					| ((s & 0xFF00) * sa >> 16 << 16);
				p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = (d + s) >> 8;
			}
		}
	}
}

void
draw_flat_scanline_s(IMAGE* dc_dest, const vector2d* vt, const IMAGE* dc_src,
	const vector2d* svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = float2int(vt->p[0].x), e = float2int(vt->p[1].x),
		y = float2int(vt->p[0].y), w = e - b;
	unsigned long* p_dest_bmp_byte = dc_dest->getbuffer();
	unsigned long* p_src_bmp_byte = dc_src->getbuffer();
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x, cury = _svt.p[0].y,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;
		for(i = b; i < e; ++i, curx += dx, cury += dy)
		{
			int ix = curx, iy = cury;
			double fx(curx - ix), fy = cury - iy;
			auto p_src_byte = p_src_bmp_byte + src_w * SDst(iy) + SDst(ix);
			unsigned long col(bilinear_interpolation(p_src_byte[0],
				p_src_byte[1], p_src_byte[src_w], p_src_byte[src_w + 1],
				fx, fy));

			p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = col;
		}
	}
}

void
draw_flat_scanline_transparent_s(IMAGE* dc_dest, const vector2d* vt,
	const IMAGE* dc_src, const vector2d* svt, int x1, int x2)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = float2int(vt->p[0].x), e = float2int(vt->p[1].x),
		y = float2int(vt->p[0].y), w = e - b;
	unsigned long* p_dest_bmp_byte = dc_dest->getbuffer();
	unsigned long* p_src_bmp_byte = dc_src->getbuffer();
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x, cury = _svt.p[0].y,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;
		for(i = b; i < e; ++i, curx += dx, cury += dy)
		{
			int ix(curx), iy(cury);
			float fx(curx - ix), fy = cury - iy;
			auto p_src_byte = p_src_bmp_byte + src_w * SDst(iy) + SDst(ix);

			if(unsigned long col = bilinear_interpolation(p_src_byte[0],
				p_src_byte[1], p_src_byte[src_w], p_src_byte[src_w + 1],
				fx, fy))
				p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = col;
		}
	}
}

void
draw_flat_scanline_alpha_s(IMAGE* dc_dest, const vector2d* vt,
	const IMAGE* dc_src, const vector2d* svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = float2int(vt->p[0].x), e = float2int(vt->p[1].x),
		y = float2int(vt->p[0].y), w = e - b;
	unsigned long* p_dest_bmp_byte = dc_dest->getbuffer();
	unsigned long* p_src_bmp_byte = dc_src->getbuffer();
	unsigned long sa = unsigned(alpha), da = 0xFF - sa;
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x, cury = _svt.p[0].y,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;

		for(i = b; i < e; ++i, curx += dx, cury += dy)
		{
			int ix(curx), iy(cury);
			float fx(curx - ix), fy = cury - iy;
			auto p_src_byte = p_src_bmp_byte + src_w * SDst(iy) + SDst(ix);
			unsigned long col(bilinear_interpolation(p_src_byte[0],
				p_src_byte[1], p_src_byte[src_w], p_src_byte[src_w + 1],
				fx, fy));
			unsigned long d = p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)];

			d = (((d & 0xFF00FF) * da & 0xFF00FF00)
				| ((d & 0xFF00) * da & 0xFF0000)) >> 8;
			col = (((col & 0xFF00FF) * sa & 0xFF00FF00)
				| ((col & 0xFF00) * sa & 0xFF0000)) >> 8;
			p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = d + col;
		}
	}
}

void
draw_flat_scanline_alphatrans_s(IMAGE* dc_dest, const vector2d* vt,
	const IMAGE* dc_src, const vector2d* svt, int x1, int x2, int alpha)
{
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int b = float2int(vt->p[0].x), e = float2int(vt->p[1].x),
		y = float2int(vt->p[0].y), w = e - b;
	unsigned long* p_dest_bmp_byte = dc_dest->getbuffer();
	unsigned long* p_src_bmp_byte = dc_src->getbuffer();
	unsigned long sa = unsigned(alpha), da = 0xFF - sa;
	auto dest_w = dc_dest->GetWidth(), src_w = dc_src->GetWidth();
	//int src_h = dc_src->h;

	if(w > 0)
	{
		int i, bx = b;
		vector2d _svt = *svt;

		_svt.p[0].x += (b - vt->p[0].x) * rw / dw;
		_svt.p[1].x += (e - vt->p[1].x) * rw / dw;

		float curx = _svt.p[0].x, cury = _svt.p[0].y,
			dx = (_svt.p[1].x - _svt.p[0].x) / w,
			dy = (_svt.p[1].y - _svt.p[0].y) / w;

		if(b < x1)
			b = x1;
		if(e > x2)
			e = x2;
		curx += (b - bx) * dx;
		cury += (b - bx) * dy;
		for(i = b; i < e; ++i, curx += dx, cury += dy)
		{
			int ix(curx), iy(cury);
			float fx(curx - ix), fy = cury - iy;
			auto p_src_byte = p_src_bmp_byte + src_w * SDst(iy) + SDst(ix);

			if(unsigned long col = bilinear_interpolation(
				p_src_byte[0], p_src_byte[1], p_src_byte[src_w],
				p_src_byte[src_w + 1], fx, fy))
			{
				unsigned long d = p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)];
				d = (((d & 0xFF00FF) * da & 0xFF00FF00)
					| ((d & 0xFF00) * da & 0xFF0000)) >> 8;
				col = (((col & 0xFF00FF) * sa & 0xFF00FF00)
					| ((col & 0xFF00) * sa & 0xFF0000)) >> 8;
				p_dest_bmp_byte[dest_w * SDst(y) + SDst(i)] = d + col;
			}
		}
	}
}

void
draw_flat_trangle_alpha(IMAGE* dc_dest, const triangle2d * dt,
	const IMAGE* dc_src, const triangle2d * tt, int x1, int y1, int x2, int y2,
	int transparent, int alpha)
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

	float dd;
	int b = float2int(t2d.p[0].y), e = float2int(t2d.p[2].y), h,
		m = float2int(t2d.p[1].y), rs, re, i, lh, rh;
	//	float dm = t2d.p[1].y;
	point2d pl, pr, spl, spr;

	pl.x = t2d.p[1].x - t2d.p[0].x;
	pr.x = t2d.p[2].x - t2d.p[0].x;
	pl.y = t2d.p[1].y - t2d.p[0].y;
	pr.y = t2d.p[2].y - t2d.p[0].y;
	spl.x = t3d.p[1].x - t3d.p[0].x;
	spr.x = t3d.p[2].x - t3d.p[0].x;
	spl.y = t3d.p[1].y - t3d.p[0].y;
	spr.y = t3d.p[2].y - t3d.p[0].y;
	//	float dh = dm - b;
	h = m - b;
	rs = b;
	if(b < y1)
		b = y1;
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
	lh = float2int((pl.y + t2d.p[0].y))
			- float2int((t2d.p[0].y));
	rh = float2int((pr.y + t2d.p[0].y))
			- float2int((t2d.p[0].y));
	if(h > 0)
		for(i = b; i < m; ++i)
		{
			vector2d vt;
			vector2d svt;
			//float dt = (i - rs) / h;
			float dlt = (i - rs) / lh;
			float drt = (i - rs) / rh;

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
			vt.p[0].y = vt.p[1].y = (i);
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
		vt.p[0].y = vt.p[1].y = (m);
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
	lh = float2int((t2d.p[2].y))
			- float2int((pl.y + t2d.p[2].y));
	rh = float2int((t2d.p[2].y))
			- float2int((pr.y + t2d.p[2].y));
	if(h > 0)
		for(i = e; i > m; --i)
		{
			vector2d vt;
			vector2d svt;
			//float dt = (re - i) / h;
			float dlt = (re - i) / lh;
			float drt = (re - i) / rh;
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
			vt.p[0].y = vt.p[1].y = (i);
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

void
draw_flat_trangle_alpha_s(IMAGE* dc_dest, const triangle2d * dt,
	const IMAGE* dc_src, const triangle2d * tt, int x1, int y1, int x2, int y2,
	int transparent, int alpha)
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

	float dd;
	int b = float2int(t2d.p[0].y), e = float2int(t2d.p[2].y),
		h, m = float2int(t2d.p[1].y), rs, re, i, lh, rh;
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
	//	dh = dm - b;
	h = m - b;
	rs = b;
	if(b < y1)
		b = y1;
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
	lh = float2int((pl.y + t2d.p[0].y))
			- float2int((t2d.p[0].y));
	rh = float2int((pr.y + t2d.p[0].y))
			- float2int((t2d.p[0].y));
	if(h > 0)
	{
		for(i = b; i < m; ++i)
		{
			vector2d vt;
			vector2d svt;
			//float dt = (i - rs) / h;
			float dlt = (i - rs) / lh;
			float drt = (i - rs) / rh;

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
			vt.p[0].y = vt.p[1].y = (i);

			svt.p[0].x = t3d.p[0].x + spl.x * dlt;
			svt.p[1].x = t3d.p[0].x + spr.x * drt;
			svt.p[0].y = t3d.p[0].y + spl.y * dlt;
			svt.p[1].y = t3d.p[0].y + spr.y * drt;

			if(b_alpha)
			{
				if(transparent)
					draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src,
						&svt, x1, x2, alpha);
				else
					draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt,
						x1, x2, alpha);
			}
			else
			{
				if(transparent)
					draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src,
						&svt, x1, x2);
				else
					draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1,
						x2);
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
		vt.p[0].y = vt.p[1].y = (m);

		svt.p[0].x = t3d.p[0].x + spl.x;
		svt.p[1].x = t3d.p[0].x + spr.x;
		svt.p[0].y = t3d.p[0].y + spl.y;
		svt.p[1].y = t3d.p[0].y + spr.y;

		if(b_alpha)
		{
			if(transparent)
				draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt,
					x1, x2, alpha);
			else
				draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1,
					x2, alpha);
		}
		else
		{
			if(transparent)
				draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt,
					x1, x2);
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
			//float dt = (re - i) / h;
			float dlt = (re - i) / lh;
			float drt = (re - i) / rh;

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
			vt.p[0].y = vt.p[1].y = (i);

			svt.p[0].x = t3d.p[2].x + spl.x * dlt;
			svt.p[1].x = t3d.p[2].x + spr.x * drt;
			svt.p[0].y = t3d.p[2].y + spl.y * dlt;
			svt.p[1].y = t3d.p[2].y + spr.y * drt;


			if(b_alpha)
			{
				if(transparent)
					draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src,
						&svt, x1, x2, alpha);
				else
					draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt,
						x1, x2, alpha);
			}
			else
			{
				if(transparent)
					draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src,
						&svt, x1, x2);
				else
					draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1,
						x2);
			}
		}
	}
}

int
putimage_trangle(IMAGE* imgdest, const IMAGE* imgtexture,
	const triangle2d * dt,// dest trangle, original
	const triangle2d * tt,// textture trangle uv 0.0 - 1.0
	int btransparent, int alpha, int smooth)
{
	auto dc_dest = imgdest;
	auto dc_src = imgtexture;

	if(dc_dest)
	{
		triangle2d _dt = *dt;
		triangle2d _tt = *tt;
		int x1 = 0, y1 = 0, x2 = int(dc_dest->GetWidth()),
			y2 = int(dc_dest->GetHeight());

		if(smooth)
			for(size_t i = 0; i < 3; ++i)
			{
				_tt.p[i].x = float2int((_tt.p[i].x
					* (dc_src->GetWidth() - 2)));
				_tt.p[i].y = float2int((_tt.p[i].y
					* (dc_src->GetHeight() - 2)));
			}
		else
			for(size_t i = 0; i < 3; ++i)
			{
				_tt.p[i].x = float2int((_tt.p[i].x
					* (dc_src->GetWidth() - 1)));
				_tt.p[i].y = float2int((_tt.p[i].y
					* (dc_src->GetHeight() - 1)));
			}
		if(smooth)
		{
			if(dc_src->GetWidth() > 1 && dc_src->GetHeight() > 1)
				draw_flat_trangle_alpha_s(dc_dest, &_dt, dc_src, &_tt, x1, y1,
					x2, y2, btransparent, alpha);
		}
		else
			draw_flat_trangle_alpha(dc_dest, &_dt, dc_src, &_tt, x1, y1, x2, y2,
				btransparent, alpha);
	}
	return grOk;
}

} // unnamed namespace;

int
putimage_rotate(IMAGE* imgdest, const IMAGE* imgtexture, int nXOriginDest,
	int nYOriginDest, float centerx, float centery, float radian,
	int btransparent, // transparent (1) or not (0)
	int alpha, // in range[0, 256], alpha==256 means no alpha
	int smooth)
{
	auto& dc_dest(cimg_ref(imgdest));
	auto dc_src(imgtexture);

	triangle2d _tt[2], _dt[2];
	double dx, dy, cr = std::cos(radian), sr = -std::sin(radian);
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
				* (dc_src->GetWidth());
			_dt[j].p[i].y = (_dt[j].p[i].y - centery)
				* (dc_src->GetHeight());
			dx = cr * double(_dt[j].p[i].x) - sr * double(_dt[j].p[i].y);
			dy = sr * double(_dt[j].p[i].x) + cr * double(_dt[j].p[i].y);
			_dt[j].p[i].x
				= float(float2int(float(dx + nXOriginDest) + FLT_EPSILON));
			_dt[j].p[i].y
				= float(float2int(float(dy + nYOriginDest) + FLT_EPSILON));
		}
	putimage_trangle(&dc_dest, imgtexture, &_dt[0], &_tt[0], btransparent,
		alpha, smooth);
	putimage_trangle(&dc_dest, imgtexture, &_dt[1], &_tt[1], btransparent,
		alpha, smooth);
	return grOk;
}

int
putimage_rotatezoom(IMAGE* imgdest, const IMAGE* imgtexture, int nXOriginDest,
	int nYOriginDest, float centerx, float centery, float radian,
	float zoom, int btransparent, // transparent (1) or not (0)
	int alpha, // in range[0, 256], alpha==256 means no alpha
	int smooth)
{
	auto& dc_dest(cimg_ref(imgdest));
	auto dc_src = imgtexture;

	triangle2d _tt[2], _dt[2];
	double dx, dy, cr = std::cos(radian), sr = -std::sin(radian);
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
			_dt[j].p[i].x = double(_dt[j].p[i].x - centerx)
				* (dc_src->GetWidth());
			_dt[j].p[i].y = double(_dt[j].p[i].y - centery)
				* (dc_src->GetHeight());
			dx = cr * double(_dt[j].p[i].x) - sr * double(_dt[j].p[i].y);
			dy = sr * double(_dt[j].p[i].x) + cr * double(_dt[j].p[i].y);
			_dt[j].p[i].x = float(float2int(float(dx) * zoom + nXOriginDest
				+ FLT_EPSILON));
			_dt[j].p[i].y = float(float2int(float(dy) * zoom + nYOriginDest
				+ FLT_EPSILON));
		}
	putimage_trangle(&dc_dest, imgtexture, &_dt[0], &_tt[0], btransparent,
		alpha, smooth);
	putimage_trangle(&dc_dest, imgtexture, &_dt[1], &_tt[1], btransparent,
		alpha, smooth);
	return grOk;
}


IMAGE&
cimg_ref(IMAGE* pimg)
{
	return pimg ? *pimg : (--update_mark_count, get_pages().get_target_ref());
}
const IMAGE&
cimg_ref(const IMAGE* pimg)
{
	return pimg ? *pimg : (--update_mark_count, get_pages().get_target_ref());
}

IMAGE&
cimg_ref_c(IMAGE* pimg)
{
	return pimg ? *pimg : get_pages().get_target_ref();
}
const IMAGE&
cimg_ref_c(const IMAGE* pimg)
{
	return pimg ? *pimg : get_pages().get_target_ref();
}

} // namespace ege;
