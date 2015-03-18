#ifndef Inc_ege_image_h_
#define Inc_ege_image_h_

#include "ege/gapi.h"
#include "ege/viewport.h"
#include "global.h"
#include <memory> // for std::unique_ptr;
#include <windows.h>
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>

namespace ege
{

// 定义图像对象
class IMAGE
{
private:
	::HDC m_hDC;
	::HBITMAP m_hBmp;
	int m_width;
	int m_height;
	unsigned long* m_pBuffer;

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
	IMAGE(::HDC, int, int);
	IMAGE(const IMAGE&);
	IMAGE(IMAGE&&) ynothrow;
	~IMAGE();

	IMAGE&
	operator=(IMAGE) ynothrow;

	void
	swap(IMAGE&) ynothrow;

	void
	gentexture(bool gen);

	::HDC
	getdc() const
	{
		return m_hDC;
	}

	int
	GetWidth() const
	{
		return m_width;
	}

	int
	GetHeight() const
	{
		return m_height;
	}

	unsigned long*
	getbuffer() const
	{
		return m_pBuffer;
	}

	int
	Resize(int, int);

	void
	getimage(IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight);
	int
	getimage(const char* pImgFile, int zoomWidth = 0, int zoomHeight = 0);
	int
	getimage(const wchar_t* pImgFile, int zoomWidth = 0, int zoomHeight = 0);
	int
	getimage(const char* pResType, const char* pResName, int zoomWidth = 0, int zoomHeight = 0);
	int
	getimage(const wchar_t* pResType, const wchar_t* pResName, int zoomWidth = 0, int zoomHeight = 0);
	int
	getimage(void* pMem, long size);

	void
	putimage(int, int, unsigned long = SRCCOPY) const;
	void
	putimage(int, int, int, int, int, int, unsigned long = SRCCOPY) const;
	void
	putimage(IMAGE*, int, int, unsigned long = SRCCOPY) const;
	void
	putimage(IMAGE*, int, int, int, int, int, int, unsigned long = SRCCOPY) const;
	void
	putimage(IMAGE*, int, int, int, int, int, int, int, int, unsigned long = SRCCOPY)
		const;

	int
	saveimage(const char* filename);
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
		int btransparent = 0, // transparent (1) or not (0)
		int alpha = -1, // in range[0, 256], alpha== -1 means no alpha
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
		int btransparent = 0, // transparent (1) or not (0)
		int alpha = -1, // in range[0, 256], alpha== -1 means no alpha
		int smooth = 0
	);
};


IMAGE*
CONVERT_IMAGE(IMAGE*);

IMAGE*
CONVERT_IMAGE_CONST(IMAGE*);

inline IMAGE&
convert_image_ref(IMAGE* pimg)
{
	return Deref(CONVERT_IMAGE(pimg));
}

inline IMAGE&
convert_image_ref_c(IMAGE* pimg)
{
	return Deref(CONVERT_IMAGE_CONST(pimg));
}

} // namespace ege;

#endif

