#ifndef Inc_ege_gapi_aa_h_
#define Inc_ege_gapi_aa_h_

#include "ege/base.h"
#include "ege/color.h"

namespace ege
{

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
ege_setpattern_pathgradient(ege_point center, color_t centercolor, int count,
	ege_point* points, int colcount, color_t* pointscolor, IMAGE* pimg = nullptr);

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

}

#endif

