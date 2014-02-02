#include "ege/colorbase.h"
#include <cmath>
#include <algorithm> // for std::min, std::max;
#include "head.h"

namespace ege
{

color_t
rgb2gray(color_t color)
{
	double c;
	color_t r;

	c = ((color >> 16) & 0xFF) * 0.299;
	c += ((color >> 8) & 0xFF) * 0.587;
	c += ((color) & 0xFF) * 0.114;
	r = color_t::Trait::IntegerType(c);
	return MakeGray(r);
}

void
rgb2hsl(color_t rgb, float* h, float* s, float* l)
{
	yassume(h),
	yassume(s),
	yassume(l);

	const auto hsl(ColorToHSL(rgb));

	yunseq(*h = hsl.h, *s = hsl.s, *l = hsl.l);
}

color_t
hsl2rgb(float h, float s, float l)
{
	return HSLToColor(hsl_t{h, s, l});
}

void
rgb2hsv(color_t rgb, float* h, float* s, float* v)
{
	yassume(h),
	yassume(s),
	yassume(v);

	[&](float& h, float& s, float& v){
		const float r(rgb.GetR() / 255.0f), g(rgb.GetG() / 255.0f),
			b(rgb.GetB() / 255.0f), max_color(std::max(r, std::max(g, b))),
			delta(max_color - std::min(r, std::min(g, b)));

		yunseq(s = max_color == 0.F ? 0.F : delta / max_color, v = max_color);
		h = (s > 0.F ? [=]{
			float res(0);

			if(r == max_color)
				res = (g - b) / delta;
			else if(g == max_color)
				res = 2.F + (b - r) / delta;
			else if(b == max_color)
				res = 4.F + (r - g) / delta;
			if(res < 0.F)
				res += 6.F;
			return res;
		}() : -6.F) * 60.F;
	}(*h, *s, *v);
}

color_t
hsv2rgb(float h, float s, float v)
{
	YAssert(IsInInterval<float>(h, 0, 360), "Invalid hue found."),
	YAssert(IsInClosedInterval(s, 0.F, 1.F), "Invalid saturation found."),
	YAssert(IsInClosedInterval(v, 0.F, 1.F), "Invalid value found.");

	if(s > 0.F)
	{
		h /= 60.F;
		if(h == 6.F)
			h = 0.F;

		const unsigned k(std::floor(h));
		const float f(h - k), aa(v * (1.F - s)),
			bb(v * (1.F - s * f)),
			cc(v * (1.F - (s * (1.F - f))));
		float r(0), g(0), b(0);

		switch(k)
		{
		case 0U:
			r = v;
			g = cc;
			b = aa;
			break;
		case 1U:
			r = bb;
			g = v;
			b = aa;
			break;
		case 2U:
			r = aa;
			g = v;
			b = cc;
			break;
		case 3U:
			r = aa;
			g = bb;
			b = v;
			break;
		case 4U:
			r = cc;
			g = aa;
			b = v;
			break;
		case 5U:
			r = v;
			g = aa;
			b = bb;
			break;
		default:
			YAssert(false, "Invalid state found.");
		}
		return Color(r * 255, g * 255, b * 255);
	}
	return MakeGray(v * 255);
}

} // namespace ege;

