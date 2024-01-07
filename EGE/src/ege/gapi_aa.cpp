#include "ege/gapi_aa.h"
#include "ege/gapi.h"
#include "image.h"
#include "global.h"
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>
#include <type_traits> // for std::decay;

namespace ege
{

using std::unique_ptr;

void
ege_enable_aa(bool enable, IMAGE* pimg)
{
	cimg_ref(pimg).m_aa = enable;
}

namespace
{

template<typename _func>
void
ege_aa_impl(_func f, IMAGE& img)
{
	Gdiplus::Graphics graphics(img.getdc());

	graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	if(img.m_aa)
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	f(graphics, img);
}

template<typename _func>
void
ege_draw_impl(_func f, IMAGE* pimg)
{
	ege_aa_impl([=](Gdiplus::Graphics& graphics, IMAGE& img){
		Gdiplus::Pen pen(std::uint32_t(img.m_color), img.m_linewidth);

		f(graphics, pen);
	}, cimg_ref(pimg));
}

template<typename _func>
void
ege_fill_impl(_func f, IMAGE* pimg)
{
	ege_aa_impl([=](Gdiplus::Graphics& graphics, IMAGE& img){
		if(img.m_pattern)
			f(graphics, Deref(img.m_pattern));
		else
		{
			Gdiplus::SolidBrush brush(std::uint32_t(img.m_fillcolor));

			f(graphics, brush);
		}
	}, cimg_ref(pimg));
}

} // unnamed namespace;


void
ege_line(float x1, float y1, float x2, float y2, IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawLine(&pen, x1, y1, x2, y2);
	}, pimg);
}

void
ege_drawpoly(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawLines(&pen, reinterpret_cast<Gdiplus::PointF*>(polypoints),
			numpoints);
	}, pimg);
}

void
ege_drawcurve(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawCurve(&pen, reinterpret_cast<Gdiplus::PointF*>(polypoints),
			numpoints);
	}, pimg);
}

void
ege_rectangle(float x, float y, float w, float h, IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawRectangle(&pen, x, y, w, h);
	}, pimg);
}

void
ege_ellipse(float x, float y, float w, float h, IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawEllipse(&pen, x, y, w, h);
	}, pimg);
}

void
ege_pie(float x, float y, float w, float h, float stangle, float sweepAngle,
	IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawPie(&pen, x, y, w, h, stangle, sweepAngle);
	}, pimg);
}


void
ege_arc(float x, float y, float w, float h, float stangle, float sweepAngle,
	IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawArc(&pen, x, y, w, h, stangle, sweepAngle);
	}, pimg);
}

void
ege_bezier(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	ege_draw_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Pen& pen){
		graphics.DrawBeziers(&pen,
			reinterpret_cast<Gdiplus::PointF*>(polypoints), numpoints);
	}, pimg);
}


void
ege_fillpoly(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	ege_fill_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Brush& brush){
		graphics.FillPolygon(&brush,
			reinterpret_cast<Gdiplus::PointF*>(polypoints), numpoints);
	}, pimg);
}

void
ege_fillrect(float x, float y, float w, float h, IMAGE* pimg)
{
	ege_fill_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Brush& brush){
		graphics.FillRectangle(&brush, x, y, w, h);
	}, pimg);
}

void
ege_fillellipse(float x, float y, float w, float h, IMAGE* pimg)
{
	ege_fill_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Brush& brush){
		graphics.FillEllipse(&brush, x, y, w, h);
	}, pimg);
}

void
ege_fillpie(float x, float y, float w, float h, float stangle, float sweepAngle,
	IMAGE* pimg)
{
	ege_fill_impl([=](Gdiplus::Graphics& graphics, Gdiplus::Brush& brush){
		graphics.FillPie(&brush, x, y, w, h, stangle, sweepAngle);
	}, pimg);
}


void
ege_setpattern_none(IMAGE* pimg)
{
	cimg_ref(pimg).m_pattern.reset();
}

void
ege_setpattern_lineargradient(float x1, float y1, color_t c1, float x2,
	float y2, color_t c2, IMAGE* pimg)
{
	cimg_ref(pimg).m_pattern.reset(new
		Gdiplus::LinearGradientBrush(Gdiplus::PointF(x1, y1),
		Gdiplus::PointF(x2, y2), Gdiplus::Color(c1), Gdiplus::Color(c2)));
}

void
ege_setpattern_pathgradient(ege_point center, color_t centercolor, int count,
	ege_point* points, int colcount, color_t* pointscolor, IMAGE* pimg)
{
	const auto pbrush(new Gdiplus::PathGradientBrush(reinterpret_cast<
		Gdiplus::PointF*>(points), count, Gdiplus::WrapModeTile));

	pbrush->SetCenterColor(Gdiplus::Color(centercolor));
	pbrush->SetCenterPoint(Gdiplus::PointF(center.x, center.y));
	pbrush->SetSurroundColors(
		reinterpret_cast<Gdiplus::Color*>(pointscolor), &colcount);
	cimg_ref(pimg).m_pattern.reset(pbrush);
}

void
ege_setpattern_ellipsegradient(ege_point center, color_t centercolor,
	float x, float y, float w, float h, color_t color, IMAGE* pimg)
{
	Gdiplus::GraphicsPath path;

	path.AddEllipse(x, y, w, h);

	const auto pbrush = new Gdiplus::PathGradientBrush(&path);
	Gdiplus::Color c(color);
	int count = 1;

	pbrush->SetCenterColor(Gdiplus::Color(centercolor));
	pbrush->SetCenterPoint(Gdiplus::PointF(center.x, center.y));
	pbrush->SetSurroundColors(&c, &count);
	cimg_ref(pimg).m_pattern.reset(pbrush);
}

void
ege_setpattern_texture(const IMAGE* srcimg, float x, float y, float w, float h,
	IMAGE* pimg)
{
	if(srcimg->m_texture)
		cimg_ref(pimg).m_pattern.reset(new Gdiplus::TextureBrush(
			srcimg->m_texture.get(), Gdiplus::WrapModeTile, x, y, w, h));
}


void
ege_setalpha(int alpha, IMAGE* pimg)
{
	auto& img(cimg_ref(pimg));

	if(img.getdc())
	{
		const auto w(img.GetWidth()), h(img.GetHeight());
		const auto buf(img.getbuffer());
		typename std::decay<decltype(*buf)>::type
			a(static_cast<unsigned long>(alpha) << 24);

		yassume(buf);
		for(typename std::decay<decltype(h)>::type y(0); y < h; ++y)
			for(typename std::decay<decltype(w)>::type x(0); x < w; ++x)
			{
				auto& cref(buf[y * w + x]);

				cref = a | (cref & 0xFFFFFF);
			}
	}
}

void
ege_gentexture(bool gen, IMAGE* pimg)
{
	cimg_ref(pimg).gentexture(gen);
}

void
ege_puttexture(const IMAGE* srcimg, float x, float y, float w, float h,
	IMAGE* pimg)
{
	ege_rect dest{x, y, w, h};

	ege_puttexture(srcimg, dest, pimg);
}
void
ege_puttexture(const IMAGE* srcimg, ege_rect dest, IMAGE* pimg)
{
	ege_rect src;

	src.x = 0;
	src.y = 0;
	src.w = srcimg->GetWidth();
	src.h = srcimg->GetHeight();
	ege_puttexture(srcimg, dest, src, &cimg_ref_c(pimg));
}
void
ege_puttexture(const IMAGE* srcimg, ege_rect dest, ege_rect src, IMAGE* pimg)
{
	auto& img(cimg_ref(pimg));

	if(Deref(srcimg).m_texture)
	{
		Gdiplus::Graphics graphics(img.getdc());

		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img.m_aa)
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
#if 0
		Gdiplus::ImageAttributes ia;
		Gdiplus::ColorMatrix mx{
			{
				{1.0, 0.0, 0.0, 0.0, 0.0},
				{0.0, 1.0, 0.0, 0.0, 0.0},
				{0.0, 0.0, 1.0, 0.0, 0.0},
				{0.0, 0.0, 0.0, 1.0, 0.0},
				{0.0, 0.0, 0.0, 0.0, 1.0},
			}
		};
		ia.SetColorMatrix(&mx);
		//graphics.SetTransform();
#endif
		graphics.DrawImage(srcimg->m_texture.get(),
			Gdiplus::RectF(dest.x, dest.y, dest.w, dest.h),
			src.x, src.y, src.w, src.h, Gdiplus::UnitPixel, {});
	}
}

} // namespace ege;

