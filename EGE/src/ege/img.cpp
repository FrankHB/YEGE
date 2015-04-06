#include "ege/img.h"
#include "image.h"
#include "global.h"

namespace ege
{

namespace
{

::POINT
getxy(IMAGE* pimg)
{
	::POINT pt;

	::GetCurrentPositionEx(cimg_ref_c(pimg).getdc(), &pt);
	return pt;
}

} // unnamed namespace;

int
getwidth(IMAGE* pimg)
{
	return cimg_ref_c(pimg).GetWidth();
}

int
getheight(IMAGE* pimg)
{
	return cimg_ref_c(pimg).GetHeight();
}

int
getx(IMAGE* pimg)
{
	return getxy(pimg).x;
}

int
gety(IMAGE* pimg)
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
delimage(IMAGE* pimg)
{
	delete pimg;
}

void*
getbuffer(IMAGE* pimg)
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
getimage(IMAGE* pDstImg, IMAGE* pSrcImg, int srcX, int srcY, int srcWidth,
	int srcHeight)
{
	Deref(pDstImg).getimage(pSrcImg, srcX, srcY, srcWidth, srcHeight);
}


int
putimage_transparent(
	IMAGE* imgdest,         // handle to dest
	IMAGE* imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	return Deref(imgsrc).putimage_transparent(imgdest, nXOriginDest, nYOriginDest,
		crTransparent, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_alphablend(
	IMAGE* imgdest,         // handle to dest
	IMAGE* imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	yassume(imgsrc);

	return imgsrc->putimage_alphablend(imgdest, nXOriginDest, nYOriginDest,
		alpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
putimage_alphatransparent(
	IMAGE* imgdest,         // handle to dest
	IMAGE* imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent, // color to make transparent
	unsigned char alpha,    // alpha
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	return Deref(imgsrc).putimage_alphatransparent(imgdest, nXOriginDest,
		nYOriginDest, crTransparent, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc,
		nHeightSrc);
}

int
putimage_withalpha(
	IMAGE* imgdest,         // handle to dest
	IMAGE* imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	int nXOriginSrc,        // x-coord of source upper-left corner
	int nYOriginSrc,        // y-coord of source upper-left corner
	int nWidthSrc,          // width of source rectangle
	int nHeightSrc          // height of source rectangle
)
{
	return Deref(imgsrc).putimage_withalpha(imgdest, nXOriginDest, nYOriginDest,
		nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

int
imagefilter_blurring(
	IMAGE* imgdest,         // handle to dest
	int intensity,
	int alpha,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest
)
{
	return cimg_ref(imgdest).imagefilter_blurring(intensity, alpha,
		nXOriginDest, nYOriginDest, nWidthDest, nHeightDest);
}

} // namespace ege;

