#pragma once

#include <cstdint>

typedef std::uint8_t u8;
typedef std::uint32_t u32;

yconstfn u32
RGBTOBGR(u32 color)
{
	return ((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16)
		| (color & 0xFF00FF00);
}

yconstfn u32
EGERGB(u8 r, u8 g, u8 b)
{
	return (r << 16) | (g << 8) | b;
}
yconstfn u32
EGERGBA(u8 r, u8 g, u8 b, u8 a)
{
	return EGERGB(r, g, b) | a << 24;
}
yconstfn u32
EGEARGB(u8 a, u8 r, u8 g, u8 b)
{
	return EGERGB(r, g, b) | a << 24;
}


yconstfn u32
EGEACOLOR(u8 a, u32 color)
{
	return (color & 0xFFFFFF) | (a << 24);
}
yconstfn u32
EGECOLORA(u8 a, u32 color)
{
	return (color & 0xFFFFFF) | (a << 24);
}


yconstfn u8
EGEGET_R(u32 c)
{
	return (c >> 16) & 0xFF;
}
yconstfn u8
EGEGET_G(u32 c)
{
	return (c >> 8) & 0xFF;
}
yconstfn u8
EGEGET_B(u32 c)
{
	return c & 0xFF;
}
yconstfn u8
EGEGET_A(u32 c)
{
	return (c >> 24) & 0xFF;
}

yconstfn u32
EGEGRAY(u32 gray)
{
	return (gray << 16) | (gray << 8) | gray;
}

yconstfn u32
EGEGRAYA(u32 gray, u8 a)
{
	return EGEGRAY(gray) | (a << 24);
}

yconstfn u32
EGEAGRAY(u8 a, u32 gray)
{
	return EGEGRAY(gray) | (a << 24);
}

// ¼æÈÝºê
#define RGBtoGRAY   rgb2gray
#define RGBtoHSL    rgb2hsl
#define RGBtoHSV    rgb2hsv
#define HSLtoRGB    hsl2rgb
#define HSVtoRGB    hsv2rgb

