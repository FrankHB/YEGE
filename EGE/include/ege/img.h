#ifndef Inc_ege_img_h_
#define Inc_ege_img_h_

#include "ege/base.h"
#include "ege/color.h"

namespace ege
{

//绘图环境设置
IMAGE* gettarget();
int settarget(IMAGE* pbuf); //用 nullptr 设置窗口为绘图目标

EGEAPI void
cleardevice(IMAGE* pimg = nullptr);                   //清屏


//绘图属性
EGEAPI void
getlinestyle(int* plinestyle, unsigned short* pupattern = nullptr,
	int* pthickness = nullptr, IMAGE* pimg = nullptr); //获取当前线形

EGEAPI void
setlinestyle(int linestyle, unsigned short upattern = 0, int thickness = 1,
	IMAGE* pimg = nullptr); //设置当前线形

EGEAPI void
setlinewidth(float width, IMAGE* pimg = nullptr); //设置当前线宽

EGEAPI void
setfillstyle(int pattern, color_t color, IMAGE* pimg = nullptr); //设置填充类型

EGEAPI void
setwritemode(int mode, IMAGE* pimg = nullptr); //设置绘图位操作模式

//色彩函数
EGEAPI color_t
getcolor(IMAGE* pimg = nullptr); //获取当前绘图前景色

EGEAPI color_t
getfillcolor(IMAGE* pimg = nullptr); //获取当前绘图填充色

EGEAPI color_t
getbkcolor(IMAGE* pimg = nullptr); //获取当前绘图背景色

EGEAPI void
setcolor(color_t color, IMAGE* pimg = nullptr); //设置当前绘图前景色

EGEAPI void
setfillcolor(color_t color, IMAGE* pimg = nullptr); //设置当前绘图填充色

//设置当前绘图背景色（设置并做背景色像素替换）
EGEAPI void
setbkcolor(color_t color, IMAGE* pimg = nullptr);

//快速设置当前绘图背景色（只设置不绘画）
EGEAPI void
setbkcolor_f(color_t color, IMAGE* pimg = nullptr);

//设置当前文字背景色
EGEAPI void
setfontbkcolor(color_t color, IMAGE* pimg = nullptr);

//设置背景混合模式(0=OPAQUE, 1=TRANSPARENT)
EGEAPI void
setbkmode(int iBkMode, IMAGE* pimg = nullptr);


//基本绘图函数

EGEAPI color_t
getpixel(int x, int y, IMAGE* pimg = nullptr); //获取点的颜色

EGEAPI void
putpixel(int x, int y, color_t color, IMAGE* pimg = nullptr); //画点

EGEAPI color_t
getpixel_f(int x, int y, IMAGE* pimg = nullptr); //获取点的颜色

EGEAPI void
putpixel_f(int x, int y, color_t color, IMAGE* pimg = nullptr); //画点

EGEAPI void
putpixels(int nPoint, int* pPoints, IMAGE* pimg = nullptr); //批量画点

EGEAPI void
putpixels_f(int nPoint, int* pPoints, IMAGE* pimg = nullptr); //批量画点


EGEAPI void
moveto(int x, int y, IMAGE* pimg = nullptr); //移动当前点(绝对坐标)

EGEAPI void
moverel(int dx, int dy, IMAGE* pimg = nullptr); //移动当前点(相对坐标)


EGEAPI void
line(int x1, int y1, int x2, int y2, IMAGE* pimg = nullptr);      //画线

EGEAPI void
linerel(int dx, int dy, IMAGE* pimg = nullptr);                   //画线(至相对坐标)

EGEAPI void
lineto(int x, int y, IMAGE* pimg = nullptr);                      //画线(至绝对坐标)

EGEAPI void
line_f(float x1, float y1, float x2, float y2, IMAGE* pimg = nullptr);  //画线

EGEAPI void
linerel_f(float dx, float dy, IMAGE* pimg = nullptr); //画线(至相对坐标)

EGEAPI void
lineto_f(float x, float y, IMAGE* pimg = nullptr); //画线(至绝对坐标)

//画矩形
EGEAPI void
rectangle(int left, int top, int right, int bottom, IMAGE* pimg = nullptr);


//画圆弧
EGEAPI void
arc(int x, int y, int stangle, int endangle, int radius, IMAGE* pimg = nullptr);

//画圆
EGEAPI void
circle(int x, int y, int radius, IMAGE* pimg = nullptr);

//画填充圆扇形
EGEAPI void
pieslice(int x, int y, int stangle, int endangle, int radius,
	IMAGE* pimg = nullptr);

//画椭圆弧线
EGEAPI void
ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg = nullptr);

//画填充椭圆
EGEAPI void
fillellipse(int x, int y, int xradius, int yradius, IMAGE* pimg = nullptr);

//画填充椭圆扇形
EGEAPI void
sector(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg = nullptr);


//画圆弧
EGEAPI void
arcf(float x, float y, float stangle, float endangle, float radius,
	IMAGE* pimg = nullptr);

//画圆
EGEAPI void
circlef(float x, float y, float radius, IMAGE* pimg = nullptr);

//画填充圆扇形
EGEAPI void
pieslicef(float x, float y, float stangle, float endangle, float radius,
	IMAGE* pimg = nullptr);

//画椭圆弧线
EGEAPI void
ellipsef(float x, float y, float stangle, float endangle, float xradius,
	float yradius, IMAGE* pimg = nullptr);

//画填充椭圆
EGEAPI void
fillellipsef(float x, float y, float xradius, float yradius,
	IMAGE* pimg = nullptr);

//画填充椭圆扇形
EGEAPI void
sectorf(float x, float y, float stangle, float endangle, float xradius,
	float yradius, IMAGE* pimg = nullptr);


//画无边框填充矩形
EGEAPI void
bar(int left, int top, int right, int bottom, IMAGE* pimg = nullptr);

//画有边框三维填充矩形
EGEAPI void
bar3d(int left, int top, int right, int bottom, int depth, int topflag,
	IMAGE* pimg = nullptr);


//画多边形
EGEAPI void
drawpoly(int numpoints, const int* polypoints, IMAGE* pimg = nullptr);

//画多条不连续线（扩展函数）
EGEAPI void
drawlines(int numlines, const int* polypoints, IMAGE* pimg = nullptr);

//画bezier曲线（扩展函数）
EGEAPI void
drawbezier(int numpoints, const int* polypoints, IMAGE* pimg = nullptr);

//画填充的多边形
EGEAPI void
fillpoly(int numpoints, const int* polypoints, IMAGE* pimg = nullptr);

//画渐变填充的多边形
EGEAPI void
fillpoly_gradient(int numpoints, const ege_colpoint* polypoints,
	IMAGE* pimg = nullptr);

//按边界颜色填充区域
EGEAPI void
floodfill(int x, int y, int border, IMAGE* pimg = nullptr);

//按起始点颜色填充区域
EGEAPI void
floodfillsurface(int x, int y, color_t areacolor, IMAGE* pimg = nullptr);


//高级绘图函数（带AA）
//ege new_api
EGEAPI void
ege_enable_aa(bool enable, IMAGE* pimg = nullptr);


EGEAPI void
ege_line(float x1, float y1, float x2, float y2, IMAGE* pimg = nullptr);

EGEAPI void
ege_drawpoly(int numpoints, ege_point* polypoints, IMAGE* pimg = nullptr);

EGEAPI void
ege_drawcurve(int numpoints, ege_point* polypoints, IMAGE* pimg = nullptr);

EGEAPI void
ege_rectangle(float x, float y, float w, float h, IMAGE* pimg = nullptr);

EGEAPI void
ege_ellipse(float x, float y, float w, float h, IMAGE* pimg = nullptr);

EGEAPI void
ege_pie(float x, float y, float w, float h, float stangle, float sweepAngle,
	IMAGE* pimg = nullptr);


EGEAPI void
ege_arc(float x, float y, float w, float h, float stangle, float sweepAngle,
	IMAGE* pimg = nullptr);

EGEAPI void
ege_bezier(int numpoints, ege_point* polypoints, IMAGE* pimg = nullptr);


EGEAPI void
ege_fillpoly(int numpoints, ege_point* polypoints, IMAGE* pimg = nullptr);

EGEAPI void
ege_fillrect(float x, float y, float w, float h, IMAGE* pimg = nullptr);

EGEAPI void
ege_fillellipse(float x, float y, float w, float h, IMAGE* pimg = nullptr);

EGEAPI void
ege_fillpie(float x, float y, float w, float h, float stangle, float sweepAngle,
	IMAGE* pimg = nullptr);


EGEAPI void
ege_setpattern_none(IMAGE* pimg = nullptr);

EGEAPI void
ege_setpattern_lineargradient(float x1, float y1, color_t c1, float x2, float y2, color_t c2, IMAGE* pimg = nullptr);

EGEAPI void
ege_setpattern_pathgradient(ege_point center, color_t centercolor,

										int count, ege_point* points, int colcount, color_t* pointscolor, IMAGE* pimg = nullptr);
EGEAPI void
ege_setpattern_ellipsegradient(ege_point center, color_t centercolor,
		float x, float y, float w, float h, color_t color,
		IMAGE* pimg = nullptr);

EGEAPI void
ege_setpattern_texture(IMAGE* srcimg, float x, float y, float w, float h,
	IMAGE* pimg = nullptr);


EGEAPI void
ege_setalpha(int alpha, IMAGE* pimg = nullptr);

EGEAPI void
ege_gentexture(bool gen, IMAGE* pimg = nullptr);

EGEAPI void
ege_puttexture(IMAGE* srcimg, float x, float y, float w, float h,
	IMAGE* pimg = nullptr);

EGEAPI void
ege_puttexture(IMAGE* srcimg, ege_rect dest, IMAGE* pimg = nullptr);

EGEAPI void
ege_puttexture(IMAGE* srcimg, ege_rect dest, ege_rect src,
	IMAGE* pimg = nullptr);

//

//图片函数
#define getmaxx getwidth
#define getmaxy getheight

EGEAPI int
getwidth(IMAGE* pimg = nullptr);           //获取图片宽度

EGEAPI int
getheight(IMAGE* pimg = nullptr);          //获取图片高度

EGEAPI int
getx(IMAGE* pimg = nullptr);               //获取当前 x 坐标

EGEAPI int
gety(IMAGE* pimg = nullptr);               //获取当前 y 坐标


IMAGE* EGEAPI
newimage(); //创建PIMAGE

IMAGE* EGEAPI
newimage(int width, int height); //创建PIMAGE

EGEAPI void
delimage(IMAGE* pImg); //创建PIMAGE

void* EGEAPI
getbuffer(IMAGE* pImg);


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

