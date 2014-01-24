#include "ege/gdi.h"
#include "image.h"
#include "global.h"
#include <cstdio>

namespace ege
{

int
getimage(IMAGE* pDstImg, const char* pImgFile, int zoomWidth, int zoomHeight)
{
	return pDstImg->getimage(pImgFile, zoomWidth, zoomHeight);
}
int
getimage(IMAGE* pDstImg, const wchar_t* pImgFile, int zoomWidth, int zoomHeight)
{
	return pDstImg->getimage(pImgFile, zoomWidth, zoomHeight);
}
int
getimage(IMAGE* pDstImg, const char* pResType, const char* pResName,
	int zoomWidth, int zoomHeight)
{
	return pDstImg->getimage(pResType, pResName, zoomWidth, zoomHeight);
}
int
getimage(IMAGE* pDstImg, const wchar_t* pResType, const wchar_t* pResName,
	int zoomWidth, int zoomHeight)
{
	return pDstImg->getimage(pResType, pResName, zoomWidth, zoomHeight);
}

void
putimage(int dstX, int dstY, IMAGE* pSrcImg, ::DWORD dwRop)
{
	pSrcImg->putimage(dstX, dstY, dwRop);
}
void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg,
	int srcX, int srcY, ::DWORD dwRop)
{
	pSrcImg->putimage(dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, IMAGE* pSrcImg, ::DWORD dwRop)
{
	pSrcImg->putimage(pDstImg, dstX, dstY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	IMAGE* pSrcImg, int srcX, int srcY, ::DWORD dwRop)
{
	pSrcImg->putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop)
{
	pSrcImg->putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		srcWidth, srcHeight, dwRop);
}
void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg,
	int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop)
{
	pSrcImg->putimage({}, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		srcWidth, srcHeight, dwRop);
}


int
saveimage(IMAGE* pimg, const char* filename)
{
	const auto img = CONVERT_IMAGE(pimg);

	int ret = 0;

	if(img)
		ret = img->saveimage(filename);
	return ret;
}

int
saveimage(IMAGE* pimg, const wchar_t* filename)
{
	const auto img = CONVERT_IMAGE(pimg);
	int ret = 0;

	if(img)
		ret = img->saveimage(filename);
	return ret;
}


int
savepng(IMAGE* pimg, const char* filename, int bAlpha)
{
	FILE* fp = fopen(filename, "wb");

	if(!fp)
		return grFileNotFound;

	int ret = pimg->savepngimg(fp, bAlpha);

	fclose(fp);
	return ret;
}
int
savepng(IMAGE* pimg, const wchar_t* filename, int bAlpha)
{
	FILE* fp = _wfopen(filename, L"wb");

	if(!fp)
		return grFileNotFound;

	int ret = pimg->savepngimg(fp, bAlpha);

	fclose(fp);
	return ret;
}

int
getimage_pngfile(IMAGE* pimg, const char* filename)
{
	FILE* fp = fopen(filename, "rb");

	if(!fp)
		return grFileNotFound;

	int ret = pimg->getpngimg(fp);

	fclose(fp);
	return ret;
}
int
getimage_pngfile(IMAGE* pimg, const wchar_t* filename)
{
	FILE* fp = _wfopen(filename, L"rb");
	if(!fp)
		return grFileNotFound;

	int ret = pimg->getpngimg(fp);

	fclose(fp);
	return ret;
}

} // namespace ege;
