#pragma once

#include "e_base.h"

namespace ege
{

class IMAGE;
typedef IMAGE *PIMAGE;


// ��ͼ��������
PIMAGE  gettarget();
int     settarget(PIMAGE pbuf); // �� NULL ���ô���Ϊ��ͼĿ��

void EGEAPI cleardevice(PIMAGE pimg = NULL);                   // ����

void EGEAPI getviewport(int *pleft, int *ptop, int *pright, int *pbottom, int *pclip = 0, PIMAGE pimg = NULL); // ��ȡ��ͼ��Ϣ
void EGEAPI setviewport(int left, int top, int right, int bottom, int clip = 1, PIMAGE pimg = NULL);           // ������ͼ
void EGEAPI clearviewport(PIMAGE pimg = NULL);                                                                 // �����ͼ

// ��ͼ����
void EGEAPI getlinestyle(int *plinestyle, unsigned short *pupattern = NULL, int *pthickness = NULL, PIMAGE pimg = NULL); // ��ȡ��ǰ����
void EGEAPI setlinestyle(int linestyle, unsigned short upattern = 0, int thickness = 1, PIMAGE pimg = NULL);          // ���õ�ǰ����
void EGEAPI setlinewidth(float width, PIMAGE pimg = NULL);          // ���õ�ǰ�߿�
void EGEAPI setfillstyle(int pattern, color_t color, PIMAGE pimg = NULL);  // �����������
void EGEAPI setwritemode(int mode, PIMAGE pimg = NULL);         // ���û�ͼλ����ģʽ

// ɫ�ʺ���
color_t EGEAPI getcolor(PIMAGE pimg = NULL);                    // ��ȡ��ǰ��ͼǰ��ɫ
color_t EGEAPI getfillcolor(PIMAGE pimg = NULL);                // ��ȡ��ǰ��ͼ���ɫ
color_t EGEAPI getbkcolor(PIMAGE pimg = NULL);                  // ��ȡ��ǰ��ͼ����ɫ
void EGEAPI setcolor(color_t color, PIMAGE pimg = NULL);        // ���õ�ǰ��ͼǰ��ɫ
void EGEAPI setfillcolor(color_t color, PIMAGE pimg = NULL);    // ���õ�ǰ��ͼ���ɫ
void EGEAPI setbkcolor(color_t color, PIMAGE pimg = NULL);      // ���õ�ǰ��ͼ����ɫ�����ò�������ɫ�����滻��
void EGEAPI setbkcolor_f(color_t color, PIMAGE pimg = NULL);    // �������õ�ǰ��ͼ����ɫ��ֻ���ò��滭��
void EGEAPI setfontbkcolor(color_t color, PIMAGE pimg = NULL);  // ���õ�ǰ���ֱ���ɫ
void EGEAPI setbkmode(int iBkMode, PIMAGE pimg = NULL);         // ���ñ������ģʽ(0=OPAQUE, 1=TRANSPARENT)

// ������ͼ����

color_t     EGEAPI getpixel  (int x, int y, PIMAGE pimg = NULL);                // ��ȡ�����ɫ
void        EGEAPI putpixel  (int x, int y, color_t color, PIMAGE pimg = NULL); // ����
color_t     EGEAPI getpixel_f(int x, int y, PIMAGE pimg = NULL);                // ��ȡ�����ɫ
void        EGEAPI putpixel_f(int x, int y, color_t color, PIMAGE pimg = NULL); // ����
void        EGEAPI putpixels  (int nPoint, int* pPoints, PIMAGE pimg = NULL);   // ��������
void        EGEAPI putpixels_f(int nPoint, int* pPoints, PIMAGE pimg = NULL);   // ��������

void EGEAPI moveto(int x, int y, PIMAGE pimg = NULL);                      // �ƶ���ǰ��(��������)
void EGEAPI moverel(int dx, int dy, PIMAGE pimg = NULL);                   // �ƶ���ǰ��(�������)

void EGEAPI line(int x1, int y1, int x2, int y2, PIMAGE pimg = NULL);      // ����
void EGEAPI linerel(int dx, int dy, PIMAGE pimg = NULL);                   // ����(���������)
void EGEAPI lineto(int x, int y, PIMAGE pimg = NULL);                      // ����(����������)
void EGEAPI line_f(float x1, float y1, float x2, float y2, PIMAGE pimg = NULL);  // ����
void EGEAPI linerel_f(float dx, float dy, PIMAGE pimg = NULL);                   // ����(���������)
void EGEAPI lineto_f(float x, float y, PIMAGE pimg = NULL);                      // ����(����������)

void EGEAPI rectangle(int left, int top, int right, int bottom, PIMAGE pimg = NULL);   // ������

void EGEAPI arc(int x, int y, int stangle, int endangle, int radius, PIMAGE pimg = NULL);                  // ��Բ��
void EGEAPI circle(int x, int y, int radius, PIMAGE pimg = NULL);                                          // ��Բ
void EGEAPI pieslice(int x, int y, int stangle, int endangle, int radius, PIMAGE pimg = NULL);             // �����Բ����
void EGEAPI ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius, PIMAGE pimg = NULL);// ����Բ����
void EGEAPI fillellipse(int x, int y, int xradius, int yradius, PIMAGE pimg = NULL);                       // �������Բ
void EGEAPI sector(int x, int y, int stangle, int endangle, int xradius, int yradius, PIMAGE pimg = NULL); // �������Բ����

void EGEAPI arcf(float x, float y, float stangle, float endangle, float radius, PIMAGE pimg = NULL);                    // ��Բ��
void EGEAPI circlef(float x, float y, float radius, PIMAGE pimg = NULL);                                                // ��Բ
void EGEAPI pieslicef(float x, float y, float stangle, float endangle, float radius, PIMAGE pimg = NULL);               // �����Բ����
void EGEAPI ellipsef(float x, float y, float stangle, float endangle, float xradius, float yradius, PIMAGE pimg = NULL);// ����Բ����
void EGEAPI fillellipsef(float x, float y, float xradius, float yradius, PIMAGE pimg = NULL);                           // �������Բ
void EGEAPI sectorf(float x, float y, float stangle, float endangle, float xradius, float yradius, PIMAGE pimg = NULL); // �������Բ����

void EGEAPI bar(int left, int top, int right, int bottom, PIMAGE pimg = NULL);                             // ���ޱ߿�������
void EGEAPI bar3d(int left, int top, int right, int bottom, int depth, int topflag, PIMAGE pimg = NULL);   // ���б߿���ά������

void EGEAPI drawpoly(int numpoints, const int *polypoints, PIMAGE pimg = NULL);     // �������
void EGEAPI drawlines(int numlines, const int *polypoints, PIMAGE pimg = NULL);     // �������������ߣ���չ������
void EGEAPI drawbezier(int numpoints, const int *polypoints, PIMAGE pimg = NULL);   // ��bezier���ߣ���չ������
void EGEAPI fillpoly(int numpoints, const int *polypoints, PIMAGE pimg = NULL);     // �����Ķ����
void EGEAPI fillpoly_gradient(int numpoints, const ege_colpoint* polypoints, PIMAGE pimg = NULL); // ���������Ķ����
void EGEAPI floodfill(int x, int y, int border, PIMAGE pimg = NULL);                // ���߽���ɫ�������
void EGEAPI floodfillsurface(int x, int y, color_t areacolor, PIMAGE pimg = NULL);  // ����ʼ����ɫ�������

// �߼���ͼ��������AA��
// ege new_api
void EGEAPI ege_enable_aa(bool enable, PIMAGE pimg = NULL);

void EGEAPI ege_line(float x1, float y1, float x2, float y2, PIMAGE pimg = NULL);
void EGEAPI ege_drawpoly(int numpoints, ege_point* polypoints, PIMAGE pimg = NULL);
void EGEAPI ege_drawcurve(int numpoints, ege_point* polypoints, PIMAGE pimg = NULL);
void EGEAPI ege_rectangle(float x, float y, float w, float h, PIMAGE pimg = NULL);
void EGEAPI ege_ellipse(float x, float y, float w, float h, PIMAGE pimg = NULL);
void EGEAPI ege_pie(float x, float y, float w, float h, float stangle, float sweepAngle, PIMAGE pimg = NULL);

void EGEAPI ege_arc(float x, float y, float w, float h, float stangle, float sweepAngle, PIMAGE pimg = NULL);
void EGEAPI ege_bezier(int numpoints, ege_point* polypoints, PIMAGE pimg = NULL);

void EGEAPI ege_fillpoly(int numpoints, ege_point* polypoints, PIMAGE pimg = NULL);
void EGEAPI ege_fillrect(float x, float y, float w, float h, PIMAGE pimg = NULL);
void EGEAPI ege_fillellipse(float x, float y, float w, float h, PIMAGE pimg = NULL);
void EGEAPI ege_fillpie(float x, float y, float w, float h, float stangle, float sweepAngle, PIMAGE pimg = NULL);

void EGEAPI ege_setpattern_none(PIMAGE pimg = NULL);
void EGEAPI ege_setpattern_lineargradient(float x1, float y1, color_t c1, float x2, float y2, color_t c2, PIMAGE pimg = NULL);
void EGEAPI ege_setpattern_pathgradient(ege_point center, color_t centercolor,
	int count, ege_point* points, int colcount, color_t* pointscolor, PIMAGE pimg = NULL);
void EGEAPI ege_setpattern_ellipsegradient(ege_point center, color_t centercolor,
	float x, float y, float w, float h, color_t color, PIMAGE pimg = NULL);
void EGEAPI ege_setpattern_texture(PIMAGE srcimg, float x, float y, float w, float h, PIMAGE pimg = NULL);

void EGEAPI ege_setalpha(int alpha, PIMAGE pimg = NULL);
void EGEAPI ege_gentexture(bool gen, PIMAGE pimg = NULL);
void EGEAPI ege_puttexture(PIMAGE srcimg, float x, float y, float w, float h, PIMAGE pimg = NULL);
void EGEAPI ege_puttexture(PIMAGE srcimg, ege_rect dest, PIMAGE pimg = NULL);
void EGEAPI ege_puttexture(PIMAGE srcimg, ege_rect dest, ege_rect src, PIMAGE pimg = NULL);
//

//ͼƬ����
#define getmaxx getwidth
#define getmaxy getheight

int EGEAPI getwidth(PIMAGE pimg = NULL);           // ��ȡͼƬ���
int EGEAPI getheight(PIMAGE pimg = NULL);          // ��ȡͼƬ�߶�
int EGEAPI getx(PIMAGE pimg = NULL);               // ��ȡ��ǰ x ����
int EGEAPI gety(PIMAGE pimg = NULL);               // ��ȡ��ǰ y ����

PIMAGE EGEAPI newimage(); // ����PIMAGE
PIMAGE EGEAPI newimage(int width, int height); // ����PIMAGE
void   EGEAPI delimage(PIMAGE pImg); // ����PIMAGE
void*  EGEAPI getbuffer(PIMAGE pImg);

int  EGEAPI resize(PIMAGE pDstImg, int width, int height); //����ߴ�
void EGEAPI getimage(PIMAGE pDstImg, int srcX, int srcY, int srcWidth, int srcHeight);             // ����Ļ��ȡͼ��
void EGEAPI getimage(PIMAGE pDstImg, const PIMAGE pSrcImg, int srcX, int srcY, int srcWidth, int srcHeight);   // ����һ�� PIMAGE �����л�ȡͼ��

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
int EGEAPI imagefilter_blurring (
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

