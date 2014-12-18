#include "ege/colorbase.h"
#include "head.h"

namespace ege
{

color_t
rgb2gray(color_t color)
{
	return MakeGray(color_t::Trait::IntegerType(((color >> 16) & 0xFF) * 0.299
		+ ((color >> 8) & 0xFF) * 0.587 + ((color) & 0xFF) * 0.114));
}

void
rgb2hsl(color_t rgb, float* h, float* s, float* l)
{
	HSL hsl(rgb);

	yunseq(Deref(h) = hsl.GetH(), Deref(s) = hsl.GetS(), Deref(l) = hsl.GetL());
}

color_t
hsl2rgb(float h, float s, float l)
{
	return Color(HSL(h, s, l));
}

void
rgb2hsv(color_t rgb, float* h, float* s, float* v)
{
	HSV hsv(rgb);

	yunseq(Deref(h) = hsv.GetH(), Deref(s) = hsv.GetS(), Deref(v) = hsv.GetV());
}

color_t
hsv2rgb(float h, float s, float v)
{
	return Color(HSV(h, s, v));
}

} // namespace ege;

