#ifndef Inc_ege_gapi_h_
#define Inc_ege_gapi_h_

#include "ege/base.h"
#include "ege/color.h"

namespace ege
{

//绘图环境设置
IMAGE* gettarget();
int settarget(IMAGE* pbuf); //用 {} 设置窗口为绘图目标

EGEAPI void
cleardevice(IMAGE* pimg = {});                   //清屏


//绘图属性
EGEAPI void
getlinestyle(int* plinestyle, unsigned short* pupattern = {},
	int* pthickness = {}, IMAGE* pimg = {}); //获取当前线形

EGEAPI void
setlinestyle(int linestyle, unsigned short upattern = 0, int thickness = 1,
	IMAGE* pimg = {}); //设置当前线形

EGEAPI void
setlinewidth(float width, IMAGE* pimg = {}); //设置当前线宽

EGEAPI void
setfillstyle(int pattern, color_t color, IMAGE* pimg = {}); //设置填充类型

EGEAPI void
setwritemode(int mode, IMAGE* pimg = {}); //设置绘图位操作模式


//色彩函数
EGEAPI color_t
getcolor(IMAGE* pimg = {}); //获取当前绘图前景色

EGEAPI color_t
getfillcolor(IMAGE* pimg = {}); //获取当前绘图填充色

EGEAPI color_t
getbkcolor(IMAGE* pimg = {}); //获取当前绘图背景色


EGEAPI void
setcolor(color_t color, IMAGE* pimg = {}); //设置当前绘图前景色

EGEAPI void
setfillcolor(color_t color, IMAGE* pimg = {}); //设置当前绘图填充色

//设置当前绘图背景色（设置并做背景色像素替换）
EGEAPI void
setbkcolor(color_t color, IMAGE* pimg = {});

//快速设置当前绘图背景色（只设置不绘画）
EGEAPI void
setbkcolor_f(color_t color, IMAGE* pimg = {});

//设置当前文字背景色
EGEAPI void
setfontbkcolor(color_t color, IMAGE* pimg = {});

//设置背景混合模式(0=OPAQUE, 1=TRANSPARENT)
EGEAPI void
setbkmode(int iBkMode, IMAGE* pimg = {});


//基本绘图函数

EGEAPI color_t
getpixel(int x, int y, IMAGE* pimg = {}); //获取点的颜色

EGEAPI void
putpixel(int x, int y, color_t color, IMAGE* pimg = {}); //画点

EGEAPI color_t
getpixel_f(int x, int y, IMAGE* pimg = {}); //获取点的颜色

EGEAPI void
putpixel_f(int x, int y, color_t color, IMAGE* pimg = {}); //画点

EGEAPI void
putpixels(int nPoint, int* pPoints, IMAGE* pimg = {}); //批量画点

EGEAPI void
putpixels_f(int nPoint, int* pPoints, IMAGE* pimg = {}); //批量画点


EGEAPI void
moveto(int x, int y, IMAGE* pimg = {}); //移动当前点(绝对坐标)

EGEAPI void
moverel(int dx, int dy, IMAGE* pimg = {}); //移动当前点(相对坐标)


EGEAPI void
line(int x1, int y1, int x2, int y2, IMAGE* pimg = {});      //画线

EGEAPI void
linerel(int dx, int dy, IMAGE* pimg = {});                   //画线(至相对坐标)

EGEAPI void
lineto(int x, int y, IMAGE* pimg = {});                      //画线(至绝对坐标)

EGEAPI void
line_f(float x1, float y1, float x2, float y2, IMAGE* pimg = {});  //画线

EGEAPI void
linerel_f(float dx, float dy, IMAGE* pimg = {}); //画线(至相对坐标)

EGEAPI void
lineto_f(float x, float y, IMAGE* pimg = {}); //画线(至绝对坐标)


//画矩形
EGEAPI void
rectangle(int left, int top, int right, int bottom, IMAGE* pimg = {});


//画圆弧
EGEAPI void
arc(int x, int y, int stangle, int endangle, int radius, IMAGE* pimg = {});

//画圆
EGEAPI void
circle(int x, int y, int radius, IMAGE* pimg = {});

//画填充圆扇形
EGEAPI void
pieslice(int x, int y, int stangle, int endangle, int radius,
	IMAGE* pimg = {});

//画椭圆弧线
EGEAPI void
ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg = {});

//画填充椭圆
EGEAPI void
fillellipse(int x, int y, int xradius, int yradius, IMAGE* pimg = {});

//画填充椭圆扇形
EGEAPI void
sector(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg = {});


//画圆弧
EGEAPI void
arcf(float x, float y, float stangle, float endangle, float radius,
	IMAGE* pimg = {});

//画圆
EGEAPI void
circlef(float x, float y, float radius, IMAGE* pimg = {});

//画填充圆扇形
EGEAPI void
pieslicef(float x, float y, float stangle, float endangle, float radius,
	IMAGE* pimg = {});

//画椭圆弧线
EGEAPI void
ellipsef(float x, float y, float stangle, float endangle, float xradius,
	float yradius, IMAGE* pimg = {});

//画填充椭圆
EGEAPI void
fillellipsef(float x, float y, float xradius, float yradius,
	IMAGE* pimg = {});

//画填充椭圆扇形
EGEAPI void
sectorf(float x, float y, float stangle, float endangle, float xradius,
	float yradius, IMAGE* pimg = {});


//画无边框填充矩形
EGEAPI void
bar(int left, int top, int right, int bottom, IMAGE* pimg = {});

//画有边框三维填充矩形
EGEAPI void
bar3d(int left, int top, int right, int bottom, int depth, int topflag,
	IMAGE* pimg = {});


//画多边形
EGEAPI void
drawpoly(int numpoints, const int* polypoints, IMAGE* pimg = {});

//画多条不连续线（扩展函数）
EGEAPI void
drawlines(int numlines, const int* polypoints, IMAGE* pimg = {});

//画bezier曲线（扩展函数）
EGEAPI void
drawbezier(int numpoints, const int* polypoints, IMAGE* pimg = {});

//画填充的多边形
EGEAPI void
fillpoly(int numpoints, const int* polypoints, IMAGE* pimg = {});

//画渐变填充的多边形
EGEAPI void
fillpoly_gradient(int numpoints, const ege_colpoint* polypoints,
	IMAGE* pimg = {});

//按边界颜色填充区域
EGEAPI void
floodfill(int x, int y, int border, IMAGE* pimg = {});

//按起始点颜色填充区域
EGEAPI void
floodfillsurface(int x, int y, color_t areacolor, IMAGE* pimg = {});

}

#endif

