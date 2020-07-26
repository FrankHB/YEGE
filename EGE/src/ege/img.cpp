#include "ege/img.h"
#include "image.h"
#include "global.h"

namespace ege
{

namespace
{

::POINT
getxy(const IMAGE* pimg)
{
	::POINT pt;

	::GetCurrentPositionEx(cimg_ref_c(pimg).getdc(), &pt);
	return pt;
}

} // unnamed namespace;

int
getwidth(const IMAGE* pimg)
{
	return cimg_ref_c(pimg).GetWidth();
}

int
getheight(const IMAGE* pimg)
{
	return cimg_ref_c(pimg).GetHeight();
}

int
getx(const IMAGE* pimg)
{
	return getxy(pimg).x;
}

int
gety(const IMAGE* pimg)
{
	return getxy(pimg).y;
}

IMAGE*
newimage()
{
	return new IMAGE(1, 1);
}
IMAGE*
newimage(int width, int height)
{
	return new IMAGE(std::max(width, 1), std::max(height, 1));
}

void
delimage(const IMAGE* pimg)
{
	delete pimg;
}

void*
getbuffer(IMAGE* pimg)
{
	return cimg_ref_c(pimg).getbuffer();
}
const void*
getbuffer(const IMAGE* pimg)
{
	return cimg_ref_c(pimg).getbuffer();
}

int
resize(IMAGE* pDstImg, int width, int height)
{
	return Deref(pDstImg).Resize(width, height);
}

void
getimage(IMAGE* pDstImg, int srcX, int srcY, int srcWidth, int srcHeight)
{
	Deref(pDstImg).getimage(&get_pages().get_target_ref(), srcX, srcY, srcWidth,
		srcHeight);
}
void
getimage(IMAGE* pDstImg, const IMAGE* pSrcImg, int srcX, int srcY, int srcWidth,
	int srcHeight)
{
	Deref(pDstImg).getimage(pSrcImg, srcX, srcY, srcWidth, srcHeight);
}


int
putimage_transparent(IMAGE* imgdest, const IMAGE* imgsrc, int nXOriginDest,
	int nYOriginDest, color_t crTransparent, int nXOriginSrc, int nYOriginSrc,
	int nWidthSrc, int nHeightSrc)
{
	return Deref(imgsrc).putimage_transparent(imgdest, nXOriginDest,
		nYOriginDest, crTransparent, nXOriginSrc, nYOriginSrc, nWidthSrc,
		nHeightSrc);
}

int
putimage_alphablend(IMAGE* imgdest, const IMAGE* imgsrc, int nXOriginDest,
	int nYOriginDest, unsigned char alpha, int nXOriginSrc, int nYOriginSrc,
	int nWidthSrc, int nHeightSrc)
{
	return Deref(imgsrc).putimage_alphablend(imgdest, nXOriginDest,
		nYOriginDest, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_alphatransparent(IMAGE* imgdest, const IMAGE* imgsrc, int nXOriginDest,
	int nYOriginDest, color_t crTransparent, unsigned char alpha,
	int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc)
{
	return Deref(imgsrc).putimage_alphatransparent(imgdest, nXOriginDest,
		nYOriginDest, crTransparent, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc,
		nHeightSrc);
}

int
putimage_withalpha(IMAGE* imgdest, const IMAGE* imgsrc, int nXOriginDest,
	int nYOriginDest, int nXOriginSrc, int nYOriginSrc, int nWidthSrc,
	int nHeightSrc)
{
	return Deref(imgsrc).putimage_withalpha(imgdest, nXOriginDest, nYOriginDest,
		nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_alphafilter(IMAGE* imgdest, const IMAGE* imgsrc, int nXOriginDest,
	int nYOriginDest, IMAGE* imgalpha, int nXOriginSrc, int nYOriginSrc,
	int nWidthSrc, int nHeightSrc)
{
	return imgsrc->putimage_alphafilter(imgdest, nXOriginDest, nYOriginDest,
		imgalpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
imagefilter_blurring(IMAGE* imgdest, int intensity, int alpha, int nXOriginDest,
	int nYOriginDest, int nWidthDest, int nHeightDest)
{
	return cimg_ref(imgdest).imagefilter_blurring(intensity, alpha,
		nXOriginDest, nYOriginDest, nWidthDest, nHeightDest);
}

} // namespace ege;

