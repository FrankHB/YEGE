#include "ege/gapi_aa.h"
#include "image.h"
#include "global.h"
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>

namespace ege
{

void
ege_enable_aa(bool enable, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	img->m_aa = enable;
}


void
ege_line(float x1, float y1, float x2, float y2, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawLine(&pen, x1, y1, x2, y2);
	}
}

void
ege_drawpoly(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawLines(&pen, (Gdiplus::PointF*)polypoints, numpoints);
	}
}

void
ege_drawcurve(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawCurve(&pen, (Gdiplus::PointF*)polypoints, numpoints);
	}
}

void
ege_rectangle(float x, float y, float w, float h, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawRectangle(&pen, x, y, w, h);
	}
}

void
ege_ellipse(float x, float y, float w, float h, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawEllipse(&pen, x, y, w, h);
	}
}

void
ege_pie(float x, float y, float w, float h, float stangle, float sweepAngle, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawPie(&pen, x, y, w, h, stangle, sweepAngle);
	}
}


void
ege_arc(float x, float y, float w, float h, float stangle, float sweepAngle, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawArc(&pen, x, y, w, h, stangle, sweepAngle);
	}
}

void
ege_bezier(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		Gdiplus::Pen pen(img->m_color, img->m_linewidth);
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		graphics.DrawBeziers(&pen, (Gdiplus::PointF*)polypoints, numpoints);
	}
}


void
ege_fillpoly(int numpoints, ege_point* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		if(img->m_pattern_obj)
		{
			graphics.FillPolygon((Gdiplus::Brush*)img->m_pattern_obj, (Gdiplus::PointF*)polypoints, numpoints);
		}
		else
		{
			Gdiplus::SolidBrush brush(img->m_fillcolor);
			graphics.FillPolygon(&brush, (Gdiplus::PointF*)polypoints, numpoints);
		}
	}
}

void
ege_fillrect(float x, float y, float w, float h, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		if(img->m_pattern_obj)
		{
			graphics.FillRectangle((Gdiplus::Brush*)img->m_pattern_obj, x, y, w, h);
		}
		else
		{
			Gdiplus::SolidBrush brush(img->m_fillcolor);
			graphics.FillRectangle(&brush, x, y, w, h);
		}
	}
}

void
ege_fillellipse(float x, float y, float w, float h, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		if(img->m_pattern_obj)
		{
			graphics.FillEllipse((Gdiplus::Brush*)img->m_pattern_obj, x, y, w, h);
		}
		else
		{
			Gdiplus::SolidBrush brush(img->m_fillcolor);
			graphics.FillEllipse(&brush, x, y, w, h);
		}
	}
}

void
ege_fillpie(float x, float y, float w, float h, float stangle, float sweepAngle, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::Graphics graphics(img->getdc());
		graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
		if(img->m_aa)
		{
			graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		}
		if(img->m_pattern_obj)
		{
			graphics.FillPie((Gdiplus::Brush*)img->m_pattern_obj, x, y, w, h, stangle, sweepAngle);
		}
		else
		{
			Gdiplus::SolidBrush brush(img->m_fillcolor);
			graphics.FillPie(&brush, x, y, w, h, stangle, sweepAngle);
		}
	}
}


void
ege_setpattern_none(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		img->delete_pattern();
	}
}

void
ege_setpattern_lineargradient(float x1, float y1, color_t c1, float x2, float y2, color_t c2, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::LinearGradientBrush* pbrush = new Gdiplus::LinearGradientBrush(
			Gdiplus::PointF(x1, y1),
			Gdiplus::PointF(x2, y2),
			Gdiplus::Color(c1),
			Gdiplus::Color(c2)
		);
		img->set_pattern(pbrush, pattern_lineargradient);
	}
}

void
ege_setpattern_pathgradient(ege_point center, color_t centercolor,
							int count, ege_point* points, int colcount, color_t* pointscolor, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::PathGradientBrush* pbrush = new Gdiplus::PathGradientBrush(
			(Gdiplus::PointF*)points,
			count,
			Gdiplus::WrapModeTile
		);
		pbrush->SetCenterColor(Gdiplus::Color(centercolor));
		pbrush->SetCenterPoint(Gdiplus::PointF(center.x, center.y));
		pbrush->SetSurroundColors((Gdiplus::Color*)pointscolor, &colcount);
		img->set_pattern(pbrush, pattern_pathgradient);
	}
}

void
ege_setpattern_ellipsegradient(ege_point center, color_t centercolor,
	float x, float y, float w, float h, color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Gdiplus::GraphicsPath path;
		path.AddEllipse(x, y, w, h);
		Gdiplus::PathGradientBrush* pbrush = new Gdiplus::PathGradientBrush(
			&path
		);
		int count = 1;
		pbrush->SetCenterColor(Gdiplus::Color(centercolor));
		pbrush->SetCenterPoint(Gdiplus::PointF(center.x, center.y));
		pbrush->SetSurroundColors((Gdiplus::Color*)&color, &count);
		img->set_pattern(pbrush, pattern_pathgradient);
	}
}

void
ege_setpattern_texture(IMAGE* srcimg, float x, float y, float w, float h, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		if(srcimg->m_texture)
		{
			Gdiplus::TextureBrush* pbrush = new Gdiplus::TextureBrush(
				(Gdiplus::Image*)srcimg->m_texture,
				Gdiplus::WrapModeTile,
				x, y, w, h);
			img->set_pattern(pbrush, pattern_texture);
		}
	}
}


void
ege_setalpha(int alpha, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		int a = alpha << 24;
		int w = pimg->getwidth();
		int h = pimg->getheight();
		for(int y = 0; y < h; ++y)
		{
			for(int x = 0; x < w; ++x)
			{
				int c;
				c = getpixel_f(x, y, img);
				c = a | (c & 0xFFFFFF);
				putpixel_f(x, y, c, img);
			}
		}
	}
}

void
ege_gentexture(bool gen, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		img->gentexture(gen);
	}
}

void
ege_puttexture(IMAGE* srcimg, float x, float y, float w, float h, IMAGE* pimg)
{
	ege_rect dest{x, y, w, h};
	ege_puttexture(srcimg, dest, pimg);
}
void
ege_puttexture(IMAGE* srcimg, ege_rect dest, IMAGE* pimg)
{
	ege_rect src;
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
	{
		src.x = 0;
		src.y = 0;
		src.w = (float)srcimg->getwidth();
		src.h = (float)srcimg->getheight();
		ege_puttexture(srcimg, dest, src, img);
	}
}
void
ege_puttexture(IMAGE* srcimg, ege_rect dest, ege_rect src, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		if(srcimg->m_texture)
		{
			Gdiplus::Graphics graphics(img->getdc());
			graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
			if(img->m_aa)
			{
				graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
			}
			/*
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
			// */
			//graphics.SetTransform();
			graphics.DrawImage((Gdiplus::Image*)srcimg->m_texture,
				Gdiplus::RectF(dest.x, dest.y, dest.w, dest.h),
				src.x,
				src.y,
				src.w,
				src.h,
				Gdiplus::UnitPixel,
				{}
			);
		}
	}
}

} // namespace ege;

