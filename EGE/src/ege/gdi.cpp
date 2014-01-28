#include "ege/gdi.h"
#include "image.h"
#include "global.h"
#include <cstdio>

namespace ege
{

int
getimage(IMAGE* pDstImg, const char* pImgFile, int zoomWidth, int zoomHeight)
{
	yassume(pDstImg);

	return pDstImg->getimage(pImgFile, zoomWidth, zoomHeight);
}
int
getimage(IMAGE* pDstImg, const wchar_t* pImgFile, int zoomWidth, int zoomHeight)
{
	yassume(pDstImg);

	return pDstImg->getimage(pImgFile, zoomWidth, zoomHeight);
}
int
getimage(IMAGE* pDstImg, const char* pResType, const char* pResName,
	int zoomWidth, int zoomHeight)
{
	yassume(pDstImg);

	return pDstImg->getimage(pResType, pResName, zoomWidth, zoomHeight);
}
int
getimage(IMAGE* pDstImg, const wchar_t* pResType, const wchar_t* pResName,
	int zoomWidth, int zoomHeight)
{
	yassume(pDstImg);

	return pDstImg->getimage(pResType, pResName, zoomWidth, zoomHeight);
}

void
putimage(int dstX, int dstY, IMAGE* pSrcImg, ::DWORD dwRop)
{
	yassume(pSrcImg);

	pSrcImg->putimage(dstX, dstY, dwRop);
}
void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg,
	int srcX, int srcY, ::DWORD dwRop)
{
	yassume(pSrcImg);

	pSrcImg->putimage(dstX, dstY, dstWidth, dstHeight, srcX, srcY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, IMAGE* pSrcImg, ::DWORD dwRop)
{
	yassume(pSrcImg);

	pSrcImg->putimage(pDstImg, dstX, dstY, dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	IMAGE* pSrcImg, int srcX, int srcY, ::DWORD dwRop)
{
	yassume(pSrcImg);

	pSrcImg->putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		dwRop);
}
void
putimage(IMAGE* pDstImg, int dstX, int dstY, int dstWidth, int dstHeight,
	IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight,
	::DWORD dwRop)
{
	yassume(pSrcImg);

	pSrcImg->putimage(pDstImg, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		srcWidth, srcHeight, dwRop);
}
void
putimage(int dstX, int dstY, int dstWidth, int dstHeight, IMAGE* pSrcImg,
	int srcX, int srcY, int srcWidth, int srcHeight, ::DWORD dwRop)
{
	yassume(pSrcImg);

	pSrcImg->putimage({}, dstX, dstY, dstWidth, dstHeight, srcX, srcY,
		srcWidth, srcHeight, dwRop);
}


int
saveimage(IMAGE* pimg, const char* filename)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		return img->saveimage(filename);
	return 0;
}

int
saveimage(IMAGE* pimg, const wchar_t* filename)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		return img->saveimage(filename);
	return 0;
}


int
savepng(IMAGE* pimg, const char* filename, int bAlpha)
{
	if(std::FILE* fp = std::fopen(filename, "wb"))
	{
		yconstraint(pimg);

		const int ret(pimg->savepngimg(fp, bAlpha));

		std::fclose(fp);
		return ret;
	}
	return grFileNotFound;

}
int
savepng(IMAGE* pimg, const wchar_t* filename, int bAlpha)
{
	if(std::FILE* fp = _wfopen(filename, L"wb"))
	{
		yconstraint(pimg);

		const int ret(pimg->savepngimg(fp, bAlpha));

		std::fclose(fp);
		return ret;
	}
	return grFileNotFound;
}

int
getimage_pngfile(IMAGE* pimg, const char* filename)
{
	if(std::FILE* fp = std::fopen(filename, "rb"))
	{
		yconstraint(pimg);

		const int ret(pimg->getpngimg(fp));

		std::fclose(fp);
		return ret;
	}
	return grFileNotFound;
}
int
getimage_pngfile(IMAGE* pimg, const wchar_t* filename)
{
	if(std::FILE* fp = _wfopen(filename, L"rb"))
	{
		yconstraint(pimg);

		const int ret(pimg->getpngimg(fp));

		std::fclose(fp);
		return ret;
	}
	return grFileNotFound;
}

} // namespace ege;
