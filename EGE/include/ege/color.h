#ifndef Inc_ege_color_h_
#define Inc_ege_color_h_

#include "ege/base.h"

namespace ege
{

// 颜色模型转换函数
color_t     EGEAPI rgb2gray(color_t rgb);
void        EGEAPI rgb2hsl(color_t rgb, float* H, float* S, float* L);
void        EGEAPI rgb2hsv(color_t rgb, float* H, float* S, float* V);
color_t     EGEAPI hsl2rgb(float H, float S, float L);
color_t     EGEAPI hsv2rgb(float H, float S, float V);

}

#endif

