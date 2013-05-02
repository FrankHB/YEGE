#ifndef Inc_ege_e_img_h_
#define Inc_ege_e_img_h_

#include "e_base.h"

namespace ege
{

class IMAGE;
typedef IMAGE* PIMAGE;


// 绘图环境设置
PIMAGE  gettarget();
int     settarget(PIMAGE pbuf); // 用 nullptr 设置窗口为绘图目标

void EGEAPI cleardevice(PIMAGE pimg = nullptr);                   // 清屏

void EGEAPI getviewport(int* pleft, int* ptop, int* pright, int* pbottom, int* pclip = nullptr, PIMAGE pimg = nullptr); // 获取视图信息
void EGEAPI setviewport(int left, int top, int right, int bottom, int clip = 1, PIMAGE pimg = nullptr);           // 设置视图
void EGEAPI clearviewport(PIMAGE pimg = nullptr);                                                                 // 清空视图

// 绘图属性
void EGEAPI getlinestyle(int* plinestyle, unsigned short* pupattern = nullptr, int* pthickness = nullptr, PIMAGE pimg = nullptr); // 获取当前线形
void EGEAPI setlinestyle(int linestyle, unsigned short upattern = 0, int thickness = 1, PIMAGE pimg = nullptr);          // 设置当前线形
void EGEAPI setlinewidth(float width, PIMAGE pimg = nullptr);          // 设置当前线宽
void EGEAPI setfillstyle(int pattern, color_t color, PIMAGE pimg = nullptr);  // 设置填充类型
void EGEAPI setwritemode(int mode, PIMAGE pimg = nullptr);         // 设置绘图位操作模式

// 色彩函数
color_t EGEAPI getcolor(PIMAGE pimg = nullptr);                    // 获取当前绘图前景色
color_t EGEAPI getfillcolor(PIMAGE pimg = nullptr);                // 获取当前绘图填充色
color_t EGEAPI getbkcolor(PIMAGE pimg = nullptr);                  // 获取当前绘图背景色
void EGEAPI setcolor(color_t color, PIMAGE pimg = nullptr);        // 设置当前绘图前景色
void EGEAPI setfillcolor(color_t color, PIMAGE pimg = nullptr);    // 设置当前绘图填充色
void EGEAPI setbkcolor(color_t color, PIMAGE pimg = nullptr);      // 设置当前绘图背景色（设置并做背景色像素替换）
void EGEAPI setbkcolor_f(color_t color, PIMAGE pimg = nullptr);    // 快速设置当前绘图背景色（只设置不绘画）
void EGEAPI setfontbkcolor(color_t color, PIMAGE pimg = nullptr);  // 设置当前文字背景色
void EGEAPI setbkmode(int iBkMode, PIMAGE pimg = nullptr);         // 设置背景混合模式(0=OPAQUE, 1=TRANSPARENT)

// 基本绘图函数

color_t     EGEAPI getpixel(int x, int y, PIMAGE pimg = nullptr);                  // 获取点的颜色
void        EGEAPI putpixel(int x, int y, color_t color, PIMAGE pimg = nullptr);   // 画点
color_t     EGEAPI getpixel_f(int x, int y, PIMAGE pimg = nullptr);                // 获取点的颜色
void        EGEAPI putpixel_f(int x, int y, color_t color, PIMAGE pimg = nullptr); // 画点
void        EGEAPI putpixels(int nPoint, int* pPoints, PIMAGE pimg = nullptr);     // 批量画点
void        EGEAPI putpixels_f(int nPoint, int* pPoints, PIMAGE pimg = nullptr);   // 批量画点

void EGEAPI moveto(int x, int y, PIMAGE pimg = nullptr);                      // 移动当前点(绝对坐标)
void EGEAPI moverel(int dx, int dy, PIMAGE pimg = nullptr);                   // 移动当前点(相对坐标)

void EGEAPI line(int x1, int y1, int x2, int y2, PIMAGE pimg = nullptr);      // 画线
void EGEAPI linerel(int dx, int dy, PIMAGE pimg = nullptr);                   // 画线(至相对坐标)
void EGEAPI lineto(int x, int y, PIMAGE pimg = nullptr);                      // 画线(至绝对坐标)
void EGEAPI line_f(float x1, float y1, float x2, float y2, PIMAGE pimg = nullptr);  // 画线
void EGEAPI linerel_f(float dx, float dy, PIMAGE pimg = nullptr);                   // 画线(至相对坐标)
void EGEAPI lineto_f(float x, float y, PIMAGE pimg = nullptr);                      // 画线(至绝对坐标)

void EGEAPI rectangle(int left, int top, int right, int bottom, PIMAGE pimg = nullptr);   // 画矩形

void EGEAPI arc(int x, int y, int stangle, int endangle, int radius, PIMAGE pimg = nullptr);                  // 画圆弧
void EGEAPI circle(int x, int y, int radius, PIMAGE pimg = nullptr);                                          // 画圆
void EGEAPI pieslice(int x, int y, int stangle, int endangle, int radius, PIMAGE pimg = nullptr);             // 画填充圆扇形
void EGEAPI ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius, PIMAGE pimg = nullptr);// 画椭圆弧线
void EGEAPI fillellipse(int x, int y, int xradius, int yradius, PIMAGE pimg = nullptr);                       // 画填充椭圆
void EGEAPI sector(int x, int y, int stangle, int endangle, int xradius, int yradius, PIMAGE pimg = nullptr); // 画填充椭圆扇形

void EGEAPI arcf(float x, float y, float stangle, float endangle, float radius, PIMAGE pimg = nullptr);                    // 画圆弧
void EGEAPI circlef(float x, float y, float radius, PIMAGE pimg = nullptr);                                                // 画圆
void EGEAPI pieslicef(float x, float y, float stangle, float endangle, float radius, PIMAGE pimg = nullptr);               // 画填充圆扇形
void EGEAPI ellipsef(float x, float y, float stangle, float endangle, float xradius, float yradius, PIMAGE pimg = nullptr);// 画椭圆弧线
void EGEAPI fillellipsef(float x, float y, float xradius, float yradius, PIMAGE pimg = nullptr);                           // 画填充椭圆
void EGEAPI sectorf(float x, float y, float stangle, float endangle, float xradius, float yradius, PIMAGE pimg = nullptr); // 画填充椭圆扇形

void EGEAPI bar(int left, int top, int right, int bottom, PIMAGE pimg = nullptr);                             // 画无边框填充矩形
void EGEAPI bar3d(int left, int top, int right, int bottom, int depth, int topflag, PIMAGE pimg = nullptr);   // 画有边框三维填充矩形

void EGEAPI drawpoly(int numpoints, const int* polypoints, PIMAGE pimg = nullptr);     // 画多边形
void EGEAPI drawlines(int numlines, const int* polypoints, PIMAGE pimg = nullptr);     // 画多条不连续线（扩展函数）
void EGEAPI drawbezier(int numpoints, const int* polypoints, PIMAGE pimg = nullptr);   // 画bezier曲线（扩展函数）
void EGEAPI fillpoly(int numpoints, const int* polypoints, PIMAGE pimg = nullptr);     // 画填充的多边形
void EGEAPI fillpoly_gradient(int numpoints, const ege_colpoint* polypoints, PIMAGE pimg = nullptr); // 画渐变填充的多边形
void EGEAPI floodfill(int x, int y, int border, PIMAGE pimg = nullptr);                // 按边界颜色填充区域
void EGEAPI floodfillsurface(int x, int y, color_t areacolor, PIMAGE pimg = nullptr);  // 按起始点颜色填充区域

// 高级绘图函数（带AA）
// ege new_api
void EGEAPI ege_enable_aa(bool enable, PIMAGE pimg = nullptr);

void EGEAPI ege_line(float x1, float y1, float x2, float y2, PIMAGE pimg = nullptr);
void EGEAPI ege_drawpoly(int numpoints, ege_point* polypoints, PIMAGE pimg = nullptr);
void EGEAPI ege_drawcurve(int numpoints, ege_point* polypoints, PIMAGE pimg = nullptr);
void EGEAPI ege_rectangle(float x, float y, float w, float h, PIMAGE pimg = nullptr);
void EGEAPI ege_ellipse(float x, float y, float w, float h, PIMAGE pimg = nullptr);
void EGEAPI ege_pie(float x, float y, float w, float h, float stangle, float sweepAngle, PIMAGE pimg = nullptr);

void EGEAPI ege_arc(float x, float y, float w, float h, float stangle, float sweepAngle, PIMAGE pimg = nullptr);
void EGEAPI ege_bezier(int numpoints, ege_point* polypoints, PIMAGE pimg = nullptr);

void EGEAPI ege_fillpoly(int numpoints, ege_point* polypoints, PIMAGE pimg = nullptr);
void EGEAPI ege_fillrect(float x, float y, float w, float h, PIMAGE pimg = nullptr);
void EGEAPI ege_fillellipse(float x, float y, float w, float h, PIMAGE pimg = nullptr);
void EGEAPI ege_fillpie(float x, float y, float w, float h, float stangle, float sweepAngle, PIMAGE pimg = nullptr);

void EGEAPI ege_setpattern_none(PIMAGE pimg = nullptr);
void EGEAPI ege_setpattern_lineargradient(float x1, float y1, color_t c1, float x2, float y2, color_t c2, PIMAGE pimg = nullptr);
void EGEAPI ege_setpattern_pathgradient(ege_point center, color_t centercolor,
										int count, ege_point* points, int colcount, color_t* pointscolor, PIMAGE pimg = nullptr);
void EGEAPI ege_setpattern_ellipsegradient(ege_point center, color_t centercolor,
		float x, float y, float w, float h, color_t color, PIMAGE pimg = nullptr);
void EGEAPI ege_setpattern_texture(PIMAGE srcimg, float x, float y, float w, float h, PIMAGE pimg = nullptr);

void EGEAPI ege_setalpha(int alpha, PIMAGE pimg = nullptr);
void EGEAPI ege_gentexture(bool gen, PIMAGE pimg = nullptr);
void EGEAPI ege_puttexture(PIMAGE srcimg, float x, float y, float w, float h, PIMAGE pimg = nullptr);
void EGEAPI ege_puttexture(PIMAGE srcimg, ege_rect dest, PIMAGE pimg = nullptr);
void EGEAPI ege_puttexture(PIMAGE srcimg, ege_rect dest, ege_rect src, PIMAGE pimg = nullptr);
//

//图片函数
#define getmaxx getwidth
#define getmaxy getheight

int EGEAPI getwidth(PIMAGE pimg = nullptr);           // 获取图片宽度
int EGEAPI getheight(PIMAGE pimg = nullptr);          // 获取图片高度
int EGEAPI getx(PIMAGE pimg = nullptr);               // 获取当前 x 坐标
int EGEAPI gety(PIMAGE pimg = nullptr);               // 获取当前 y 坐标

PIMAGE EGEAPI newimage(); // 创建PIMAGE
PIMAGE EGEAPI newimage(int width, int height); // 创建PIMAGE
void   EGEAPI delimage(PIMAGE pImg); // 创建PIMAGE
void*  EGEAPI getbuffer(PIMAGE pImg);

int  EGEAPI resize(PIMAGE pDstImg, int width, int height); //重设尺寸
void EGEAPI getimage(PIMAGE pDstImg, int srcX, int srcY, int srcWidth, int srcHeight);             // 从屏幕获取图像
void EGEAPI getimage(PIMAGE pDstImg, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight);   // 从另一个 PIMAGE 对象中获取图像

int EGEAPI putimage_transparent(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent,  // color to make transparent
	int nXOriginSrc = 0,    // x-coord of source upper-left corner
	int nYOriginSrc = 0,    // y-coord of source upper-left corner
	int nWidthSrc = 0,      // width of source rectangle
	int nHeightSrc = 0      // height of source rectangle
);
int EGEAPI putimage_alphablend(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	unsigned char alpha,    // alpha
	int nXOriginSrc = 0,    // x-coord of source upper-left corner
	int nYOriginSrc = 0,    // y-coord of source upper-left corner
	int nWidthSrc = 0,      // width of source rectangle
	int nHeightSrc = 0      // height of source rectangle
);
int EGEAPI putimage_alphatransparent(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	color_t crTransparent,  // color to make transparent
	unsigned char alpha,    // alpha
	int nXOriginSrc = 0,    // x-coord of source upper-left corner
	int nYOriginSrc = 0,    // y-coord of source upper-left corner
	int nWidthSrc = 0,      // width of source rectangle
	int nHeightSrc = 0      // height of source rectangle
);
int EGEAPI putimage_withalpha(
	PIMAGE imgdest,         // handle to dest
	PIMAGE imgsrc,          // handle to source
	int nXOriginDest,       // x-coord of destination upper-left corner
	int nYOriginDest,       // y-coord of destination upper-left corner
	int nXOriginSrc = 0,    // x-coord of source upper-left corner
	int nYOriginSrc = 0,    // y-coord of source upper-left corner
	int nWidthSrc = 0,      // width of source rectangle
	int nHeightSrc = 0      // height of source rectangle
);
int EGEAPI imagefilter_blurring(
	PIMAGE imgdest,         // handle to dest
	int intensity,
	int alpha,
	int nXOriginDest = 0,
	int nYOriginDest = 0,
	int nWidthDest = 0,
	int nHeightDest = 0
);
int EGEAPI putimage_rotate(
	PIMAGE imgdest,
	PIMAGE imgtexture,
	int nXOriginDest,
	int nYOriginDest,
	float centerx,
	float centery,
	float radian,
	int btransparent = 0,           // transparent (1) or not (0)
	int alpha = -1,                  // in range[0, 256], alpha== -1 means no alpha
	int smooth = 0
);

int EGEAPI putimage_rotatezoom(
	PIMAGE imgdest,
	PIMAGE imgtexture,
	int nXOriginDest,
	int nYOriginDest,
	float centerx,
	float centery,
	float radian,
	float zoom,
	int btransparent = 0,           // transparent (1) or not (0)
	int alpha = -1,                  // in range[0, 256], alpha== -1 means no alpha
	int smooth = 0
);

}

#endif

