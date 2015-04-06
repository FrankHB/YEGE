#ifndef Inc_ege_colorbase_h_
#define Inc_ege_colorbase_h_

#include "ege/def.h"
#if YEGE_Use_YSLib
#	include <ysbuild.h>
#else
#	include <cstdint>
#endif

namespace ege
{

#if YEGE_Use_YSLib
using color_t = YSLib::Drawing::Pixel;
using color_int_t = color_t::Trait::IntegerType;
using mono_t = YSLib::Drawing::MonoType;
#else
using color_t = int;
using color_int_t = std::uint32_t;
using mono_t = std::uint8_t;
#endif

yconstfn color_int_t
RGBTOBGR(mono_t color)
{
	return ((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16)
		   | (color & 0xFF00FF00);
}

yconstfn color_int_t
EGERGB(mono_t r, mono_t g, mono_t b)
{
	return (b << 16) | (g << 8) | r;
}

yconstfn color_int_t
EGERGBA(mono_t r, mono_t g, mono_t b, mono_t a)
{
	return EGERGB(r, g, b) | a << 24;
}

yconstfn color_int_t
EGEARGB(mono_t a, mono_t r, mono_t g,
	mono_t b)
{
	return EGERGB(r, g, b) | a << 24;
}


yconstfn color_int_t
EGEACOLOR(mono_t a, color_int_t color)
{
	return (color & 0xFFFFFF) | (a << 24);
}
yconstfn color_int_t
EGECOLORA(mono_t a, color_int_t color)
{
	return (color & 0xFFFFFF) | (a << 24);
}


yconstfn mono_t
EGEGET_R(color_int_t c)
{
	return (c >> 16) & 0xFF;
}

yconstfn mono_t
EGEGET_G(color_int_t c)
{
	return (c >> 8) & 0xFF;
}

yconstfn mono_t
EGEGET_B(color_int_t c)
{
	return c & 0xFF;
}

yconstfn mono_t
EGEGET_A(color_int_t c)
{
	return (c >> 24) & 0xFF;
}

yconstfn color_int_t
EGEGRAY(color_int_t gray)
{
	return (gray << 16) | (gray << 8) | gray;
}

yconstfn color_int_t
EGEGRAYA(color_int_t gray, mono_t a)
{
	return EGEGRAY(gray) | (a << 24);
}

yconstfn color_int_t
EGEAGRAY(mono_t a, color_int_t gray)
{
	return EGEGRAY(gray) | (a << 24);
}

} // namespace ege;

#endif

