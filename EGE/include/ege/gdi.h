#ifndef Inc_ege_gdi_h_
#define Inc_ege_gdi_h_

#include "ege/img.h"
#include <windows.h>

namespace ege
{

EGEAPI int
getimage(IMAGE* pDstImg, const char*  pImgFile, int zoomWidth = 0, int zoomHeight = 0); // 从图片文件获取图像(bmp/jpg/gif/emf/wmf)

EGEAPI int
getimage(IMAGE* pDstImg, const wchar_t* pImgFile, int zoomWidth = 0, int zoomHeight = 0); // 从图片文件获取图像(bmp/jpg/gif/emf/wmf)

EGEAPI int
getimage(IMAGE* pDstImg, const char*  pResType, const char* pResName, int zoomWidth = 0, int zoomHeight = 0);   // 从资源文件获取图像(bmp/jpg/gif/emf/wmf)

EGEAPI int
getimage(IMAGE* pDstImg, const wchar_t* pResType, const wchar_t* pResName, int zoomWidth = 0, int zoomHeight = 0);   // 从资源文件获取图像(bmp/jpg/gif/emf/wmf)

EGEAPI void
putimage(int dstX, int dstY, IMAGE* pSrcImg, ::DWORD dwRop = SRCCOPY); // 绘制图像到屏幕

EGEAPI void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg, int srcX, int srcY, ::DWORD dwRop = SRCCOPY);    // 绘制图像到屏幕(指定宽高)

EGEAPI void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop = SRCCOPY);   // 绘制图像到屏幕(指定源宽高和目标宽高进行拉伸)

EGEAPI void
putimage(IMAGE* pDstImg, int dstX, int dstY, IMAGE* pSrcImg, ::DWORD dwRop = SRCCOPY); // 绘制图像到另一图像中

EGEAPI void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg, int srcX, int srcY, ::DWORD dwRop = SRCCOPY);    // 绘制图像到另一图像中(指定宽高)

EGEAPI void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop = SRCCOPY);   // 绘制图像到另一图像中(指定源宽高和目标宽高进行拉伸)

EGEAPI int
saveimage(IMAGE* pimg, const char*  filename);

EGEAPI int
saveimage(IMAGE* pimg, const wchar_t* filename);

EGEAPI int
savepng(IMAGE* pimg, const char*  filename, int bAlpha = 0);

EGEAPI int
savepng(IMAGE* pimg, const wchar_t* filename, int bAlpha = 0);

EGEAPI int
getimage_pngfile(IMAGE* pimg, const char*  filename);

EGEAPI int
getimage_pngfile(IMAGE* pimg, const wchar_t* filename);

}

#endif

