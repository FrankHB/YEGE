#ifndef Inc_ege_color_h_
#define Inc_ege_color_h_

#include "ege/def.h"

namespace ege
{

typedef unsigned int color_t;


struct ege_colpoint
{
	float x;
	float y;
	color_t color;
};


// 颜色模型转换函数
EGEAPI color_t
rgb2gray(color_t rgb);

EGEAPI void
rgb2hsl(color_t rgb, float* H, float* S, float* L);

EGEAPI void
rgb2hsv(color_t rgb, float* H, float* S, float* V);

EGEAPI color_t
hsl2rgb(float H, float S, float L);

EGEAPI color_t
hsv2rgb(float H, float S, float V);

}

#endif

