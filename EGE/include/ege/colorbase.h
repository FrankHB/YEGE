#ifndef Inc_ege_colorbase_h_
#define Inc_ege_colorbase_h_

#include "def.h"
#include <cstdint>
#include <ysbuild.h>
#include YFM_YCLib_Video

namespace ege
{

using color_t = platform::PixelType;

yconstfn color_t::Trait::IntegerType
RGBTOBGR(platform::MonoType color)
{
	return ((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16)
		   | (color & 0xFF00FF00);
}

yconstfn color_t::Trait::IntegerType
EGERGB(platform::MonoType r, platform::MonoType g, platform::MonoType b)
{
	return (r << 16) | (g << 8) | b;
}

yconstfn color_t::Trait::IntegerType
EGERGBA(platform::MonoType r, platform::MonoType g, platform::MonoType b,
	platform::MonoType a)
{
	return EGERGB(r, g, b) | a << 24;
}

yconstfn color_t::Trait::IntegerType
EGEARGB(platform::MonoType a, platform::MonoType r, platform::MonoType g,
	platform::MonoType b)
{
	return EGERGB(r, g, b) | a << 24;
}


yconstfn color_t::Trait::IntegerType
EGEACOLOR(platform::MonoType a, color_t::Trait::IntegerType color)
{
	return (color & 0xFFFFFF) | (a << 24);
}
yconstfn color_t::Trait::IntegerType
EGECOLORA(platform::MonoType a, color_t::Trait::IntegerType color)
{
	return (color & 0xFFFFFF) | (a << 24);
}


yconstfn platform::MonoType
EGEGET_R(color_t::Trait::IntegerType c)
{
	return (c >> 16) & 0xFF;
}

yconstfn platform::MonoType
EGEGET_G(color_t::Trait::IntegerType c)
{
	return (c >> 8) & 0xFF;
}

yconstfn platform::MonoType
EGEGET_B(color_t::Trait::IntegerType c)
{
	return c & 0xFF;
}

yconstfn platform::MonoType
EGEGET_A(color_t::Trait::IntegerType c)
{
	return (c >> 24) & 0xFF;
}

yconstfn color_t::Trait::IntegerType
EGEGRAY(color_t::Trait::IntegerType gray)
{
	return (gray << 16) | (gray << 8) | gray;
}

yconstfn color_t::Trait::IntegerType
EGEGRAYA(color_t::Trait::IntegerType gray, platform::MonoType a)
{
	return EGEGRAY(gray) | (a << 24);
}

yconstfn color_t::Trait::IntegerType
EGEAGRAY(platform::MonoType a, color_t::Trait::IntegerType gray)
{
	return EGEGRAY(gray) | (a << 24);
}

// 兼容宏
#define RGBtoGRAY   rgb2gray
#define RGBtoHSL    rgb2hsl
#define RGBtoHSV    rgb2hsv
#define HSLtoRGB    hsl2rgb
#define HSVtoRGB    hsv2rgb

} // namespace ege;

#endif

