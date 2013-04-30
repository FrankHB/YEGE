#pragma once

#include "e_img.h"
#include <windows.h>

namespace ege
{

int  EGEAPI getimage(PIMAGE pDstImg, LPCSTR  pImgFile, int zoomWidth = 0, int zoomHeight = 0);     // ��ͼƬ�ļ���ȡͼ��(bmp/jpg/gif/emf/wmf)
int  EGEAPI getimage(PIMAGE pDstImg, LPCWSTR pImgFile, int zoomWidth = 0, int zoomHeight = 0);     // ��ͼƬ�ļ���ȡͼ��(bmp/jpg/gif/emf/wmf)
int  EGEAPI getimage(PIMAGE pDstImg, LPCSTR  pResType, LPCSTR  pResName, int zoomWidth = 0, int zoomHeight = 0);   // ����Դ�ļ���ȡͼ��(bmp/jpg/gif/emf/wmf)
int  EGEAPI getimage(PIMAGE pDstImg, LPCWSTR pResType, LPCWSTR pResName, int zoomWidth = 0, int zoomHeight = 0);   // ����Դ�ļ���ȡͼ��(bmp/jpg/gif/emf/wmf)
void EGEAPI putimage(int dstX, int dstY, const PIMAGE pSrcImg, DWORD dwRop = SRCCOPY);                         // ����ͼ����Ļ
void EGEAPI putimage(int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);    // ����ͼ����Ļ(ָ�����)
void EGEAPI putimage(int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop = SRCCOPY);   // ����ͼ����Ļ(ָ��Դ��ߺ�Ŀ���߽�������)
void EGEAPI putimage(PIMAGE pDstImg, int dstX, int dstY, const PIMAGE pSrcImg, DWORD dwRop = SRCCOPY);         // ����ͼ����һͼ����
void EGEAPI putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, DWORD dwRop = SRCCOPY);    // ����ͼ����һͼ����(ָ�����)
void EGEAPI putimage(PIMAGE pDstImg, int dstX, int dstY, int dstWidth, int dstHeight, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, DWORD dwRop = SRCCOPY);   // ����ͼ����һͼ����(ָ��Դ��ߺ�Ŀ���߽�������)
int  EGEAPI saveimage(PIMAGE pimg, LPCSTR  filename);
int  EGEAPI saveimage(PIMAGE pimg, LPCWSTR filename);
int  EGEAPI savepng(PIMAGE pimg, LPCSTR  filename, int bAlpha = 0);
int  EGEAPI savepng(PIMAGE pimg, LPCWSTR filename, int bAlpha = 0);
int  EGEAPI getimage_pngfile(PIMAGE pimg, LPCSTR  filename);
int  EGEAPI getimage_pngfile(PIMAGE pimg, LPCWSTR filename);

}

