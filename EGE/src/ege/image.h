#ifndef Inc_ege_image_h_
#define Inc_ege_image_h_

#include "ege/gapi.h"
#include "ege/viewport.h"
#include <windows.h>

#define CONVERT_IMAGE(pimg) ( ((size_t)(pimg)<0x20 ?\
							   ((pimg) ?\
								(get_global_state().img_page[(size_t)(pimg) & 0xF])\
								: (--ege::update_mark_count, get_global_state().imgtarget))\
								   : pimg) )

#define CONVERT_IMAGE_CONST(pimg) ( (size_t)(pimg)<0x20 ?\
									((pimg) ?\
									 (get_global_state().img_page[(size_t)(pimg) & 0xF])\
									 : get_global_state().imgtarget)\
										: pimg)

namespace ege
{

// 定义图像对象
class IMAGE
{
	int m_initflag;

public:
	::HDC m_hDC = {};
	::HBITMAP  m_hBmp;
	int m_width;
	int m_height;
	PDWORD m_pBuffer;
	color_t m_color;
	color_t m_fillcolor;
	bool m_aa;

private:
	int  newimage(::HDC hdc, int width, int height);
	int  deleteimage();

public:
	viewporttype m_vpt;
	textsettingstype m_texttype;
	linestyletype m_linestyle;
	float m_linewidth;
	color_t m_bk_color;
	void* m_pattern_obj = {};
	int m_pattern_type;
	void* m_texture = {};

private:
	void inittest(const wchar_t* strCallFunction = {}) const;

public:
	IMAGE();
	IMAGE(int width, int height);
	IMAGE(IMAGE& img);              // 拷贝构造函数
	IMAGE& operator = (const IMAGE& img); // 赋值运算符重载函数
	~IMAGE();
	void set_pattern(void* obj, int type);
	void delete_pattern();
	void gentexture(bool gen);

	::HDC
	getdc() const
	{
		return m_hDC;
	}
	int
	getwidth() const
	{
		return m_width;
	}
	int
	getheight() const
	{
		return m_height;
	}
	PDWORD
	getbuffer() const
	{
		return m_pBuffer;
	}

	int
	createimage(int width, int height);
	int
	resize(int width, int height);
	void
	copyimage(IMAGE* pSrcImg);
	void
	getimage(int srcX, int srcY, int srcWidth, int srcHeight);
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
	getpngimg(FILE* fp);
	int
	savepngimg(FILE* fp, int bAlpha);
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


} // namespace ege;

#endif

