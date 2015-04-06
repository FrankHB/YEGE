#include "ege/color.h"
#include "head.h"
#if !YEGE_Use_YSLib
#	include <cmath>
#	include <algorithm> // for std::min, std::max;
#endif

namespace ege
{

#if !YEGE_Use_YSLib
namespace
{

std::array<float, 3>
rgb2hsl_impl(float r, float g, float b)
{
#define IFSWAP(a, b) if(*a>*b){t=a;a=b;b=t;}

	float* t, *dp[3]{&r, &g, &b};

	IFSWAP(dp[0], dp[1]);
	IFSWAP(dp[1], dp[2]);
	IFSWAP(dp[0], dp[1]);
#undef IFSWAP

	float rh, rs, rl;

	rl = (*dp[0] + *dp[2]) / 2;
	if(rl < 1e-2f)
		return {0, 0, 0};
	if(rl > 0.99)
		return {0, 0, 1};
	if(fabs(rl - 0.5) < 1e-2)
		rl = 0.5;
#define BLACKUNFORMAT(c, v) ((c)/((v)*2))
#define WHITEUNFORMAT(c, v) (1-(1-c)/((1-(v))*2))
	if(rl == 0.5)
		;
	else if(rl < 0.5)
	{
		for(int n = 0; n < 3; ++n)
		{
			*dp[n] = BLACKUNFORMAT(*dp[n], rl);
			if(*dp[n] > 1)
				*dp[n] = 1;
		}
	}
	else
	{
		for(int n = 0; n < 3; ++n)
		{
			*dp[n] = WHITEUNFORMAT(*dp[n], rl);
			if(*dp[n] > 1)
				*dp[n] = 1;
		}
	}
#undef BLACKUNFORMAT
#undef WHITEUNFORMAT
	rs = *dp[2] * 2 - 1;
	if(rs < 1e-2)
		return {0, rs, rl};
#define SATUNFORMAT(c, s) (((c)-.5F)/(s)+.5F)
	for(int n = 0; n < 3; ++n)
	{
		*dp[n] = SATUNFORMAT(*dp[n], rs);
		if(*dp[n] > 1)
			*dp[n] = 1;
	}
#undef SATUNFORMAT
	rh = *dp[1];
	if((dp[2] == &r) && (dp[1] == &g))
		;
	else if((dp[2] == &g) && (dp[1] == &b))
		rh += 2;
	else if((dp[2] == &b) && (dp[1] == &r))
		rh += 4;
	else if((dp[2] == &g) && (dp[1] == &r))
		rh = 2 - rh;
	else if((dp[2] == &b) && (dp[1] == &g))
		rh = 4 - rh;
	else if((dp[2] == &r) && (dp[1] == &b))
		rh = 6 - rh;
	return {rh / 6, rs, rl};
}

std::array<float, 3>
RGB_TO_HSV(mono_t ir, mono_t ig, mono_t ib)
{
	float r(ir / 255.F), g(ig / 255.F), b(ib / 255.F),
		minRGB, maxRGB, deltaRGB, rh(0), rs, rv;

	minRGB = std::min(r, std::min(g, b));
	maxRGB = std::max(r, std::max(g, b));
	deltaRGB = maxRGB - minRGB;
	rv = maxRGB;
	if(maxRGB != 0.F)
		rs = deltaRGB / maxRGB;
	else
		rs = 0.F;
	if(rs <= 0.F)
		rh = -1.F;
	else
	{
		if(r == maxRGB)
			rh = (g - b) / deltaRGB;
		else if(g == maxRGB)
			rh = 2.0f + (b - r) / deltaRGB;
		else if(b == maxRGB)
			rh = 4.0f + (r - g) / deltaRGB;
		rh *= 60.F;
		if(rh < 0.F)
			rh += 360.F;
		rh /= 360.F;
	}
	return {rh, rs, rv};
}

std::array<mono_t, 3>
HSV_TO_RGB(float ih, float is, float iv)
{
	float R = 0, G = 0, B = 0;
	int k;
	float aa, bb, cc, f;
	if(is <= 0.0)
		R = G = B = iv;
	else
	{
		if(ih == 1.0)
			ih = 0.0;
		ih *= 6.0;
		k = int(floor(ih)); // ??
		f = ih - k;
		aa = iv * (1.F - is);
		bb = iv * (1.F - is * f);
		cc = iv * (1.F - (is * (1.F - f)));
		switch(k)
		{
		case 0:
			R = iv;
			G = cc;
			B = aa;
			break;
		case 1:
			R = bb;
			G = iv;
			B = aa;
			break;
		case 2:
			R = aa;
			G = iv;
			B = cc;
			break;
		case 3:
			R = aa;
			G = bb;
			B = iv;
			break;
		case 4:
			R = cc;
			G = aa;
			B = iv;
			break;
		case 5:
			R = iv;
			G = aa;
			B = bb;
			break;
		}
	}
	return {mono_t(R * 255), mono_t(G * 255), mono_t(B * 255)};
}

} // unnamed namespace;
#endif

color_t
rgb2gray(color_t color)
{
#if YEGE_Use_YSLib
	return MakeGray(color_t::Trait::IntegerType(((color >> 16) & 0xFF) * 0.299
		+ ((color >> 8) & 0xFF) * 0.587 + ((color) & 0xFF) * 0.114));
#else
	double c;
	color_t r;

	c = ((color >> 16) & 0xFF) * 0.299;
	c += ((color >> 8) & 0xFF) * 0.587;
	c += (color & 0xFF) * 0.114;
	r = color_t(c);
	return EGERGB(r, r, r);
#endif
}

void
rgb2hsl(color_t rgb, float* h, float* s, float* l)
{
#if YEGE_Use_YSLib
	HSL hsl{Color(rgb)};

	yunseq(Deref(h) = hsl.GetH(), Deref(s) = hsl.GetS(), Deref(l) = hsl.GetL());
#else
	const auto hsl(rgb2hsl_impl(EGEGET_R(rgb) / 255.F, EGEGET_G(rgb) / 255.F,
		EGEGET_B(rgb) / 255.F));

	yunseq(Deref(h) = hsl[0] * 360.F, Deref(s) = hsl[1], Deref(l) = hsl[2]);
#endif
}

color_t
hsl2rgb(float h, float s, float l)
{
#if YEGE_Use_YSLib
	return Color(HSL(h, s, l));
#else
	float r, g, b;

	h /= 360.F;
	if(h < 0.F)
		h += float(int((-h + 1)));
	if(h >= 1.F)
		h -= float(int((h)));
	if(s == 0)
	{
		r = l;
		g = l;
		b = l;
	}
	else
	{
		float xh = h == 1 ? 0 : h * 6;
		int i = int(floor(xh) + 0.1), n;
		float dp[3];

		dp[0] = 1;
		dp[2] = 0;
		xh -= i;
		if(i & 1)
			dp[1] = 1 - xh;
		else
			dp[1] = xh;
		for(n = 0; n < 3; ++n)
			dp[n] = (dp[n] - .5F) * s + .5F;
		if(l == .5F)
			;
		else if(l < .5F)
		{
			if(l < 0)
				l = 0;
			for(n = 0; n < 3; ++n)
				dp[n] = dp[n] * l * 2;
		}
		else
		{
			if(l > 1)
				l = 1;
			for(n = 0; n < 3; ++n)
				dp[n] = 1- (1 - dp[n]) * (1 - l) * 2;
		}
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
	return EGERGB(static_cast<color_int_t>(r * 255),
		static_cast<color_int_t>(g * 255),
		static_cast<color_int_t>(b * 255));
#endif
}

void
rgb2hsv(color_t rgb, float* h, float* s, float* v)
{
#if YEGE_Use_YSLib
	HSV hsv{Color(rgb)};

	yunseq(Deref(h) = hsv.GetH(), Deref(s) = hsv.GetS(), Deref(v) = hsv.GetV());
#else
	const auto chsv(RGB_TO_HSV(mono_t(EGEGET_R(rgb)), mono_t(EGEGET_G(rgb)),
		mono_t(EGEGET_B(rgb))));

	yunseq(Deref(h) = chsv[0] * 360.F, Deref(s) = chsv[1], Deref(v) = chsv[2]);
#endif
}

color_t
hsv2rgb(float h, float s, float v)
{
#if YEGE_Use_YSLib
	return Color(HSV(h, s, v));
#else
	if(h < 0.F)
		h += float(int((-h / 360.F + 1) * 360.F));
	if(h >= 360.F)
		h -= float(int((h / 360.F) * 360.F));

	const auto crgb(HSV_TO_RGB(h / 360.F, s, v));

	return EGERGB(crgb[0], crgb[1], crgb[2]);
#endif
}

} // namespace ege;

