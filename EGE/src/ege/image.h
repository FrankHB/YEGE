#ifndef Inc_ege_image_h_
#define Inc_ege_image_h_

#include "ege/base.h"
#include "ege/color.h"
#include "ege/viewport.h"
#include <memory> // for std::unique_ptr;
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>
#include "head.h"
#include YFM_YSLib_Adaptor_Image

namespace ege
{

using platform_ex::ScreenBuffer;

inline Size
ToSize(int w, int h)
{
	if(w < 0 || h < 0)
		throw std::invalid_argument("ege::ToSize");
	return {w, h};
}


// 定义图像对象
class IMAGE
{
private:
	ScreenBuffer sbuf;
	::HDC m_hDC;

	::HBITMAP
	GetBitmap() const
	{
		return sbuf.GetNativeHandle();
	}

	int
	Refresh();

public:
	color_t m_color;
	color_t m_fillcolor;
	bool m_aa = {};
	viewporttype m_vpt;
	textsettingstype m_texttype;
	linestyletype m_linestyle;
	float m_linewidth;
	color_t m_bk_color;
	std::unique_ptr<Gdiplus::Brush> m_pattern{};
	std::unique_ptr<Gdiplus::Image> m_texture{};

	IMAGE();
	IMAGE(int, int);
	IMAGE(const Size&);
	IMAGE(::HDC, int, int);
	IMAGE(::HDC, const Size&);
	IMAGE(const IMAGE&);
	IMAGE(IMAGE&&) ynothrow;
	~IMAGE();

	IMAGE&
	operator=(IMAGE) ynothrow;

	DefGetterMem(const ynothrow, PixelType*, BufferPtr, sbuf)
	DefGetter(const ynothrow, SDst, Height, GetSize().Height)
	DefGetterMem(const ynothrow, const Size&, Size, sbuf)
	DefGetter(const ynothrow, SDst, Width, GetSize().Width)

	void
	swap(IMAGE&) ynothrow;

	void
	gentexture(bool gen);

	::HDC
	getdc() const
	{
		return m_hDC;
	}

	::DWORD*
	getbuffer() const
	{
		static_assert(sizeof( ::DWORD) == sizeof(YSLib::Drawing::PixelType), "");
		static_assert(yalignof( ::DWORD) == yalignof(YSLib::Drawing::PixelType), "");

		return reinterpret_cast<::DWORD*>(sbuf.GetBufferPtr());
	}

	int
	Resize(const Size&);
	int
	Resize(int width, int height)
	{
		return Resize(ToSize(width, height));
	}

	graphics_errors
	getimage(HBitmap&&);
	graphics_errors
	getimage(ystdex::octet*, size_t);
	graphics_errors
	getimage(const char*);
	graphics_errors
	getimage(const wchar_t*);
	graphics_errors
	getimage(const char*, const char*);
	graphics_errors
	getimage(const wchar_t*, const wchar_t*);
	void
	getimage(IMAGE*, int, int, int, int);

	void
	putimage(int dstX, int dstY, ::DWORD dwRop = SRCCOPY) const;
	void
	putimage(int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, ::DWORD dwRop = SRCCOPY) const;
	void
	putimage(IMAGE* pDstImg, int dstX, int dstY, ::DWORD dwRop = SRCCOPY) const;
	void
	putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, ::DWORD dwRop = SRCCOPY) const;
	void
	putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop = SRCCOPY) const;

	int
	saveimage(const char*  filename);
	int
	saveimage(const wchar_t* filename);
	int
	getpngimg(std::FILE* fp);
	int
	savepngimg(std::FILE* fp, int bAlpha);

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
	);

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
	);

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
	);

	int
	putimage_withalpha(
		IMAGE* imgdest,         // handle to dest
		int nXOriginDest,       // x-coord of destination upper-left corner
		int nYOriginDest,       // y-coord of destination upper-left corner
		int nXOriginSrc = 0,    // x-coord of source upper-left corner
		int nYOriginSrc = 0,    // y-coord of source upper-left corner
		int nWidthSrc = 0,      // width of source rectangle
		int nHeightSrc = 0      // height of source rectangle
	);

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
	);

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
		IMAGE* imgtexture,
		int nXOriginDest,
		int nYOriginDest,
		float centerx,
		float centery,
		float radian,
		int btransparent = 0,           // transparent (1) or not (0)
		int alpha = -1,                  // in range[0, 256], alpha== -1 means no alpha
		int smooth = 0
	);

	int putimage_rotatezoom(
		IMAGE* imgtexture,
		int nXOriginDest,
		int nYOriginDest,
		float centerx,
		float centery,
		float radian,
		float zoom,
		int btransparent = 0,           // transparent (1) or not (0)
		int alpha = -1,                  // in range[0, 256], alpha== -1 means no alpha
		int smooth = 0
	);
};


IMAGE*
CONVERT_IMAGE(IMAGE*);

IMAGE*
CONVERT_IMAGE_CONST(IMAGE*);

} // namespace ege;

#endif

