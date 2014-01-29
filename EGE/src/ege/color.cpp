#include "ege.h"
#include <cmath>
#include <algorithm> // for std::min, std::max;

namespace ege
{

using platform::MonoType;

struct COLORHSL
{
	float h;
	float s;
	float l;
};

struct COLORHSV
{
	float h;
	float s;
	float v;
};

struct COLORRGB
{
	MonoType r;
	MonoType g;
	MonoType b;
};

namespace
{

COLORHSV
RGB_TO_HSV(const COLORRGB& rgb)
{
	float r, g, b, minRGB, maxRGB, deltaRGB;
	COLORHSV output;

	output.h = 0;
	r = rgb.r / 255.0f;
	g = rgb.g / 255.0f;
	b = rgb.b / 255.0f;
	minRGB = std::min(r, std::min(g, b));
	maxRGB = std::max(r, std::max(g, b));
	deltaRGB = maxRGB - minRGB;
	output.v = maxRGB;
	if(maxRGB != 0.0f)
		output.s = deltaRGB / maxRGB;
	else
		output.s = 0.0f;
	if(output.s <= 0.0f)
		output.h = -1.0f;
	else
	{
		if(r == maxRGB)
			output.h = (g - b) / deltaRGB;
		else if(g == maxRGB)
			output.h = 2.0f + (b - r) / deltaRGB;
		else if(b == maxRGB)
			output.h = 4.0f + (r - g) / deltaRGB;
		output.h *= 60.0f;
		if(output.h < 0.0f)
			output.h += 360.0f;
		output.h /= 360.0f;
	}
	return output;
}

COLORRGB
HSV_TO_RGB(COLORHSV hsv)
{
	float R = 0, G = 0, B = 0;

	if(hsv.s <= 0.0)
		return {MonoType(hsv.v * 255), MonoType(hsv.v * 255),
			MonoType(hsv.v * 255)};
	else
	{
		if(hsv.h == 1.0)
			hsv.h = 0.0;
		hsv.h *= 6.0;

		int k = floor(hsv.h);
		float f = hsv.h - k, aa(hsv.v * (1.0f - hsv.s)),
			bb(hsv.v * (1.0f - hsv.s * f)),
			cc(hsv.v * (1.0f - (hsv.s * (1.0f - f))));

		switch(k)
		{
		case 0:
			R = hsv.v;
			G = cc;
			B = aa;
			break;
		case 1:
			R = bb;
			G = hsv.v;
			B = aa;
			break;
		case 2:
			R = aa;
			G = hsv.v;
			B = cc;
			break;
		case 3:
			R = aa;
			G = bb;
			B = hsv.v;
			break;
		case 4:
			R = cc;
			G = aa;
			B = hsv.v;
			break;
		case 5:
			R = hsv.v;
			G = aa;
			B = bb;
			break;
		}
	}
	return {MonoType(R * 255), MonoType(G * 255), MonoType(B * 255)};
}

}



color_t
rgb2gray(color_t color)
{
	double c;
	color_t r;

	c = ((color >> 16) & 0xFF) * 0.299;
	c += ((color >> 8) & 0xFF) * 0.587;
	c += ((color) & 0xFF) * 0.114;
	r = color_t::Trait::IntegerType(c);
	return EGERGB(r, r, r);
}

void
rgb2hsl(color_t rgb, float* H, float* S, float* L)
{
	COLORHSL hsl = [](color_t::Trait::IntegerType _col) -> COLORHSL{
		COLORHSL cr_col;
		float r(EGEGET_R(_col) / 255.0f), g(EGEGET_G(_col) / 255.0f),
			b(EGEGET_B(_col) / 255.0f);
		float*dp[3]{&r, &g, &b};

#define IFSWAP(a, b) \
	if(*a>*b) \
		std::swap(a, b)
		IFSWAP(dp[0], dp[1]);
		IFSWAP(dp[1], dp[2]);
		IFSWAP(dp[0], dp[1]);
#undef IFSWAP
		cr_col.l = (*dp[0] + *dp[2]) / 2;
		if(cr_col.l < 1e-2f)
		{
			cr_col.l = 0;
			cr_col.h = 0;
			cr_col.s = 0;
			return cr_col;
		}
		if(cr_col.l > 0.99)
		{
			cr_col.l = 1;
			cr_col.h = 0;
			cr_col.s = 0;
			return cr_col;
		}
		if(fabs(cr_col.l - 0.5) < 1e-2)
			cr_col.l = 0.5;
	#define BLACKUNFORMAT(c, v) ((c)/((v)*2))
	#define WHITEUNFORMAT(c, v) (1-(1-c)/((1-(v))*2))
		if(cr_col.l == 0.5)
			;
		else if(cr_col.l < 0.5)
			for(int n = 0; n < 3; ++n)
			{
				*dp[n] = BLACKUNFORMAT(*dp[n], cr_col.l);
				if(*dp[n] > 1)
					*dp[n] = 1;
			}
		else
			for(int n = 0; n < 3; ++n)
			{
				*dp[n] = WHITEUNFORMAT(*dp[n], cr_col.l);
				if(*dp[n] > 1)
					*dp[n] = 1;
			}
	#undef BLACKUNFORMAT
	#undef WHITEUNFORMAT
		cr_col.s = *dp[2] * 2 - 1;
		if(cr_col.s < 1e-2)
		{
			cr_col.h = 0;
			return cr_col;
		}
	#define SATUNFORMAT(c, s) (((c)-0.5f)/(s)+0.5f)
		for(int n = 0; n < 3; ++n)
		{
			*dp[n] = SATUNFORMAT(*dp[n], cr_col.s);
			if(*dp[n] > 1)
				*dp[n] = 1;
		}
	#undef SATUNFORMAT
		cr_col.h = *dp[1];
		if((dp[2] == &r) && (dp[1] == &g)) cr_col.h = cr_col.h + 0;
		else if((dp[2] == &g) && (dp[1] == &b))
			cr_col.h = cr_col.h + 2;
		else if((dp[2] == &b) && (dp[1] == &r))
			cr_col.h = cr_col.h + 4;
		else if((dp[2] == &g) && (dp[1] == &r))
			cr_col.h = 2 - cr_col.h;
		else if((dp[2] == &b) && (dp[1] == &g))
			cr_col.h = 4 - cr_col.h;
		else if((dp[2] == &r) && (dp[1] == &b))
			cr_col.h = 6 - cr_col.h;
		cr_col.h /= 6;
		return cr_col;
	}(rgb);

	*H = hsl.h * 360.0f;
	*S = hsl.s;
	*L = hsl.l;
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
rgb2hsv(color_t rgb, float* H, float* S, float* V)
{
	const auto
		chsv(RGB_TO_HSV(COLORRGB{EGEGET_R(rgb), EGEGET_G(rgb), EGEGET_B(rgb)}));

	*H = chsv.h * 360.0f;
	*S = chsv.s;
	*V = chsv.v;
}

color_t
hsv2rgb(float H, float S, float V)
{
	if(H < 0.0f)
		H += int(-H / 360.0f + 1) * 360.0f;
	if(H >= 360.0f)
		H -= int(H / 360.0f) * 360.0f;

	const auto crgb(HSV_TO_RGB(COLORHSV{H / 360.0f, S, V}));

	return EGERGB(crgb.r, crgb.g, crgb.b);
}

} // namespace ege;
