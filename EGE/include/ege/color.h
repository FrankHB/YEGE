#ifndef Inc_ege_color_h_
#define Inc_ege_color_h_

#include "ege/colorbase.h" // for EGERGB, color_t;

namespace ege
{

// 颜色
enum COLORS
{
	BLACK = 0,
	BLUE = EGERGB(0, 0, 0xA8),
	GREEN = EGERGB(0, 0xA8, 0),
	CYAN = EGERGB(0, 0xA8, 0xA8),
	RED = EGERGB(0xA8, 0, 0),
	MAGENTA = EGERGB(0xA8, 0, 0xA8),
	BROWN = EGERGB(0xA8, 0xA8, 0),
	LIGHTGRAY = EGERGB(0xA8, 0xA8, 0xA8),
	DARKGRAY = EGERGB(0x54, 0x54, 0x54),
	LIGHTBLUE = EGERGB(0x54, 0x54, 0xFC),
	LIGHTGREEN = EGERGB(0x54, 0xFC, 0x54),
	LIGHTCYAN = EGERGB(0x54, 0xFC, 0xFC),
	LIGHTRED = EGERGB(0xFC, 0x54, 0x54),
	LIGHTMAGENTA = EGERGB(0xFC, 0x54, 0xFC),
	YELLOW = EGERGB(0xFC, 0xFC, 0x54),
	WHITE = EGERGB(0xFC, 0xFC, 0xFC),
};


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

