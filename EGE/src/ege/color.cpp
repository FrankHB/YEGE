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
hsl2rgb(float _h, float _s, float _l)
{
	float r, g, b;

	_h /= 360.0f;
	if(_h < 0.0f)
		_h += int(-_h + 1);
	if(_h >= 1.0f)
		_h -= int(_h);
	if(_s == 0)
	{
		r = _l;
		g = _l;
		b = _l;
	}
	else
	{
		float dp[3];
		float xh = _h * 6;

		if(xh == 6)
			xh = 0;

		int i(floor(xh) + 0.1), n;

		dp[0] = 1;
		dp[2] = 0;
		xh -= i;
		if(i & 1)
			dp[1] = 1 - xh;
		else
			dp[1] = xh;
#define SATFORMAT(c, s) (((c)-0.5f)*(s)+0.5f)
		for(n = 0; n < 3; ++n)
			dp[n] = SATFORMAT(dp[n], _s);
#undef SATFORMAT
#define BLACKFORMAT(c, v) ((c)*(v)*2)
#define WHITEFORMAT(c, v) (1-(1-(c))*(1-(v))*2)
		if(_l == 0.5f)
			;
		else if(_l < 0.5f)
		{
			if(_l < 0)
				_l = 0;
			for(n = 0; n < 3; ++n)
				dp[n] = BLACKFORMAT(dp[n], _l);
		}
		else
		{
			if(_l > 1)
				_l = 1;
			for(n = 0; n < 3; ++n)
				dp[n] = WHITEFORMAT(dp[n], _l);
		}
#undef BLACKFORMAT
#undef WHITEFORMAT
		if(i == 0)
		{
			r = dp[0];
			g = dp[1];
			b = dp[2];
		}
		else if(i == 1)
		{
			r = dp[1];
			g = dp[0];
			b = dp[2];
		}
		else if(i == 2)
		{
			r = dp[2];
			g = dp[0];
			b = dp[1];
		}
		else if(i == 3)
		{
			r = dp[2];
			g = dp[1];
			b = dp[0];
		}
		else if(i == 4)
		{
			r = dp[1];
			g = dp[2];
			b = dp[0];
		}
		else
		{
			r = dp[0];
			g = dp[2];
			b = dp[1];
		}
	}
	return EGERGB(r * 255, g * 255, b * 255);
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

