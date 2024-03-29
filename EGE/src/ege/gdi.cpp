﻿#include "ege/gdi.h"
#include "image.h"
#include "global.h"
#include <cstdio>

namespace ege
{

namespace
{

int
getimage_common(IMAGE* pimg, std::FILE* fp) ynothrow
{
	if(fp)
	{
		const auto ret(Deref(pimg).getpngimg(fp));

		std::fclose(fp);
		return ret;
	}
	return grFileNotFound;
}

} // unnamed namespace;

int
getimage(IMAGE* pDstImg, const char* pImgFile, int, int)
{
	return Deref(pDstImg).getimage(pImgFile);
}
int
getimage(IMAGE* pDstImg, const wchar_t* pImgFile, int, int)
{
	return Deref(pDstImg).getimage(pImgFile);
}
int
getimage(IMAGE* pDstImg, const char* pResType, const char* pResName, int, int)
{
	return Deref(pDstImg).getimage(pResType, pResName);
}
int
getimage(IMAGE* pDstImg, const wchar_t* pResType, const wchar_t* pResName, int,
	int)
{
	return Deref(pDstImg).getimage(pResType, pResName);
}

void
putimage(int dstX, int dstY, const IMAGE* pSrcImg, unsigned long dwRop)
{
	Deref(pSrcImg).putimage(dstX, dstY, dwRop);
}
void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, const IMAGE* pSrcImg,
	int srcX, int srcY, unsigned long dwRop)
{
	Deref(pSrcImg).putimage(dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, const IMAGE* pSrcImg,
	unsigned long dwRop)
{
	Deref(pSrcImg).putimage(pDstImg, dstX, dstY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	const IMAGE* pSrcImg, int srcX, int srcY, unsigned long dwRop)
{
	Deref(pSrcImg).putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX,
		srcY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	const IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight,
	unsigned long dwRop)
{
	Deref(pSrcImg).putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX,
		srcY, srcWidth, srcHeight, dwRop);
}
void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, const IMAGE* pSrcImg,
	int srcX, int srcY, int srcWidth, int srcHeight, unsigned long dwRop)
{
	Deref(pSrcImg).putimage({}, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		srcWidth, srcHeight, dwRop);
}

int
saveimage(const IMAGE* pimg, const char* filename)
{
	return cimg_ref(pimg).saveimage(filename);
}
int
saveimage(const IMAGE* pimg, const wchar_t* filename)
{
	return cimg_ref(pimg).saveimage(filename);
}


int
savepng(const IMAGE* pimg, const char* filename, int)
{
#if YEGE_Use_YSLib
	return cimg_ref(pimg).saveimage(filename, ImageFormat::PNG);
#else
	return cimg_ref(pimg).saveimage(filename);
#endif
}
int
savepng(const IMAGE* pimg, const wchar_t* filename, int)
{
#if YEGE_Use_YSLib
	return cimg_ref(pimg).saveimage(filename, ImageFormat::PNG);
#else
	return cimg_ref(pimg).saveimage(filename);
#endif
}

int
getimage_pngfile(IMAGE* pimg, const char* filename)
{
	return getimage_common(pimg, std::fopen(filename, "rb"));
}
int
getimage_pngfile(IMAGE* pimg, const wchar_t* filename)
{
	return getimage_common(pimg, ::_wfopen(filename, L"rb"));
}

} // namespace ege;
