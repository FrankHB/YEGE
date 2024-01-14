#ifndef Inc_ege_colorbase_h_
#define Inc_ege_colorbase_h_

#include "ege/def.h"
#if YEGE_Use_YSLib
#	include <YSBuild.h>
#else
#	include <cstdint>
#endif

namespace ege
{

#if YEGE_Use_YSLib
using color_t = YSLib::Drawing::Pixel;
using color_int_t = color_t::traits_type::integer;
using YSLib::Drawing::AlphaType;
using YSLib::Drawing::MonoType;
using YSLib::Drawing::BitmapPtr;
using YSLib::Drawing::ConstBitmapPtr;
#else
using color_t = unsigned;
using color_int_t = std::uint32_t;
using AlphaType = octet;
using MonoType = octet;
using BitmapPtr = color_int_t*;
using ConstBitmapPtr = const color_int_t*;
#endif

yconstfn color_int_t
ARGBTOZBGR(color_int_t color) ynothrow
{
	return
		((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16) | (color & 0xFF00);
}

yconstfn color_int_t
RGBTOBGR(color_int_t color) ynothrow
{
	return ((color & 0xFF) << 16) | ((color & 0xFF0000) >> 16)
		| (color & 0xFF00FF00);
}

yconstfn color_int_t
EGERGBA(MonoType r, MonoType g, MonoType b, AlphaType a) ynothrow
{
	return
		color_int_t(r << 16) | color_int_t(g << 8) | color_int_t(b) | a << 24;
}

yconstfn color_int_t
EGEARGB(AlphaType a, MonoType r, MonoType g, MonoType b) ynothrow
{
	return ege::EGERGBA(r, g, b, a);
}

yconstfn color_int_t
EGEACOLOR(AlphaType a, color_int_t color) ynothrow
{
	return (color & 0xFFFFFF) | (a << 24);
}

yconstfn color_int_t
EGECOLORA(AlphaType a, color_int_t color) ynothrow
{
	return (color & 0xFFFFFF) | (a << 24);
}

yconstfn color_int_t
EGERGB(MonoType r, MonoType g, MonoType b) ynothrow
{
	return EGERGBA(r, g, b, 0xFF);
}

yconstfn MonoType
EGEGET_R(color_int_t c) ynothrow
{
	return (c >> 16) & 0xFF;
}

yconstfn MonoType
EGEGET_G(color_int_t c) ynothrow
{
	return (c >> 8) & 0xFF;
}

yconstfn MonoType
EGEGET_B(color_int_t c) ynothrow
{
	return c & 0xFF;
}

yconstfn AlphaType
EGEGET_A(color_int_t c) ynothrow
{
	return (c >> 24) & 0xFF;
}

yconstfn color_int_t
EGEGRAY(color_int_t gray) ynothrow
{
	return (gray << 16) | (gray << 8) | gray;
}

yconstfn color_int_t
EGEGRAYA(color_int_t gray, AlphaType a) ynothrow
{
	return EGEGRAY(gray) | (a << 24);
}

yconstfn color_int_t
EGEAGRAY(AlphaType a, color_int_t gray) ynothrow
{
	return EGEGRAY(gray) | (a << 24);
}

} // namespace ege;

#endif

