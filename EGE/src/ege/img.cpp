#include "ege/img.h"
#include "image.h"
#include "global.h"

namespace ege
{

int
getwidth(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	CONVERT_IMAGE_END;
	if(img)
		return img->m_width;
	return 0;
}

int
getheight(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	CONVERT_IMAGE_END;
	if(img)
		return img->m_height;
	return 0;
}

int
getx(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
	{
		POINT pt;
		GetCurrentPositionEx(img->m_hDC, &pt);
		CONVERT_IMAGE_END;
		return pt.x;
	}
	CONVERT_IMAGE_END;
	return -1;
}

int
gety(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
	{
		POINT pt;
		GetCurrentPositionEx(img->m_hDC, &pt);
		CONVERT_IMAGE_END;
		return pt.y;
	}
	CONVERT_IMAGE_END;
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
	if(width  < 1) width  = 1;
	if(height < 1) height = 1;
	return new IMAGE(width, height);
}

void
delimage(IMAGE* pImg)
{
	delete pImg;
}

void*
getbuffer(IMAGE* pImg)
{
	const auto img = CONVERT_IMAGE_CONST(pImg);
	CONVERT_IMAGE_END;

	return img->getbuffer();
}



int
resize(IMAGE* pDstImg, int width, int height)
{
	return pDstImg->resize(width, height);
}

void
getimage(IMAGE* pDstImg, int srcX, int srcY, int srcWidth, int srcHeight)
{
	pDstImg->getimage(srcX, srcY, srcWidth, srcHeight);
}
void
getimage(IMAGE* pDstImg, IMAGE* pSrcImg, int srcX, int srcY, int srcWidth,
	int srcHeight)
{
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
	return imgsrc->putimage_transparent(imgdest, nXOriginDest, nYOriginDest, crTransparent, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
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
	return imgsrc->putimage_alphablend(imgdest, nXOriginDest, nYOriginDest, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
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
	return imgsrc->putimage_alphatransparent(imgdest, nXOriginDest, nYOriginDest, crTransparent, alpha, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
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
	return imgsrc->putimage_withalpha(imgdest, nXOriginDest, nYOriginDest, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
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
	const auto img = CONVERT_IMAGE(imgdest);
	int ret = 0;
	if(img)
	{
		ret = img->imagefilter_blurring(intensity, alpha, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest);
	}
	CONVERT_IMAGE_END;
	return ret;
}

} // namespace ege;

