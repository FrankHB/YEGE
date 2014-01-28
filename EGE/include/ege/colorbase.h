#ifndef Inc_ege_colorbase_h_
#define Inc_ege_colorbase_h_

#include "def.h"
#include <cstdint>

yconstfn std::uint32_t
RGBTOBGR(std::uint32_t color)
{
	return ((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16)
		   | (color & 0xFF00FF00);
}

yconstfn std::uint32_t
EGERGB(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	return (r << 16) | (g << 8) | b;
}
yconstfn std::uint32_t
EGERGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
{
	return EGERGB(r, g, b) | a << 24;
}
yconstfn std::uint32_t
EGEARGB(std::uint8_t a, std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
	return EGERGB(r, g, b) | a << 24;
}


yconstfn std::uint32_t
EGEACOLOR(std::uint8_t a, std::uint32_t color)
{
	return (color & 0xFFFFFF) | (a << 24);
}
yconstfn std::uint32_t
EGECOLORA(std::uint8_t a, std::uint32_t color)
{
	return (color & 0xFFFFFF) | (a << 24);
}


yconstfn std::uint8_t
EGEGET_R(std::uint32_t c)
{
	return (c >> 16) & 0xFF;
}
yconstfn std::uint8_t
EGEGET_G(std::uint32_t c)
{
	return (c >> 8) & 0xFF;
}
yconstfn std::uint8_t
EGEGET_B(std::uint32_t c)
{
	return c & 0xFF;
}
yconstfn std::uint8_t
EGEGET_A(std::uint32_t c)
{
	return (c >> 24) & 0xFF;
}

yconstfn std::uint32_t
EGEGRAY(std::uint32_t gray)
{
	return (gray << 16) | (gray << 8) | gray;
}

yconstfn std::uint32_t
EGEGRAYA(std::uint32_t gray, std::uint8_t a)
{
	return EGEGRAY(gray) | (a << 24);
}

yconstfn std::uint32_t
EGEAGRAY(std::uint8_t a, std::uint32_t gray)
{
	return EGEGRAY(gray) | (a << 24);
}

// 兼容宏
#define RGBtoGRAY   rgb2gray
#define RGBtoHSL    rgb2hsl
#define RGBtoHSV    rgb2hsv
#define HSLtoRGB    hsl2rgb
#define HSVtoRGB    hsv2rgb

#endif

