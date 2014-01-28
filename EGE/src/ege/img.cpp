#include "ege/img.h"
#include "image.h"
#include "global.h"

namespace ege
{

int
getwidth(IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE_CONST(pimg));

	return img ? img->getwidth() : 0;
}

int
getheight(IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE_CONST(pimg));

	return img ? img->getheight() : 0;
}

int
getx(IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::POINT pt;

		::GetCurrentPositionEx(img->getdc(), &pt);
		return pt.x;
	}
	return -1;
}

int
gety(IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::POINT pt;

		::GetCurrentPositionEx(img->getdc(), &pt);
		return pt.y;
	}
	return -1;
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
	const auto img(CONVERT_IMAGE_CONST(pimg));

	yassume(img);

	return img->getbuffer();
}



int
resize(IMAGE* pDstImg, int width, int height)
{
	yassume(pDstImg);

	return pDstImg->resize(width, height);
}

void
getimage(IMAGE* pDstImg, int srcX, int srcY, int srcWidth, int srcHeight)
{
	yassume(pDstImg);

	pDstImg->getimage(get_pages().imgtarget, srcX, srcY, srcWidth, srcHeight);
}
void
getimage(IMAGE* pDstImg, IMAGE* pSrcImg, int srcX, int srcY, int srcWidth,
	int srcHeight)
{
	yassume(pDstImg);

	pDstImg->getimage(pSrcImg, srcX, srcY, srcWidth, srcHeight);
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
	yassume(imgsrc);

	return imgsrc->putimage_transparent(imgdest, nXOriginDest, nYOriginDest,
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
	yassume(imgsrc);

	return imgsrc->putimage_alphatransparent(imgdest, nXOriginDest,
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
	yassume(imgsrc);

	return imgsrc->putimage_withalpha(imgdest, nXOriginDest, nYOriginDest,
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
	if(const auto img = CONVERT_IMAGE(imgdest))
	{
		return img->imagefilter_blurring(intensity, alpha, nXOriginDest,
			nYOriginDest, nWidthDest, nHeightDest);
	}
	return 0;
}

} // namespace ege;

