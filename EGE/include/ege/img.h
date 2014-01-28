#ifndef Inc_ege_img_h_
#define Inc_ege_img_h_

#include "ege/base.h"
#include "ege/color.h"

namespace ege
{

//图片函数
#define getmaxx getwidth
#define getmaxy getheight

EGEAPI int
getwidth(IMAGE* pimg = {});           //获取图片宽度

EGEAPI int
getheight(IMAGE* pimg = {});          //获取图片高度

EGEAPI int
getx(IMAGE* pimg = {});               //获取当前 x 坐标

EGEAPI int
gety(IMAGE* pimg = {});               //获取当前 y 坐标


IMAGE* EGEAPI
newimage(); //创建PIMAGE
IMAGE* EGEAPI
newimage(int width, int height); //创建PIMAGE

EGEAPI void
delimage(IMAGE* pimg); //删除PIMAGE

void* EGEAPI
getbuffer(IMAGE* pimg);


EGEAPI int
resize(IMAGE* pDstImg, int width, int height); //重设尺寸

EGEAPI void
getimage(IMAGE* pDstImg, int srcX, int srcY, int srcWidth, int srcHeight);             //从屏幕获取图像
EGEAPI void
getimage(IMAGE* pDstImg, IMAGE* pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight);   //从另一个 IMAGE* 对象中获取图像


EGEAPI int
putimage_transparent(
	IMAGE* imgdest,         //handle to dest
	IMAGE* imgsrc,          //handle to source
	int nXOriginDest,       //x-coord of destination upper-left corner
	int nYOriginDest,       //y-coord of destination upper-left corner
	color_t crTransparent,  //color to make transparent
	int nXOriginSrc = 0,    //x-coord of source upper-left corner
	int nYOriginSrc = 0,    //y-coord of source upper-left corner
	int nWidthSrc = 0,      //width of source rectangle
	int nHeightSrc = 0      //height of source rectangle
);

EGEAPI int
putimage_alphablend(
	IMAGE* imgdest,         //handle to dest
	IMAGE* imgsrc,          //handle to source
	int nXOriginDest,       //x-coord of destination upper-left corner
	int nYOriginDest,       //y-coord of destination upper-left corner
	unsigned char alpha,    //alpha
	int nXOriginSrc = 0,    //x-coord of source upper-left corner
	int nYOriginSrc = 0,    //y-coord of source upper-left corner
	int nWidthSrc = 0,      //width of source rectangle
	int nHeightSrc = 0      //height of source rectangle
);

EGEAPI int
putimage_alphatransparent(
	IMAGE* imgdest,         //handle to dest
	IMAGE* imgsrc,          //handle to source
	int nXOriginDest,       //x-coord of destination upper-left corner
	int nYOriginDest,       //y-coord of destination upper-left corner
	color_t crTransparent,  //color to make transparent
	unsigned char alpha,    //alpha
	int nXOriginSrc = 0,    //x-coord of source upper-left corner
	int nYOriginSrc = 0,    //y-coord of source upper-left corner
	int nWidthSrc = 0,      //width of source rectangle
	int nHeightSrc = 0      //height of source rectangle
);

EGEAPI int
putimage_withalpha(
	IMAGE* imgdest,         //handle to dest
	IMAGE* imgsrc,          //handle to source
	int nXOriginDest,       //x-coord of destination upper-left corner
	int nYOriginDest,       //y-coord of destination upper-left corner
	int nXOriginSrc = 0,    //x-coord of source upper-left corner
	int nYOriginSrc = 0,    //y-coord of source upper-left corner
	int nWidthSrc = 0,      //width of source rectangle
	int nHeightSrc = 0      //height of source rectangle
);

EGEAPI int
imagefilter_blurring(
	IMAGE* imgdest,         //handle to dest
	int intensity,
	int alpha,
	int nXOriginDest = 0,
	int nYOriginDest = 0,
	int nWidthDest = 0,
	int nHeightDest = 0
);

EGEAPI int
putimage_rotate(
	IMAGE* imgdest,
	IMAGE* imgtexture,
	int nXOriginDest,
	int nYOriginDest,
	float centerx,
	float centery,
	float radian,
	int btransparent = 0, //transparent (1) or not (0)
	int alpha = -1, //in range[0, 256], alpha== -1 means no alpha
	int smooth = 0
);

EGEAPI int
putimage_rotatezoom(
	IMAGE* imgdest,
	IMAGE* imgtexture,
	int nXOriginDest,
	int nYOriginDest,
	float centerx,
	float centery,
	float radian,
	float zoom,
	int btransparent = 0, //transparent (1) or not (0)
	int alpha = -1, //in range[0, 256], alpha== -1 means no alpha
	int smooth = 0
);

}

#endif

