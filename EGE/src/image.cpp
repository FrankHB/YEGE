/*
* EGE (Easy Graphics Engine)
* filename  image.cpp

���ļ��������ж�image���������Ľӿں��ඨ��
*/

#include "ege/head.h"
#include "libpng/png.h"
#include "libpng/pngstruct.h"
#include "libpng/pnginfo.h"
#include "ocidl.h"
#include "olectl.h"

#include <math.h>

namespace ege {

static HBITMAP  g_hbmp_def;
static HBRUSH   g_hbr_def;
static HPEN     g_pen_def;
static HFONT    g_font_def;

IMAGE::IMAGE() {
	m_initflag = IMAGE_INIT_FLAG;
	m_hDC = NULL;
	m_pattern_obj = NULL;
	m_texture = NULL;
	PIMAGE img = CONVERT_IMAGE_CONST(0);
	if (img) {
		newimage(img->m_hDC, 1, 1);
	} else {
		newimage(graph_setting.dc, 1, 1);
	}
	CONVERT_IMAGE_END;
}

IMAGE::IMAGE(int width, int height) {
	m_initflag = IMAGE_INIT_FLAG;
	m_hDC = NULL;
	m_pattern_obj = NULL;
	m_texture = NULL;
	PIMAGE img = CONVERT_IMAGE_CONST(0);
	if (img) {
		newimage(img->m_hDC, width, height);
	} else {
		newimage(graph_setting.dc, width, height);
	}
	CONVERT_IMAGE_END;
}

IMAGE::IMAGE(IMAGE &img) {
	m_initflag = IMAGE_INIT_FLAG;
	m_hDC = NULL;
	m_pattern_obj = NULL;
	m_texture = NULL;
	newimage(img.m_hDC, img.m_width, img.m_height);
	BitBlt(m_hDC, 0, 0, img.m_width, img.m_height, img.m_hDC, 0, 0, SRCCOPY);
}

IMAGE::~IMAGE() {
	gentexture(false);
	delete_pattern();
	deleteimage();
}

void IMAGE::inittest(const WCHAR* strCallFunction) const {
	if (m_initflag != IMAGE_INIT_FLAG) {
		WCHAR str[60];
		wsprintfW(str, L"Fatal error: read/write at 0x%p. At function '%s'", this, strCallFunction);
		MessageBoxW(graph_setting.hwnd, str, L"EGE ERROR message", MB_ICONSTOP);
		ExitProcess((UINT)grError);
	}
}

void
IMAGE::set_pattern(void* obj, int type) {
	delete_pattern();
	m_pattern_type = type;
	m_pattern_obj = obj;
}

void
IMAGE::delete_pattern() {
	if (m_pattern_obj == NULL) return;

	if (m_pattern_type == pattern_none) {
	} else if (m_pattern_type == pattern_lineargradient) {
		delete (Gdiplus::LinearGradientBrush*)m_pattern_obj;
	} else if (m_pattern_type == pattern_pathgradient) {
		delete (Gdiplus::PathGradientBrush*)m_pattern_obj;
	} else if (m_pattern_type == pattern_texture) {
		delete (Gdiplus::TextureBrush*)m_pattern_obj;
	}
	m_pattern_obj = NULL;
}

void
IMAGE::gentexture(bool gen) {
	if (!gen) {
		if (m_texture != NULL) {
			delete (Gdiplus::Bitmap*)m_texture;
			m_texture = NULL;
		}
	} else {
		if (m_texture != NULL) {
			gentexture(true);
		}
		Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(getwidth(), getheight(), getwidth() * 4, PixelFormat32bppARGB, (BYTE*)getbuffer());
		m_texture = bitmap;
	}
}

int
IMAGE::deleteimage() {
	DeleteObject(SelectObject(m_hDC, g_hbmp_def));
	DeleteObject(SelectObject(m_hDC, g_hbr_def));
	DeleteObject(SelectObject(m_hDC, g_pen_def));
	DeleteObject(SelectObject(m_hDC, g_font_def));
	DeleteDC(m_hDC);
	m_hDC = NULL;
	return 0;
}

int
IMAGE::newimage(HDC hdc, int width, int height) {
	HDC dc;
	HBITMAP bitmap;
	auto bmi = ::BITMAPINFO();
	VOID* p_bmp_buf;

	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height - 1;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biSizeImage = width * height * 4;

	memset(&m_vpt, 0, sizeof(m_vpt));

	if (hdc == NULL) {
		hdc = m_hDC;
		if (hdc == NULL) {
			WCHAR str[60];
			wsprintfW(str, L"Fatal error: read/write at 0x%08x. At function 'newimage', construct PIMAGE before 'initgraph'?", this);
			MessageBoxW(graph_setting.hwnd, str, L"ERROR message", MB_ICONSTOP);
			ExitProcess((UINT)grError);
		}
	}
	if (m_hDC) {
		dc = m_hDC;
	} else {
		dc = CreateCompatibleDC(hdc);
	}
	if (dc != NULL) {
		bitmap = CreateDIBSection(
			NULL,
			&bmi,
			DIB_RGB_COLORS,
			(VOID**)&p_bmp_buf,
			NULL,
			0
			);

		if (bitmap != NULL) {
			HBITMAP hbmp_def = (HBITMAP)SelectObject(dc, bitmap);
			int b_resize = 0;
			if (g_hbmp_def == NULL) {
				g_hbmp_def = hbmp_def;
				g_hbr_def  = (HBRUSH)GetCurrentObject(dc, OBJ_BRUSH);
				g_pen_def  = (HPEN)GetCurrentObject(dc, OBJ_PEN);
				g_font_def = (HFONT)GetCurrentObject(dc, OBJ_FONT);
			}
			if (m_hDC) {
				DeleteObject(hbmp_def);
				b_resize = 1;
			} else {
				m_vpt.left   = 0;
				m_vpt.top    = 0;
				m_vpt.right  = width;
				m_vpt.bottom = height;
				m_vpt.clipflag   = 1;
			}
			m_hDC = dc;
			m_hBmp = bitmap;
			m_width = width;
			m_height = height;
			m_pBuffer = (PDWORD)p_bmp_buf;

			if (b_resize == 0) {
				setcolor(LIGHTGRAY, this);
				setbkcolor(BLACK, this);
				SetBkMode(dc, OPAQUE); //TRANSPARENT);
				setfillstyle(SOLID_FILL, 0, this);
				setlinestyle(PS_SOLID, 0, 1, this);
				settextjustify(LEFT_TEXT, TOP_TEXT, this);
				ege_enable_aa(false, this);
			} else {
				//SetBkMode(dc, bkMode);
			}
			setviewport(0, 0, m_width, m_height, 1, this);
			cleardevice(this);
		} else {
			int err = GetLastError();
			DeleteDC(dc);
			return err;
		}
	} else {
		return GetLastError();
	}
	return 0;
}

int
IMAGE::createimage(int width, int height) {
	inittest(L"IMAGE::createimage");
	PIMAGE img = CONVERT_IMAGE_CONST(0);
	if (img == NULL) {
		img = graph_setting.img_page[graph_setting.active_page];
	}
	int ret = newimage(img->m_hDC, width, height);
	CONVERT_IMAGE_END;
	if (ret) {
		return ret;
	}
	return 0;
}

int
IMAGE::resize(int width, int height) {
	inittest(L"IMAGE::createimage");
	PIMAGE img = CONVERT_IMAGE_CONST(0);
	int ret = newimage(img->m_hDC, width, height);
	CONVERT_IMAGE_END;
	if (ret) {
		return ret;
	}
	return 0;
}

IMAGE&
IMAGE::operator = (const IMAGE &img) {
	inittest(L"IMAGE::operator=");
	getimage((PIMAGE)(&img), 0, 0, img.m_width, img.m_height);
	return *this;
}

void
IMAGE::copyimage(const PIMAGE pSrcImg) {
	inittest(L"IMAGE::copyimage");
	const PIMAGE img = CONVERT_IMAGE_CONST(pSrcImg);
	int ret = 0;
	if (m_width != img->m_width || m_height != img->m_height)
		ret = newimage(0, img->m_width, img->m_height);
	if (ret == 0) {
		memcpy(getbuffer(), img->getbuffer(), m_width * m_height * 4); // 4 byte per pixel
	}
	CONVERT_IMAGE_END;
}

void
IMAGE::getimage(const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight) {
	inittest(L"IMAGE::getimage");
	const PIMAGE img = CONVERT_IMAGE_CONST(pSrcImg);
	int ret = newimage(0, srcWidth, srcHeight);
	if (ret == 0) {
		BitBlt(m_hDC, 0, 0, srcWidth, srcHeight, img->m_hDC, srcX, srcY, SRCCOPY);
	}
	CONVERT_IMAGE_END;
}

void
IMAGE::getimage(int srcX, int srcY, int srcWidth, int srcHeight) {
	PIMAGE img = CONVERT_IMAGE_CONST(0);
	getimage(img, srcX, srcY, srcWidth, srcHeight);
	CONVERT_IMAGE_END;
}

void
IMAGE::putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, DWORD dwRop) const {
	inittest(L"IMAGE::putimage");
	PIMAGE img = CONVERT_IMAGE(pDstImg);
	BitBlt(img->m_hDC, dstX, dstY, dstWidth, dstHeight, m_hDC, srcX, srcY, dwRop);
	CONVERT_IMAGE_END;
}

void
IMAGE::putimage(PIMAGE pDstImg, int dstX, int dstY, DWORD dwRop) const {
	putimage(pDstImg, dstX, dstY, m_width, m_height, 0, 0, dwRop);
}

void
IMAGE::putimage(int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, DWORD dwRop) const {
	PIMAGE img = CONVERT_IMAGE(0);
	putimage(img, dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
	CONVERT_IMAGE_END;
}

void
IMAGE::putimage(int dstX, int dstY, DWORD dwRop) const {
	PIMAGE img = CONVERT_IMAGE(0);
	putimage(img, dstX, dstY, dwRop);
	CONVERT_IMAGE_END;
}

int
IMAGE::getimage(LPCSTR filename, int zoomWidth, int zoomHeight) {
	inittest(L"IMAGE::getimage");
	{
		int ret = getimage_pngfile(this, filename);
		if (ret == 0) return 0;
	}

	WCHAR           wszPath[MAX_PATH*2+1];
	MultiByteToWideChar(CP_ACP, 0, filename, -1, wszPath, MAX_PATH*2);
	return getimage(wszPath, zoomWidth, zoomHeight);
}

int
IMAGE::getimage(LPCWSTR filename, int, int) {
	inittest(L"IMAGE::getimage");
	{
		int ret = getimage_pngfile(this, filename);
		if (ret == 0) return 0;
	}

	struct IPicture *pPicture;
	OLECHAR         wszPath[MAX_PATH*2+1];
	WCHAR           szPath[MAX_PATH*2+1] = L"";
	long            lWidth,         lHeight;
	long            lWidthPixels,   lHeightPixels;
	HRESULT         hr;

	if (wcsstr(filename, L"http://")) {
		lstrcpyW(szPath, filename);
	} else if (filename[1] == ':') {
		lstrcpyW(szPath, filename);
	} else {
		GetCurrentDirectoryW(MAX_PATH*2, szPath);
		lstrcatW(szPath, L"\\");
		lstrcatW(szPath, filename);
	}

	lstrcpyW(wszPath, szPath);
	hr = ::OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture,
		(void**)&pPicture);
	if(FAILED(hr))
		return grIOerror;

	PIMAGE img = CONVERT_IMAGE_CONST(0);

	pPicture->get_Width (&lWidth );
	lWidthPixels  = MulDiv(lWidth,  GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
	CONVERT_IMAGE_END;

	createimage(lWidthPixels, lHeightPixels);
	{
		::HDC dc = m_hDC;

		pPicture->Render(dc, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight,
			lWidth, -lHeight, 0);
	}
	pPicture->Release();
	return grOk;
}

// private function
static int
saveimagetofile(PIMAGE img, FILE* fp) {
	auto bmpfHead = ::BITMAPFILEHEADER();
	auto bmpinfo = ::BITMAPINFOHEADER();
	int pitch = img->m_width * 3, addbit, y, x, zero = 0;

	addbit = 4 - (pitch & 3);
	if(pitch & 3)
		pitch = ((pitch + 4) & ~3); else addbit = 0;
	bmpfHead.bfType = *(WORD*)"BM";
	bmpfHead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpfHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
		+ pitch * img->m_height;
	bmpinfo.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.biBitCount = 24;
	bmpinfo.biHeight = img->m_height;
	bmpinfo.biWidth = img->m_width;
	bmpinfo.biPlanes = 1;
	bmpinfo.biSizeImage = pitch * img->m_height;
	//bmpinfo.biXPelsPerMeter
	fwrite(&bmpfHead, sizeof(bmpfHead), 1, fp);
	fwrite(&bmpinfo, sizeof(bmpinfo), 1, fp);

	for (y = img->m_height - 1; y >= 0; --y) {
		for (x = 0; x < img->m_width; ++x) {
			DWORD col = img->m_pBuffer[y * img->m_width + x];
			//col = RGBTOBGR(col);
			size_t ret = fwrite(&col, 3, 1, fp);
			if (ret < 1) goto ERROR_BREAK;
		}
		if (addbit > 0)
			fwrite(&zero, addbit, 1, fp);
	}
	return 0;
ERROR_BREAK:
	return grIOerror;
}

int
IMAGE::saveimage(LPCSTR  filename) {
	FILE *fp = NULL;
	int ret;
	fp = fopen(filename, "wb");
	if (fp == NULL) return grIOerror;
	ret = saveimagetofile(this, fp);
	fclose(fp);
	return ret;
}

int
IMAGE::saveimage(LPCWSTR filename) {
	FILE *fp = NULL;
	int ret;
	fp = _wfopen(filename, L"wb");
	if (fp == NULL) return grIOerror;
	ret = saveimagetofile(this, fp);
	fclose(fp);
	return ret;
}

int
IMAGE::getpngimg(FILE* fp) {
	png_structp png_ptr;
	png_infop info_ptr;
	uint32 width, height, depth;

	{
		char header[16];
		uint32 number = 8;
		fread(header, 1, number, fp);
		int isn_png = png_sig_cmp((png_const_bytep)header, 0, number);

		if (isn_png) {
			return grIOerror;
		}
		fseek(fp, 0, SEEK_SET);
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		return -1;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		return -1;
	}
	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR|PNG_TRANSFORM_EXPAND, NULL);
	png_set_expand(png_ptr);

	newimage(NULL, (int)(info_ptr->width), (int)(info_ptr->height)); //png_get_IHDR
	width = info_ptr->width;
	height = info_ptr->height;
	depth = info_ptr->pixel_depth;

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	for (uint32 i = 0; i < height; ++i) {
		if (depth == 24) {
			for (uint32 j = 0; j < width; ++j) {
				m_pBuffer[i * width + j] = 0xFFFFFF & (DWORD&)row_pointers[i][j * 3];
			}
		} else if (depth == 32) {
			for (uint32 j = 0; j < width; ++j) {
				m_pBuffer[i * width + j] = ((DWORD*)(row_pointers[i]))[j];
				if ( (m_pBuffer[i * width + j] & 0xFF000000) == 0) {
					m_pBuffer[i * width + j] = 0;
				}
			}
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return 0;
}

int
IMAGE::savepngimg(FILE* fp, int bAlpha) {
	unsigned long i, j;
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp palette;
	png_byte *image;
	png_bytep *row_pointers;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	uint32 pixelsize = bAlpha ? 4 : 3;
	uint32 width = m_width, height = m_height;
	if (png_ptr == NULL) {
		return -1;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_write_struct(&png_ptr, NULL);
		return -1;
	}

	/*if (setjmp(png_jmpbuf(png_ptr)))
	{
	png_destroy_write_struct(&png_ptr, &info_ptr);
	return -1;
	}// */

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, bAlpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof (png_color));
	png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

	png_write_info(png_ptr, info_ptr);

	png_set_packing(png_ptr);

	image = (png_byte *)malloc(width * height * pixelsize * sizeof(png_byte) + 4);
	if(image == NULL) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return -1;
	}

	row_pointers = (png_bytep *)malloc(height * sizeof(png_bytep));
	if(row_pointers == NULL) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		free(image);
		image = NULL;
		return -1;
	}

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; ++j) {
			(DWORD&)image[(i * width  + j) * pixelsize] = RGBTOBGR(m_pBuffer[i * width + j]);
		}
		row_pointers[i] = (png_bytep)image + i * width * pixelsize;
	}

	png_write_image(png_ptr, row_pointers);

	png_write_end(png_ptr, info_ptr);

	png_free(png_ptr, palette);
	palette = NULL;

	png_destroy_write_struct(&png_ptr, &info_ptr);

	free(row_pointers);
	row_pointers = NULL;

	free(image);
	image = NULL;

	return 0;
}

int
IMAGE::getimage(LPCSTR pResType, LPCSTR pResName, int, int) {
	inittest(L"IMAGE::getimage");
	struct _graph_setting * pg = &graph_setting;
	HRSRC hrsrc = FindResourceA(pg->instance, pResName, pResType);

	if (hrsrc) {
		HGLOBAL         hg = LoadResource(0, hrsrc);
		DWORD           dwSize = SizeofResource(0, hrsrc);
		HGLOBAL         hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		LPVOID          pvRes = LockResource(hg);
		LPVOID          pvData;
		struct IPicture *pPicture;
		IStream         *pStm;
		long            lWidth,         lHeight;
		long            lWidthPixels,   lHeightPixels;
		HRESULT         hr;

		if (hGlobal == NULL || (pvData = GlobalLock(hGlobal)) == NULL) {
			return grAllocError;
		}
		memcpy(pvData, pvRes, dwSize);
		GlobalUnlock(hGlobal);
		if (S_OK != CreateStreamOnHGlobal(hGlobal, TRUE, &pStm)) {
			return grNullPointer;
		}

		hr = OleLoadPicture(
			pStm,
			(LONG)dwSize,
			TRUE,
			IID_IPicture,
			(void**)&pPicture
			);

		GlobalFree(hGlobal);

		if(FAILED(hr)) {
			return grIOerror;
		}


		PIMAGE img = CONVERT_IMAGE_CONST(0);
		pPicture->get_Width(&lWidth);
		lWidthPixels = MulDiv(lWidth, GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
		pPicture->get_Height(&lHeight);
		lHeightPixels = MulDiv(lHeight, GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
		CONVERT_IMAGE_END;

		createimage(lWidthPixels, lHeightPixels);
		{
			HDC dc = m_hDC;

			pPicture->Render(
				dc,
				0, 0,
				lWidthPixels, lHeightPixels,
				0, lHeight,
				lWidth, -lHeight,
				0
				);
		}

		pPicture->Release();

		return grOk;
	}

	return grIOerror;
}


int
IMAGE::getimage(LPCWSTR pResType, LPCWSTR pResName, int, int)
{
	inittest(L"IMAGE::getimage");
	struct _graph_setting * pg = &graph_setting;
	HRSRC hrsrc = FindResourceW(pg->instance, pResName, pResType);


	if(hrsrc)
	{
		auto hg = ::LoadResource(0, hrsrc);
		auto dwSize = ::SizeofResource(0, hrsrc);
		auto hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, dwSize);
		auto pvRes = ::LockResource(hg);
		LPVOID pvData;
		IPicture *pPicture;
		IStream *pStm;
		long lWidth, lHeight;
		long lWidthPixels, lHeightPixels;

		if (hGlobal == NULL || (pvData = GlobalLock(hGlobal)) == NULL)
			return grAllocError;
		::memcpy(pvData, pvRes, dwSize);
		::GlobalUnlock(hGlobal);
		if (S_OK != CreateStreamOnHGlobal(hGlobal, TRUE, &pStm))
			return grNullPointer;

		auto hr = OleLoadPicture(pStm, (LONG)dwSize, TRUE, IID_IPicture,
			(void**)&pPicture);

		GlobalFree(hGlobal);

		if(FAILED(hr)) {
			return grIOerror;
		}


		PIMAGE img = CONVERT_IMAGE_CONST(0);
		pPicture->get_Width(&lWidth);
		lWidthPixels = MulDiv(lWidth, GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
		pPicture->get_Height(&lHeight);
		lHeightPixels = MulDiv(lHeight, GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
		CONVERT_IMAGE_END;

		createimage(lWidthPixels, lHeightPixels);
		{
			HDC dc = m_hDC;

			pPicture->Render(
				dc,
				0, 0,
				lWidthPixels, lHeightPixels,
				0, lHeight,
				lWidth, -lHeight,
				0
				);
		}

		pPicture->Release();

		return grOk;
	}

	return grIOerror;
}
int
IMAGE::getimage(void* pMem, long size) {
	inittest(L"IMAGE::getimage");

	if (pMem) {
		DWORD           dwSize = size;
		HGLOBAL         hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwSize);
		LPVOID          pvData;
		struct IPicture *pPicture;
		IStream         *pStm;
		long            lWidth,         lHeight;
		long            lWidthPixels,   lHeightPixels;
		HRESULT         hr;

		if (hGlobal == NULL || (pvData = GlobalLock(hGlobal)) == NULL) {
			return grAllocError;
		}
		memcpy(pvData, pMem, dwSize);
		GlobalUnlock(hGlobal);
		if (S_OK != CreateStreamOnHGlobal(hGlobal, TRUE, &pStm)) {
			return grNullPointer;
		}

		hr = OleLoadPicture(
			pStm,
			(LONG)dwSize,
			TRUE,
			IID_IPicture,
			(void**)&pPicture
			);

		GlobalFree(hGlobal);

		if(FAILED(hr)) {
			return grIOerror;
		}


		PIMAGE img = CONVERT_IMAGE_CONST(0);
		pPicture->get_Width(&lWidth);
		lWidthPixels = MulDiv(lWidth, GetDeviceCaps(img->m_hDC, LOGPIXELSX), 2540);
		pPicture->get_Height(&lHeight);
		lHeightPixels = MulDiv(lHeight, GetDeviceCaps(img->m_hDC, LOGPIXELSY), 2540);
		CONVERT_IMAGE_END;

		createimage(lWidthPixels, lHeightPixels);
		{
			HDC dc = m_hDC;

			pPicture->Render(
				dc,
				0, 0,
				lWidthPixels, lHeightPixels,
				0, lHeight,
				lWidth, -lHeight,
				0
				);
		}

		pPicture->Release();

		return grOk;
	}

	return grIOerror;
}

void
IMAGE::putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop) const {
	inittest(L"IMAGE::putimage");
	const PIMAGE img = CONVERT_IMAGE(pDstImg);
	if (img) {
		SetStretchBltMode(img->m_hDC, COLORONCOLOR);
		StretchBlt(img->m_hDC, dstX, dstY, dstWidth, dstHeight, m_hDC, srcX, srcY, srcWidth, srcHeight, dwRop);
	}
	CONVERT_IMAGE_END;
}

/* private function */
static
void
fix_rect_1size(
	PIMAGE pdest,
	PIMAGE psrc,
	int* nXOriginDest,   // x-coord of destination upper-left corner
	int* nYOriginDest,   // y-coord of destination upper-left corner
	int* nXOriginSrc,    // x-coord of source upper-left corner
	int* nYOriginSrc,    // y-coord of source upper-left corner
	int* nWidthSrc,      // width of source rectangle
	int* nHeightSrc      // height of source rectangle
	)
{
	viewporttype _vpt = {0, 0, pdest->m_width, pdest->m_height, 0};
	/* default value proc */
	if(*nWidthSrc == 0)
	{
		*nWidthSrc  = psrc->m_width;
		*nHeightSrc = psrc->m_height;
	}
	/* fix src rect */
	if(*nWidthSrc > psrc->m_width)
	{
		*nWidthSrc -= *nWidthSrc - psrc->m_width;
		*nWidthSrc = psrc->m_width;
	}
	if(*nHeightSrc > psrc->m_height)
	{
		*nHeightSrc -= *nHeightSrc - psrc->m_height;
		*nHeightSrc = psrc->m_height;
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

int
IMAGE::putimage_transparent(
	PIMAGE imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
) {
	inittest(L"IMAGE::putimage_transparent");
	const PIMAGE img = CONVERT_IMAGE(imgdest);
	if (img) {
		PIMAGE imgsrc = this;
		int y, x;
		DWORD ddx, dsx;
		DWORD *pdp, *psp, cr;
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
		for (y=0; y<nHeightSrc; ++y) {
			for (x=0; x<nWidthSrc; ++x, ++psp, ++pdp) {
				if (*psp != cr) {
					*pdp = *psp;
				}
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	CONVERT_IMAGE_END;
	return grOk;
}

int
IMAGE::putimage_alphablend(
	PIMAGE imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
) {
	inittest(L"IMAGE::putimage_alphablend");
	const PIMAGE img = CONVERT_IMAGE(imgdest);
	if (img) {
		PIMAGE imgsrc = this;
		int y, x;
		DWORD ddx, dsx;
		DWORD *pdp, *psp;
		DWORD sa = alpha + 1, da = 0xFF - alpha;
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
		for (y=0; y<nHeightSrc; ++y) {
			for (x=0; x<nWidthSrc; ++x, ++psp, ++pdp) {
				DWORD d=*pdp, s=*psp;
				d = ((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da >> 16 << 16);
				s = ((s&0xFF00FF)*sa & 0xFF00FF00) | ((s&0xFF00)*sa >> 16 << 16);
				*pdp = (d + s) >> 8;
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	CONVERT_IMAGE_END;
	return grOk;
}

int
IMAGE::putimage_alphatransparent(
	PIMAGE imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
) {
	inittest(L"IMAGE::putimage_alphablend");
	const PIMAGE img = CONVERT_IMAGE(imgdest);
	if (img) {
		PIMAGE imgsrc = this;
		int y, x;
		DWORD ddx, dsx;
		DWORD *pdp, *psp, cr;
		DWORD sa = alpha + 1, da = 0xFF - alpha;
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
		for (y=0; y<nHeightSrc; ++y) {
			for (x=0; x<nWidthSrc; ++x, ++psp, ++pdp) {
				if (*psp != cr) {
					DWORD d=*pdp, s=*psp;
					d = ((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da >> 16 << 16);
					s = ((s&0xFF00FF)*sa & 0xFF00FF00) | ((s&0xFF00)*sa >> 16 << 16);
					*pdp = (d + s) >> 8;
				}
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	CONVERT_IMAGE_END;
	return grOk;
}

int
IMAGE::putimage_withalpha(
	PIMAGE imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
) {
	inittest(L"IMAGE::putimage_alphablend");
	const PIMAGE img = CONVERT_IMAGE(imgdest);
	if (img) {
		PIMAGE imgsrc = this;
		int y, x;
		DWORD ddx, dsx;
		DWORD *pdp, *psp;
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
		for (y=0; y<nHeightSrc; ++y) {
			for (x=0; x<nWidthSrc; ++x, ++psp, ++pdp) {
				DWORD alpha = *psp >> 24;
				//if (*psp != cr)
				{
					DWORD sa = alpha + 1, da = 0xFF - alpha;
					DWORD d=*pdp, s=*psp;
					d = ((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da >> 16 << 16);
					s = ((s&0xFF00FF)*sa & 0xFF00FF00) | ((s&0xFF00)*sa >> 16 << 16);
					*pdp = (d + s) >> 8;
				}
			}
			pdp += ddx;
			psp += dsx;
		}
	}
	CONVERT_IMAGE_END;
	return grOk;
}

int
IMAGE::putimage_alphafilter(
	PIMAGE imgdest,         // handle to dest
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	PIMAGE imgalpha,        // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
) {
	inittest(L"IMAGE::putimage_alphablend");
	const PIMAGE img = CONVERT_IMAGE(imgdest);
	if (img) {
		PIMAGE imgsrc = this;
		int y, x;
		DWORD ddx, dsx;
		DWORD *pdp, *psp, *pap;
		//DWORD sa = alpha + 1, da = 0xFF - alpha;
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
		for (y=0; y<nHeightSrc; ++y) {
			for (x=0; x<nWidthSrc; ++x, ++psp, ++pdp, ++pap) {
				DWORD d=*pdp, s=*psp;
				if (*pap) {
					DWORD sa = (*pap & 0xFF) + 1, da = 0xFF - (*pap & 0xFF);
					d = ((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da >> 16 << 16);
					s = ((s&0xFF00FF)*sa & 0xFF00FF00) | ((s&0xFF00)*sa >> 16 << 16);
					*pdp = (d + s) >> 8;
				}
			}
			pdp += ddx;
			psp += dsx;
			pap += dsx;
		}
	}
	CONVERT_IMAGE_END;
	return grOk;
}


/* private function */
static
void
fix_rect_0size(PIMAGE pdest,
	int* nXOriginDest,   // x-coord of destination upper-left corner
	int* nYOriginDest,   // y-coord of destination upper-left corner
	int* nWidthDest,      // width of destination rectangle
	int* nHeightDest      // height of destination rectangle
	)
{
	struct viewporttype _vpt = {0, 0, pdest->m_width, pdest->m_height, 0};

	if(*nWidthDest == 0)
		*nWidthDest = pdest->m_width;
	if(*nHeightDest == 0)
		*nHeightDest = pdest->m_height;
	if(*nXOriginDest < _vpt.left)
		*nXOriginDest += _vpt.left - *nXOriginDest;
	if(*nYOriginDest < _vpt.top)
		*nYOriginDest += _vpt.top - *nYOriginDest;
	if (*nXOriginDest + *nWidthDest > _vpt.right)
		*nWidthDest -= *nXOriginDest + *nWidthDest - _vpt.right;
	if (*nYOriginDest + *nHeightDest > _vpt.bottom)
		*nHeightDest -= *nYOriginDest + *nHeightDest - _vpt.bottom;
}

int
IMAGE::imagefilter_blurring_4(int intensity, int alpha, int nXOriginDest,
	int nYOriginDest, int nWidthDest, int nHeightDest)
{
	inittest(L"IMAGE::imagefilter_blurring_4");
	struct _graph_setting * pg = &graph_setting;
	DWORD* buff = pg->g_t_buff;
	int x2, y2, ix, iy;
	DWORD *pdp, lsum, sumRB, sumG;
	int ddx, dldx;
	int centerintensity;
	int intensity2, intensity3, intensity4;
	int intensity2f, intensity3f, intensity4f;
	PIMAGE imgdest = this;

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
			sumRB = (pdp[dldx]&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (pdp[dldx]&0xFF00) + (pdp[1]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for (ix = nXOriginDest + 1; ix < x2; ++ix) {
			sumRB = (lsum&0xFF00FF) + (pdp[dldx]&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (lsum&0xFF00) + (pdp[dldx]&0xFF00) + (pdp[1]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum&0xFF00FF) + (pdp[dldx]&0xFF00FF);
			sumG = (lsum&0xFF00) + (pdp[dldx]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	for (iy = nYOriginDest + 1; iy < y2; ++iy) {
		ix = nXOriginDest;
		{
			sumRB = (buff[ix]&0xFF00FF) + (pdp[dldx]&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (buff[ix]&0xFF00) + (pdp[dldx]&0xFF00) + (pdp[1]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for (ix = nXOriginDest + 1; ix < x2; ++ix) {
			sumRB = (lsum&0xFF00FF) + (buff[ix]&0xFF00FF) + (pdp[dldx]&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (lsum&0xFF00) + (buff[ix]&0xFF00) + (pdp[dldx]&0xFF00) + (pdp[1]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity4 + ((sumRB * intensity4f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity4 + ((sumG * intensity4f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum&0xFF00FF) + (buff[ix]&0xFF00FF) + (pdp[dldx]&0xFF00FF);
			sumG = (lsum&0xFF00) + (buff[ix]&0xFF00) + (pdp[dldx]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[ix]&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (buff[ix]&0xFF00) + (pdp[1]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for (ix = nXOriginDest + 1; ix < x2; ++ix) {
			sumRB = (lsum&0xFF00FF) + (buff[ix]&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (lsum&0xFF00) + (buff[ix]&0xFF00) + (pdp[1]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum&0xFF00FF) + (buff[ix]&0xFF00FF);
			sumG = (lsum&0xFF00) + (buff[ix]&0xFF00);
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		//pdp += ddx;
	}
	return grOk;
}

int
IMAGE::imagefilter_blurring_8 (
	int intensity,
	int alpha,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest
) {
	inittest(L"IMAGE::imagefilter_blurring_4");
	struct _graph_setting * pg = &graph_setting;
	DWORD* buff = pg->g_t_buff, lbuf;
	int x2, y2, ix, iy;
	DWORD *pdp, lsum, sumRB, sumG;
	int ddx, dldx;
	int centerintensity;
	int intensity2, intensity3, intensity4;
	int intensity2f, intensity3f, intensity4f;

	PIMAGE imgdest = this;
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
			sumRB = (pdp[1]&0xFF00FF)
				+ (pdp[dldx]&0xFF00FF) + (pdp[dldx+1]&0xFF00FF);
			sumG = + (pdp[1]&0xFF00)
				+ (pdp[dldx]&0xFF00) + (pdp[dldx+1]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for (ix = nXOriginDest + 1; ix < x2; ++ix) {
			sumRB = (lsum&0xFF00FF) + (pdp[1]&0xFF00FF)
				+ (pdp[dldx-1]&0xFF00FF) + (pdp[dldx]&0xFF00FF) + (pdp[dldx+1]&0xFF00FF);
			sumG = (lsum&0xFF00) + (pdp[1]&0xFF00)
				+ (pdp[dldx-1]&0xFF00) + (pdp[dldx]&0xFF00) + (pdp[dldx+1]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lsum&0xFF00FF)
				+ (pdp[dldx-1]&0xFF00FF) + (pdp[dldx]&0xFF00FF);
			sumG = (lsum&0xFF00)
				+ (pdp[dldx-1]&0xFF00) + (pdp[dldx]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	for (iy = nYOriginDest + 1; iy < y2; ++iy) {
		ix = nXOriginDest;
		{
			sumRB = (buff[ix]&0xFF00FF) + (buff[ix+1]&0xFF00FF)
				+ (pdp[1]&0xFF00FF)
				+ (pdp[dldx]&0xFF00FF) + (pdp[dldx+1]&0xFF00FF);
			sumG = (buff[ix]&0xFF00) + (buff[ix+1]&0xFF00)
				+ (pdp[1]&0xFF00)
				+ (pdp[dldx]&0xFF00) + (pdp[dldx+1]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for (ix = nXOriginDest + 1; ix < x2; ++ix) {
			sumRB = (lbuf&0xFF00FF) + (buff[ix]&0xFF00FF) + (buff[ix+1]&0xFF00FF)
				+ (lsum&0xFF00FF) + (pdp[1]&0xFF00FF)
				+ (pdp[dldx-1]&0xFF00FF) + (pdp[dldx]&0xFF00FF) + (pdp[dldx+1]&0xFF00FF);
			sumG = (lbuf&0xFF00) + (buff[ix]&0xFF00) + (buff[ix+1]&0xFF00)
				+ (lsum&0xFF00) + (pdp[1]&0xFF00)
				+ (pdp[dldx-1]&0xFF00) + (pdp[dldx]&0xFF00) + (pdp[dldx+1]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity4 + ((sumRB * intensity4f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity4 + ((sumG * intensity4f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lbuf&0xFF00FF) + (buff[ix]&0xFF00FF)
				+ (lsum&0xFF00FF)
				+ (lbuf&0xFF00FF) + (pdp[dldx]&0xFF00FF);
			sumG = (buff[ix-1]&0xFF00) + (buff[ix]&0xFF00)
				+ (lsum&0xFF00)
				+ (pdp[dldx-1]&0xFF00) + (pdp[dldx]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		pdp += ddx;
	}
	{
		ix = nXOriginDest;
		{
			sumRB = (buff[ix]&0xFF00FF) + (buff[ix+1]&0xFF00FF)
				+ (pdp[1]&0xFF00FF);
			sumG = (buff[ix]&0xFF00) + (buff[ix+1]&0xFF00)
				+ (pdp[1]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		for (ix = nXOriginDest + 1; ix < x2; ++ix) {
			sumRB = (lbuf&0xFF00FF) + (buff[ix]&0xFF00FF) + (buff[ix+1]&0xFF00FF)
				+ (lsum&0xFF00FF) + (pdp[1]&0xFF00FF);
			sumG = (lbuf&0xFF00) + (buff[ix]&0xFF00) + (buff[ix+1]&0xFF00)
				+ (lsum&0xFF00) + (pdp[1]&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity3 + ((sumRB * intensity3f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity3 + ((sumG * intensity3f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		{
			sumRB = (lbuf&0xFF00FF) + (buff[ix]&0xFF00FF)
				+ (lsum&0xFF00FF);
			sumG = (lbuf&0xFF00) + (buff[ix]&0xFF00)
				+ (lsum&0xFF00);
			lbuf = buff[ix];
			buff[ix] = lsum = *pdp;
			sumRB = sumRB * intensity2 + ((sumRB * intensity2f >> 8) & 0xFF00FF) + (lsum&0xFF00FF) * centerintensity;
			sumG = sumG * intensity2 + ((sumG * intensity2f >> 8)) + (lsum&0xFF00) * centerintensity;
			*pdp = ((sumRB & 0xFF00FF00) | (sumG & 0xFF0000)) >> 8;
			++pdp;
		}
		//pdp += ddx;
	}
	return grOk;
}

int
IMAGE::imagefilter_blurring (
	int intensity,
	int alpha,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest
) {
	inittest(L"IMAGE::imagefilter_blurring");
	PIMAGE imgdest = this;

	fix_rect_0size(imgdest, &nXOriginDest, &nYOriginDest, &nWidthDest, &nHeightDest);
	if (nWidthDest <= 0 || nHeightDest <= 0) {
		return grInvalidRegion;
	}

	if (alpha < 0 || alpha > 0x100) {
		alpha = 0x100;
	}

	if (intensity <= 0x80) {
		imagefilter_blurring_4(
			intensity * 2,
			alpha,
			nXOriginDest,
			nYOriginDest,
			nWidthDest,
			nHeightDest
		);
	} else {
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
	PIMAGE imgtexture,
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
	PIMAGE imgtexture,
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

/* ege 3d �ṹ */
struct point2d {
	float x;
	float y;
};

struct point3d {
	float x;
	float y;
	float z;
};

struct vector2d {
	struct point2d p[2];
};

struct vector3d {
	struct point3d p[2];
};

typedef struct trangle2d {
	struct point2d p[3];
	int color;
}trangle2d;

struct trangle3d {
	struct point3d p[3];
	int color;
};

/* private funcion */
static
int float2int(float f) {
	if (f>=0) {
		return (int)(f + .5);
	} else {
		return (int)(f - .5);
	}
}

/* private funcion */
static
void
draw_flat_scanline(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				lp_dest_bmp_byte[dest_w * y + i] = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_scanline_transparent(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = (int)(vt->p[0].x+.5), e = (int)(vt->p[1].x+.5), y = (int)(vt->p[0].y+.5), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD  col;
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				col = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
				if (col) {
					lp_dest_bmp_byte[dest_w * y + i] = col;
				}
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_scanline_alpha(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2, int alpha) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				DWORD d = lp_dest_bmp_byte[dest_w * y + i], s = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
				d = ((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da >> 16 << 16);
				s = ((s&0xFF00FF)*sa & 0xFF00FF00) | ((s&0xFF00)*sa >> 16 << 16);
				lp_dest_bmp_byte[dest_w * y + i] = (d + s) >> 8;
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_scanline_alphatrans(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2, int alpha) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = (int)(vt->p[0].x+.5), e = (int)(vt->p[1].x+.5), y = (int)(vt->p[0].y+.5), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x + .5f, cury = _svt.p[0].y + .5f;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				DWORD s = lp_src_bmp_byte[src_w * (int)(cury) + (int)(curx)];
				if (s) {
					DWORD d = lp_dest_bmp_byte[dest_w * y + i];
					d = ((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da >> 16 << 16);
					s = ((s&0xFF00FF)*sa & 0xFF00FF00) | ((s&0xFF00)*sa >> 16 << 16);
					lp_dest_bmp_byte[dest_w * y + i] = (d + s) >> 8;
				}
			}
		}
	}
}

/* private funcion */
/* static
color_t bilinear_interpolation(color_t LT, color_t RT, color_t LB, color_t RB, double x, double y) {
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
}// */

/* private funcion */
static
void
draw_flat_scanline_s(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD Trb, Tg, Brb, Bg, crb, cg;
	DWORD alphaA, alphaB;
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				int ix = (int)curx, iy = (int)cury;
				double fx = curx - ix, fy = cury - iy;
				DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w+1],
					fx,
					fy
					);
				lp_dest_bmp_byte[dest_w * y + i] = col;
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_scanline_transparent_s(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD Trb, Tg, Brb, Bg, crb, cg;
	DWORD alphaA, alphaB;
	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				int ix = (int)curx, iy = (int)cury;
				float fx = curx - ix, fy = cury - iy;
				DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w+1],
					fx,
					fy
					);
				if (col) {
					lp_dest_bmp_byte[dest_w * y + i] = col;
				}
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_scanline_alpha_s(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2, int alpha) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD Trb, Tg, Brb, Bg, crb, cg;
	DWORD alphaA, alphaB;
	DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				int ix = (int)curx, iy = (int)cury;
				float fx = curx - ix, fy = cury - iy;
				DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w+1],
					fx,
					fy
					);
				{
					DWORD d = lp_dest_bmp_byte[dest_w * y + i];
					d = (((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da & 0xFF0000)) >> 8;
					col = (((col&0xFF00FF)*sa & 0xFF00FF00) | ((col&0xFF00)*sa & 0xFF0000)) >> 8;
					lp_dest_bmp_byte[dest_w * y + i] = d + col;
				}
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_scanline_alphatrans_s(PIMAGE dc_dest, const struct vector2d* vt, PIMAGE dc_src, const struct vector2d* svt, int x1, int x2, int alpha) {
	float dw = vt->p[1].x - vt->p[0].x, rw = svt->p[1].x - svt->p[0].x;
	int s = float2int((float)vt->p[0].x), e = float2int((float)vt->p[1].x), y = float2int((float)vt->p[0].y), w = e - s;
	DWORD* lp_dest_bmp_byte = (DWORD*)dc_dest->getbuffer();
	DWORD* lp_src_bmp_byte = (DWORD*)dc_src->getbuffer();
	DWORD Trb, Tg, Brb, Bg, crb, cg;
	DWORD alphaA, alphaB;
	DWORD sa = alpha, da = 0xFF - sa;

	int dest_w = dc_dest->getwidth();
	int src_w = dc_src->getwidth();
	//int src_h = dc_src->h;

	if (w>0) {
		int i, bx = s;
		struct vector2d _svt = *svt;
		_svt.p[0].x += (float)((s - vt->p[0].x) * rw / dw);
		_svt.p[1].x += (float)((e - vt->p[1].x) * rw / dw);
		{
			float curx = _svt.p[0].x, cury = _svt.p[0].y;
			float dx = (_svt.p[1].x - _svt.p[0].x) / w;
			float dy = (_svt.p[1].y - _svt.p[0].y) / w;

			if (s<x1) {
				s = x1;
			}
			if (e>x2) {
				e = x2;
			}
			curx += (s - bx) * dx;
			cury += (s - bx) * dy;

			for (i = s; i<e; ++i, curx += dx, cury += dy) {
				int ix = (int)curx, iy = (int)cury;
				float fx = curx - ix, fy = cury - iy;
				DWORD* lp_src_byte = lp_src_bmp_byte + src_w * iy + ix;
				DWORD col;
				BILINEAR_INTERPOLATION(
					col,
					lp_src_byte[0],
					lp_src_byte[1],
					lp_src_byte[src_w],
					lp_src_byte[src_w+1],
					fx,
					fy
					);
				if (col) {
					DWORD d = lp_dest_bmp_byte[dest_w * y + i];
					d = (((d&0xFF00FF)*da & 0xFF00FF00) | ((d&0xFF00)*da & 0xFF0000)) >> 8;
					col = (((col&0xFF00FF)*sa & 0xFF00FF00) | ((col&0xFF00)*sa & 0xFF0000)) >> 8;
					lp_dest_bmp_byte[dest_w * y + i] = d + col;
				}
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_trangle_alpha(PIMAGE dc_dest, const struct trangle2d* dt, PIMAGE dc_src, const struct trangle2d* tt, int x1, int y1, int x2, int y2, int transparent, int alpha) {
	struct trangle2d  t2d = *dt;
	struct trangle2d  t3d = *tt;
	int b_alpha;
	//int width = x2 - x1, height = y2 - y1;

	if (alpha >= 0 && alpha < 0x100) {
		b_alpha = 1;
	} else {
		b_alpha = 0;
	}
	{
		struct point2d _t;
		struct point2d _t3;

		if (t2d.p[1].y > t2d.p[2].y) {
			SWAP(t2d.p[1], t2d.p[2], _t);
			SWAP(t3d.p[1], t3d.p[2], _t3);
		}
		if (t2d.p[0].y > t2d.p[1].y) {
			SWAP(t2d.p[0], t2d.p[1], _t);
			SWAP(t3d.p[0], t3d.p[1], _t3);
		}
		if (t2d.p[1].y > t2d.p[2].y) {
			SWAP(t2d.p[1], t2d.p[2], _t);
			SWAP(t3d.p[1], t3d.p[2], _t3);
		}
	}
	{
		float dd;
		int s = float2int((float)t2d.p[0].y), e = float2int((float)t2d.p[2].y), h, m = float2int((float)t2d.p[1].y);
		int rs, re;
		int i, lh, rh;
	//	float dm = t2d.p[1].y;
		struct point2d pl, pr, pt;
		struct point2d spl, spr;

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
		if (s < y1) {
			s = y1;
		}
		if (m >= y2) {
			m = y2;
		}
		/*if (pl.y > pr.y) {
		dd = pr.y / pl.y;
		pl.x *= dd;
		spl.x *= dd;
		spl.y *= dd;
		} else {
		dd = pl.y / pr.y;
		pr.x *= dd;
		spr.x *= dd;
		spr.y *= dd;
		}// */
		if (pl.x > pr.x) {
			SWAP(pl, pr, pt);
			SWAP(spl, spr, pt);
		} else {
			;
		}
		lh = float2int((float)(pl.y + t2d.p[0].y)) - float2int((float)(t2d.p[0].y));
		rh = float2int((float)(pr.y + t2d.p[0].y)) - float2int((float)(t2d.p[0].y));
		if (h > 0) {
			for (i=s; i<m; ++i) {
				struct vector2d vt;
				struct vector2d svt;
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

				if (b_alpha) {
					if (transparent) {
						draw_flat_scanline_alphatrans(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					} else {
						draw_flat_scanline_alpha(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					}
				} else {
					if (transparent) {
						draw_flat_scanline_transparent(dc_dest, &vt, dc_src, &svt, x1, x2);
					} else {
						draw_flat_scanline(dc_dest, &vt, dc_src, &svt, x1, x2);
					}
				}
			}
		}
		if (pl.y > pr.y) {
			dd = pr.y / pl.y;
			pl.x *= dd;
			spl.x *= dd;
			spl.y *= dd;
		} else {
			dd = pl.y / pr.y;
			pr.x *= dd;
			spr.x *= dd;
			spr.y *= dd;
		}
		if (m >= y1 && m < y2 && m<e) {
			struct vector2d vt;
			struct vector2d svt;

			vt.p[0].x = t2d.p[0].x + pl.x;
			vt.p[1].x = t2d.p[0].x + pr.x;
			vt.p[0].y = vt.p[1].y = (float)(m);

			svt.p[0].x = t3d.p[0].x + spl.x;
			svt.p[1].x = t3d.p[0].x + spr.x;
			svt.p[0].y = t3d.p[0].y + spl.y;
			svt.p[1].y = t3d.p[0].y + spr.y;

			if (b_alpha) {
				if (transparent) {
					draw_flat_scanline_alphatrans(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				} else {
					draw_flat_scanline_alpha(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				}
			} else {
				if (transparent) {
					draw_flat_scanline_transparent(dc_dest, &vt, dc_src, &svt, x1, x2);
				} else {
					draw_flat_scanline(dc_dest, &vt, dc_src, &svt, x1, x2);
				}
			}
		}// */
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
		if (m < y1) {
			m = y1 - 1;
		}
		if (e >= y2) {
			e = y2 - 1;
		}
		/*if (pl.y < pr.y) {
		dd = pr.y / pl.y;
		pl.x *= dd;
		spl.x *= dd;
		spl.y *= dd;
		} else {
		dd = pl.y / pr.y;
		pr.x *= dd;
		spr.x *= dd;
		spr.y *= dd;
		}// */
		if (pl.x > pr.x) {
			SWAP(pl, pr, pt);
			SWAP(spl, spr, pt);
		} else {
			;
		}
		lh = float2int((float)(t2d.p[2].y)) - float2int((float)(pl.y + t2d.p[2].y));
		rh = float2int((float)(t2d.p[2].y)) - float2int((float)(pr.y + t2d.p[2].y));
		if (h > 0) {
			for (i=e; i>m; --i) {
				struct vector2d vt;
				struct vector2d svt;
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


				if (b_alpha) {
					if (transparent) {
						draw_flat_scanline_alphatrans(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					} else {
						draw_flat_scanline_alpha(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					}
				} else {
					if (transparent) {
						draw_flat_scanline_transparent(dc_dest, &vt, dc_src, &svt, x1, x2);
					} else {
						draw_flat_scanline(dc_dest, &vt, dc_src, &svt, x1, x2);
					}
				}
			}
		}
	}
}

/* private funcion */
static
void
draw_flat_trangle_alpha_s(PIMAGE dc_dest, const struct trangle2d* dt, PIMAGE dc_src, const struct trangle2d* tt, int x1, int y1, int x2, int y2, int transparent, int alpha) {
	struct trangle2d  t2d = *dt;
	struct trangle2d  t3d = *tt;
	int b_alpha;
	//int width = x2 - x1, height = y2 - y1;

	if (alpha >= 0 && alpha < 0x100) {
		b_alpha = 1;
	} else {
		b_alpha = 0;
	}
	{
		struct point2d _t;
		struct point2d _t3;

		if (t2d.p[1].y > t2d.p[2].y) {
			SWAP(t2d.p[1], t2d.p[2], _t);
			SWAP(t3d.p[1], t3d.p[2], _t3);
		}
		if (t2d.p[0].y > t2d.p[1].y) {
			SWAP(t2d.p[0], t2d.p[1], _t);
			SWAP(t3d.p[0], t3d.p[1], _t3);
		}
		if (t2d.p[1].y > t2d.p[2].y) {
			SWAP(t2d.p[1], t2d.p[2], _t);
			SWAP(t3d.p[1], t3d.p[2], _t3);
		}
	}
	{
		float dd;
		int s = float2int((float)t2d.p[0].y), e = float2int((float)t2d.p[2].y), h, m = float2int((float)t2d.p[1].y);
		int rs, re;
		int i, lh, rh;
	//	float dm = t2d.p[1].y;
	//	float dh;
		struct point2d pl, pr, pt;
		struct point2d spl, spr;

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
		if (s < y1) {
			s = y1;
		}
		if (m >= y2) {
			m = y2;
		}
		/*if (pl.y > pr.y) {
		dd = pr.y / pl.y;
		pl.x *= dd;
		spl.x *= dd;
		spl.y *= dd;
		} else {
		dd = pl.y / pr.y;
		pr.x *= dd;
		spr.x *= dd;
		spr.y *= dd;
		}// */
		if (pl.x > pr.x) {
			SWAP(pl, pr, pt);
			SWAP(spl, spr, pt);
		} else {
			;
		}
		lh = float2int((float)(pl.y + t2d.p[0].y)) - float2int((float)(t2d.p[0].y));
		rh = float2int((float)(pr.y + t2d.p[0].y)) - float2int((float)(t2d.p[0].y));
		if (h > 0) {
			for (i=s; i<m; ++i) {
				struct vector2d vt;
				struct vector2d svt;
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

				if (b_alpha) {
					if (transparent) {
						draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					} else {
						draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					}
				} else {
					if (transparent) {
						draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt, x1, x2);
					} else {
						draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1, x2);
					}
				}
			}
		}
		if (pl.y > pr.y) {
			dd = pr.y / pl.y;
			pl.x *= dd;
			spl.x *= dd;
			spl.y *= dd;
		} else {
			dd = pl.y / pr.y;
			pr.x *= dd;
			spr.x *= dd;
			spr.y *= dd;
		}
		if (m >= y1 && m < y2 && m<e) {
			struct vector2d vt;
			struct vector2d svt;

			vt.p[0].x = t2d.p[0].x + pl.x;
			vt.p[1].x = t2d.p[0].x + pr.x;
			vt.p[0].y = vt.p[1].y = (float)(m);

			svt.p[0].x = t3d.p[0].x + spl.x;
			svt.p[1].x = t3d.p[0].x + spr.x;
			svt.p[0].y = t3d.p[0].y + spl.y;
			svt.p[1].y = t3d.p[0].y + spr.y;

			if (b_alpha) {
				if (transparent) {
					draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				} else {
					draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
				}
			} else {
				if (transparent) {
					draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt, x1, x2);
				} else {
					draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1, x2);
				}
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
		if (m < y1) {
			m = y1 - 1;
		}
		if (e >= y2) {
			e = y2 - 1;
		}
		/*if (pl.y < pr.y) {
		dd = pr.y / pl.y;
		pl.x *= dd;
		spl.x *= dd;
		spl.y *= dd;
		} else {
		dd = pl.y / pr.y;
		pr.x *= dd;
		spr.x *= dd;
		spr.y *= dd;
		}// */
		if (pl.x > pr.x) {
			SWAP(pl, pr, pt);
			SWAP(spl, spr, pt);
		} else {
			;
		}
		lh = float2int((float)(t2d.p[2].y)) - float2int((float)(pl.y + t2d.p[2].y));
		rh = float2int((float)(t2d.p[2].y)) - float2int((float)(pr.y + t2d.p[2].y));
		if (h > 0) {
			for (i=e; i>m; --i) {
				struct vector2d vt;
				struct vector2d svt;
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


				if (b_alpha) {
					if (transparent) {
						draw_flat_scanline_alphatrans_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					} else {
						draw_flat_scanline_alpha_s(dc_dest, &vt, dc_src, &svt, x1, x2, alpha);
					}
				} else {
					if (transparent) {
						draw_flat_scanline_transparent_s(dc_dest, &vt, dc_src, &svt, x1, x2);
					} else {
						draw_flat_scanline_s(dc_dest, &vt, dc_src, &svt, x1, x2);
					}
				}
			}
		}
	}
}

int
putimage_trangle(
				   PIMAGE imgdest,
				   PIMAGE imgtexture,
				   const struct trangle2d * dt,// dest trangle, original
				   const struct trangle2d * tt,// textture trangle uv 0.0 - 1.0
				   int btransparent,
				   int alpha,
				   int smooth
				   )
{
	PIMAGE dc_dest = imgdest;
	PIMAGE dc_src  = imgtexture;
	if (dc_dest) {
		struct trangle2d _dt = *dt;
		struct trangle2d _tt = *tt;
		int x1 = 0, y1 = 0, x2 = dc_dest->getwidth(), y2 = dc_dest->getheight(), i;

		if (smooth) {
			for (i=0; i<3; ++i) {
				_tt.p[i].x = (float)float2int((float)(_tt.p[i].x * (dc_src->getwidth() - 2)));
				_tt.p[i].y = (float)float2int((float)(_tt.p[i].y * (dc_src->getheight() - 2)));
			}
		} else {
			for (i=0; i<3; ++i) {
				_tt.p[i].x = (float)float2int((float)(_tt.p[i].x * (dc_src->getwidth() - 1)));
				_tt.p[i].y = (float)float2int((float)(_tt.p[i].y * (dc_src->getheight() - 1)));
			}
		}

		if (smooth) {
			if (dc_src->getwidth() > 1 && dc_src->getheight() > 1) {
				draw_flat_trangle_alpha_s(dc_dest, &_dt, dc_src, &_tt, x1, y1, x2, y2, btransparent, alpha);
			}
		} else {
			draw_flat_trangle_alpha(dc_dest, &_dt, dc_src, &_tt, x1, y1, x2, y2, btransparent, alpha);
		}
	}
	return grOk;
}

int
putimage_rotate(
				  PIMAGE imgdest,
				  PIMAGE imgtexture,
				  int nXOriginDest,
				  int nYOriginDest,
				  float centerx,
				  float centery,
				  float radian,
				  int btransparent,           // transparent (1) or not (0)
				  int alpha,                  // in range[0, 256], alpha==256 means no alpha
				  int smooth
				  )
{
	PIMAGE dc_dest  = CONVERT_IMAGE(imgdest);
	PIMAGE dc_src  = imgtexture;
	if (dc_dest) {
		struct trangle2d _tt[2];
		struct trangle2d _dt[2];
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
		memcpy(&_dt, &_tt, sizeof(trangle2d)*2);
		for (j = 0; j<2; ++j) {
			for (i = 0; i<3; ++i) {
				_dt[j].p[i].x = (_dt[j].p[i].x - centerx) * (dc_src->getwidth());
				_dt[j].p[i].y = (_dt[j].p[i].y - centery) * (dc_src->getheight());
				dx = cr * _dt[j].p[i].x - sr * _dt[j].p[i].y;
				dy = sr * _dt[j].p[i].x + cr * _dt[j].p[i].y;
				_dt[j].p[i].x = (float)float2int((float)((dx + nXOriginDest) + FLOAT_EPS));
				_dt[j].p[i].y = (float)float2int((float)((dy + nYOriginDest) + FLOAT_EPS));
			}
		}
		putimage_trangle(
			dc_dest,
			imgtexture,
			&_dt[0],
			&_tt[0],
			btransparent,
			alpha,
			smooth
			);
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
	CONVERT_IMAGE_END;
	return grOk;
}

int
putimage_rotatezoom(
					  PIMAGE imgdest,
					  PIMAGE imgtexture,
					  int nXOriginDest,
					  int nYOriginDest,
					  float centerx,
					  float centery,
					  float radian,
					  float zoom,
					  int btransparent,           // transparent (1) or not (0)
					  int alpha,                  // in range[0, 256], alpha==256 means no alpha
					  int smooth
					  )
{
	PIMAGE dc_dest  = CONVERT_IMAGE(imgdest);
	PIMAGE dc_src  = imgtexture;
	if (dc_dest) {
		struct trangle2d _tt[2];
		struct trangle2d _dt[2];
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
		memcpy(&_dt, &_tt, sizeof(trangle2d)*2);
		for (j = 0; j<2; ++j) {
			for (i = 0; i<3; ++i) {
				_dt[j].p[i].x = (_dt[j].p[i].x - centerx) * (dc_src->getwidth());
				_dt[j].p[i].y = (_dt[j].p[i].y - centery) * (dc_src->getheight());
				dx = cr * _dt[j].p[i].x - sr * _dt[j].p[i].y;
				dy = sr * _dt[j].p[i].x + cr * _dt[j].p[i].y;
				_dt[j].p[i].x = (float)float2int((float)((dx * zoom + nXOriginDest) + FLOAT_EPS));
				_dt[j].p[i].y = (float)float2int((float)((dy * zoom + nYOriginDest) + FLOAT_EPS));
			}
		}
		putimage_trangle(
			dc_dest,
			imgtexture,
			&_dt[0],
			&_tt[0],
			btransparent,
			alpha,
			smooth
			);
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
	CONVERT_IMAGE_END;
	return grOk;
}




int
getwidth(PIMAGE pimg) {
	PIMAGE img = CONVERT_IMAGE_CONST(pimg);
	CONVERT_IMAGE_END;
	if (img) {
		return img->m_width;
	}
	return 0;
}

int
getheight(PIMAGE pimg) {
	PIMAGE img = CONVERT_IMAGE_CONST(pimg);
	CONVERT_IMAGE_END;
	if (img) {
		return img->m_height;
	}
	return 0;
}

int
getx(PIMAGE pimg) {
	PIMAGE img = CONVERT_IMAGE_CONST(pimg);
	if (img) {
		POINT pt;
		GetCurrentPositionEx(img->m_hDC, &pt);
		CONVERT_IMAGE_END;
		return pt.x;
	}
	CONVERT_IMAGE_END;
	return -1;
}

int
gety(PIMAGE pimg) {
	PIMAGE img = CONVERT_IMAGE_CONST(pimg);
	if (img) {
		POINT pt;
		GetCurrentPositionEx(img->m_hDC, &pt);
		CONVERT_IMAGE_END;
		return pt.y;
	}
	CONVERT_IMAGE_END;
	return -1;
}

PIMAGE
newimage() {
	return new IMAGE(1, 1);
}

PIMAGE
newimage(int width, int height) {
	if (width  < 1) width  = 1;
	if (height < 1) height = 1;
	return new IMAGE(width, height);
}

void
delimage(PIMAGE pImg) {
	delete pImg;
}

void* getbuffer(PIMAGE pImg) {
	PIMAGE img = CONVERT_IMAGE_CONST(pImg);
	CONVERT_IMAGE_END;
	return img->getbuffer();
}

int
resize(PIMAGE pDstImg, int width, int height) {
	return pDstImg->resize(width, height);
}

void
getimage(PIMAGE pDstImg, int srcX, int srcY, int srcWidth, int srcHeight) {
	pDstImg->getimage(srcX, srcY, srcWidth, srcHeight);
}

void
getimage(PIMAGE pDstImg, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight) {
	pDstImg->getimage(pSrcImg, srcX, srcY, srcWidth, srcHeight);
}

void
putimage(int dstX, int dstY, const PIMAGE pSrcImg, DWORD dwRop) {
	pSrcImg->putimage(dstX, dstY, dwRop);
}

void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, DWORD dwRop) {
	pSrcImg->putimage(dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
}

void
putimage(PIMAGE pDstImg, int dstX, int dstY, const PIMAGE pSrcImg, DWORD dwRop) {
	pSrcImg->putimage(pDstImg, dstX, dstY, dwRop);
}

void
putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, DWORD dwRop) {
	pSrcImg->putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
}

int
getimage(PIMAGE pDstImg, LPCSTR pImgFile, int zoomWidth, int zoomHeight) {
	return pDstImg->getimage(pImgFile, zoomWidth, zoomHeight);
}

int
getimage(PIMAGE pDstImg, LPCWSTR pImgFile, int zoomWidth, int zoomHeight) {
	return pDstImg->getimage(pImgFile, zoomWidth, zoomHeight);
}

int
getimage(PIMAGE pDstImg, LPCSTR pResType, LPCSTR pResName, int zoomWidth, int zoomHeight) {
	return pDstImg->getimage(pResType, pResName, zoomWidth, zoomHeight);
}

int
getimage(PIMAGE pDstImg, LPCWSTR pResType, LPCWSTR pResName, int zoomWidth, int zoomHeight) {
	return pDstImg->getimage(pResType, pResName, zoomWidth, zoomHeight);
}

void
putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop) {
	pSrcImg->putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX, srcY, srcWidth, srcHeight, dwRop);
}

void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop) {
	pSrcImg->putimage(NULL, dstX, dstY, dstWidth, dstHeight, srcX, srcY, srcWidth, srcHeight, dwRop);
}

int
putimage_transparent(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
	)
{
	return imgsrc->putimage_transparent(imgdest, nXOriginDest, nYOriginDest, crTransparent, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_alphablend(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
	)
{
	return imgsrc->putimage_alphablend(imgdest, nXOriginDest, nYOriginDest, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_alphatransparent(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
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
	return imgsrc->putimage_alphatransparent(imgdest, nXOriginDest, nYOriginDest, crTransparent, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_withalpha(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
	)
{
	return imgsrc->putimage_withalpha(imgdest, nXOriginDest, nYOriginDest, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
imagefilter_blurring (
	PIMAGE imgdest,         // handle to dest
	int intensity,
	int alpha,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest
 )
{
	const PIMAGE img = CONVERT_IMAGE(imgdest);
	int ret = 0;
	if (img) {
		ret = img->imagefilter_blurring(intensity, alpha, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest);
	}
	CONVERT_IMAGE_END;
	return ret;
}

int
saveimage(PIMAGE pimg, LPCSTR  filename) {
	const PIMAGE img = CONVERT_IMAGE(pimg);
	int ret = 0;
	if (img) {
		ret = img->saveimage(filename);
	}
	CONVERT_IMAGE_END;
	return ret;
}

int
saveimage(PIMAGE pimg, LPCWSTR filename) {
	const PIMAGE img = CONVERT_IMAGE(pimg);
	int ret = 0;
	if (img) {
		ret = img->saveimage(filename);
	}
	CONVERT_IMAGE_END;
	return ret;
}


int
getimage_pngfile(PIMAGE pimg, LPCSTR  filename) {
	FILE *fp = NULL;
	int ret;
	fp = fopen(filename, "rb");
	if (fp == NULL) return grFileNotFound;
	ret = pimg->getpngimg(fp);
	fclose(fp);
	return ret;
}

int
getimage_pngfile(PIMAGE pimg, LPCWSTR filename) {
	FILE *fp = NULL;
	int ret;
	fp = _wfopen(filename, L"rb");
	if (fp == NULL) return grFileNotFound;
	ret = pimg->getpngimg(fp);
	fclose(fp);
	return ret;
}

int
savepng(PIMAGE pimg, LPCSTR  filename, int bAlpha) {
	FILE *fp = NULL;
	int ret;
	fp = fopen(filename, "wb");
	if (fp == NULL) return grFileNotFound;
	ret = pimg->savepngimg(fp, bAlpha);
	fclose(fp);
	return ret;
}

int
savepng(PIMAGE pimg, LPCWSTR filename, int bAlpha) {
	FILE *fp = NULL;
	int ret;
	fp = _wfopen(filename, L"wb");
	if (fp == NULL) return grFileNotFound;
	ret = pimg->savepngimg(fp, bAlpha);
	fclose(fp);
	return ret;
}

void
ege_enable_aa(bool enable, PIMAGE pimg) {
	PIMAGE img  = CONVERT_IMAGE(pimg);
	img->m_aa = enable;
	CONVERT_IMAGE_END;
}

} // namespace ege
