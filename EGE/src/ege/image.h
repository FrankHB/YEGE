#ifndef Inc_ege_image_h_
#define Inc_ege_image_h_

#include "head.h" // for unique_ptr, etc;
#include "ege/viewport.h"
#include "ege/windows.h"
#if YEGE_Use_YSLib
#	include "ege/color.h"
#else
#	include "ege/gapi.h"
#	include "global.h"
#	include <Windows.h>
#endif
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>

namespace ege
{

inline Size
ToSize(int w, int h)
{
	if(w <= 0 || h <= 0)
		throw std::invalid_argument("ege::ToSize");
	return {w, h};
}


// 定义图像对象
class IMAGE
{
private:
	ScreenBuffer sbuf;
	::HDC m_hDC;

public:
	color_t m_color;
	color_t m_fillcolor;
	bool m_aa = {};
	viewporttype m_vpt;
	textsettingstype m_texttype;
	linestyletype m_linestyle;
	float m_linewidth;
	color_t m_bk_color;
	unique_ptr<Gdiplus::Brush> m_pattern{};
	unique_ptr<Gdiplus::Image> m_texture{};

	IMAGE();
	IMAGE(int, int);
	IMAGE(::HDC, int, int);
	IMAGE(const Size&);
	IMAGE(::HDC, const Size&);
	IMAGE(const IMAGE&);
	IMAGE(IMAGE&&) ynothrow;
	~IMAGE();

	IMAGE&
	operator=(IMAGE) ynothrow;

	DefGetter(const ynothrow, ::HBITMAP, Bitmap, sbuf.GetNativeHandle())
	DefGetterMem(const ynothrow, BitmapPtr, BufferPtr, sbuf)
	DefGetter(const ynothrow, SDst, Height, GetSize().Height)
	DefGetterMem(const ynothrow, const Size&, Size, sbuf)
	DefGetter(const ynothrow, SDst, Width, GetSize().Width)

	void
	SetViewport(int, int, int, int, int);

	void
	swap(IMAGE&) ynothrow;

	void
	gentexture(bool gen);

	::HDC
	getdc() const
	{
		return m_hDC;
	}

private:
	int
	Refresh(::HBITMAP);

public:
	unsigned long*
	getbuffer() const
	{
#if YEGE_Use_YSLib
		static_assert(sizeof(unsigned long) == sizeof(YSLib::Drawing::Pixel),
			"");
		static_assert(
			yalignof(unsigned long) == yalignof(YSLib::Drawing::Pixel), "");
#endif

		return reinterpret_cast<unsigned long*>(sbuf.GetBufferPtr());
	}

	int
	Resize(const Size&);
	int
	Resize(int width, int height)
	{
		return Resize(ToSize(width, height));
	}

	void
	getimage(const IMAGE*, int, int, int, int);
	graphics_errors
	getimage(::HRSRC);
	graphics_errors
	getimage(void*, unsigned long);
	graphics_errors
	getimage(const char*);
	graphics_errors
	getimage(const wchar_t*);
	graphics_errors
	getimage(const char*, const char*);
	graphics_errors
	getimage(const wchar_t*, const wchar_t*);

#if YEGE_Use_YSLib
	graphics_errors
	getimage_b(HBitmap&&);
#else
	graphics_errors
	getimage_b(void*);
#endif

	void
	putimage(int, int, unsigned long = SRCCOPY) const;
	void
	putimage(int, int, int, int, int, int, unsigned long = SRCCOPY) const;
	void
	putimage(IMAGE*, int, int, unsigned long = SRCCOPY) const;
	void
	putimage(IMAGE*, int, int, int, int, int, int, unsigned long = SRCCOPY)
		const;
	void
	putimage(IMAGE*, int, int, int, int, int, int, int, int,
		unsigned long = SRCCOPY) const;

#if YEGE_Use_YSLib
	int
	saveimage(const char* filename, ImageFormat = ImageFormat::BMP) const;
	int
	saveimage(const wchar_t* filename, ImageFormat = ImageFormat::BMP) const;
#else
	int
	saveimage(const char* filename) const;
	int
	saveimage(const wchar_t* filename) const;

	int
	savepngimg(std::FILE* fp, int bAlpha) const;
#endif

	int
	getpngimg(std::FILE* fp);

	int
	putimage_transparent(
		IMAGE* imgdest,         // handle to dest
		int nXOriginDest,       // x-coord of destination upper-left corner
		int nYOriginDest,       // y-coord of destination upper-left corner
		color_t crTransparent,  // color to make transparent
		int nXOriginSrc = 0,    // x-coord of source upper-left corner
		int nYOriginSrc = 0,    // y-coord of source upper-left corner
		int nWidthSrc = 0,      // width of source rectangle
		int nHeightSrc = 0      // height of source rectangle
	) const;

	int
	putimage_alphablend(
		IMAGE* imgdest,         // handle to dest
		int nXOriginDest,       // x-coord of destination upper-left corner
		int nYOriginDest,       // y-coord of destination upper-left corner
		unsigned char alpha,    // alpha
		int nXOriginSrc = 0,    // x-coord of source upper-left corner
		int nYOriginSrc = 0,    // y-coord of source upper-left corner
		int nWidthSrc = 0,      // width of source rectangle
		int nHeightSrc = 0      // height of source rectangle
	) const;

	int
	putimage_alphatransparent(
		IMAGE* imgdest,         // handle to dest
		int nXOriginDest,       // x-coord of destination upper-left corner
		int nYOriginDest,       // y-coord of destination upper-left corner
		color_t crTransparent,  // color to make transparent
		unsigned char alpha,    // alpha
		int nXOriginSrc = 0,    // x-coord of source upper-left corner
		int nYOriginSrc = 0,    // y-coord of source upper-left corner
		int nWidthSrc = 0,      // width of source rectangle
		int nHeightSrc = 0      // height of source rectangle
	) const;

	int
	putimage_withalpha(
		IMAGE* imgdest,         // handle to dest
		int nXOriginDest,       // x-coord of destination upper-left corner
		int nYOriginDest,       // y-coord of destination upper-left corner
		int nXOriginSrc = 0,    // x-coord of source upper-left corner
		int nYOriginSrc = 0,    // y-coord of source upper-left corner
		int nWidthSrc = 0,      // width of source rectangle
		int nHeightSrc = 0      // height of source rectangle
	) const;

	int
	putimage_alphafilter(
		IMAGE* imgdest,         // handle to dest
		int nXOriginDest,       // x-coord of destination upper-left corner
		int nYOriginDest,       // y-coord of destination upper-left corner
		IMAGE* imgalpha,        // alpha
		int nXOriginSrc = 0,    // x-coord of source upper-left corner
		int nYOriginSrc = 0,    // y-coord of source upper-left corner
		int nWidthSrc = 0,      // width of source rectangle
		int nHeightSrc = 0      // height of source rectangle
	) const;

	int
	imagefilter_blurring_4(
		int intensity,
		int alpha,
		int nXOriginDest,
		int nYOriginDest,
		int nWidthDest,
		int nHeightDest
	);

	int
	imagefilter_blurring_8(
		int intensity,
		int alpha,
		int nXOriginDest,
		int nYOriginDest,
		int nWidthDest,
		int nHeightDest
	);

	int
	imagefilter_blurring(
		int intensity,
		int alpha,
		int nXOriginDest = 0,
		int nYOriginDest = 0,
		int nWidthDest = 0,
		int nHeightDest = 0
	);

	int putimage_rotate(
		const IMAGE* imgtexture,
		int nXOriginDest,
		int nYOriginDest,
		float centerx,
		float centery,
		float radian,
		int btransparent = 0, // transparent (1) or not (0)
		int alpha = -1, // in range[0, 256], alpha== -1 means no alpha
		int smooth = 0
	);

	int putimage_rotatezoom(
		const IMAGE* imgtexture,
		int nXOriginDest,
		int nYOriginDest,
		float centerx,
		float centery,
		float radian,
		float zoom,
		int btransparent = 0, // transparent (1) or not (0)
		int alpha = -1, // in range[0, 256], alpha== -1 means no alpha
		int smooth = 0
	);
};


IMAGE&
cimg_ref(IMAGE*);
const IMAGE&
cimg_ref(const IMAGE*);

IMAGE&
cimg_ref_c(IMAGE*);
const IMAGE&
cimg_ref_c(const IMAGE*);

} // namespace ege;

#endif

