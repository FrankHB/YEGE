#pragma once

#include "e_img.h"
#include <windows.h>

namespace ege
{

int  EGEAPI getimage(PIMAGE pDstImg, LPCSTR  pImgFile, int zoomWidth = 0, int zoomHeight = 0);     // 从图片文件获取图像(bmp/jpg/gif/emf/wmf)
int  EGEAPI getimage(PIMAGE pDstImg, LPCWSTR pImgFile, int zoomWidth = 0, int zoomHeight = 0);     // 从图片文件获取图像(bmp/jpg/gif/emf/wmf)
int  EGEAPI getimage(PIMAGE pDstImg, LPCSTR  pResType, LPCSTR  pResName, int zoomWidth = 0, int zoomHeight = 0);   // 从资源文件获取图像(bmp/jpg/gif/emf/wmf)
int  EGEAPI getimage(PIMAGE pDstImg, LPCWSTR pResType, LPCWSTR pResName, int zoomWidth = 0, int zoomHeight = 0);   // 从资源文件获取图像(bmp/jpg/gif/emf/wmf)
void EGEAPI putimage(int dstX, int dstY, const PIMAGE pSrcImg, DWORD dwRop = SRCCOPY);                         // 绘制图像到屏幕
void EGEAPI putimage(int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);    // 绘制图像到屏幕(指定宽高)
void EGEAPI putimage(int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop = SRCCOPY);   // 绘制图像到屏幕(指定源宽高和目标宽高进行拉伸)
void EGEAPI putimage(PIMAGE pDstImg, int dstX, int dstY, const PIMAGE pSrcImg, DWORD dwRop = SRCCOPY);         // 绘制图像到另一图像中
void EGEAPI putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);    // 绘制图像到另一图像中(指定宽高)
void EGEAPI putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop = SRCCOPY);   // 绘制图像到另一图像中(指定源宽高和目标宽高进行拉伸)
int  EGEAPI saveimage(PIMAGE pimg, LPCSTR  filename);
int  EGEAPI saveimage(PIMAGE pimg, LPCWSTR filename);
int  EGEAPI savepng(PIMAGE pimg, LPCSTR  filename, int bAlpha = 0);
int  EGEAPI savepng(PIMAGE pimg, LPCWSTR filename, int bAlpha = 0);
int  EGEAPI getimage_pngfile(PIMAGE pimg, LPCSTR  filename);
int  EGEAPI getimage_pngfile(PIMAGE pimg, LPCWSTR filename);

}

