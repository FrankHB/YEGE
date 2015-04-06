#include "ege/gapi.h"
#include "global.h"
#include "image.h"
#include <utility> // for std::swap;
#include <cfloat> // for FLT_EPSILON;
#include <cmath> // for std::cos;
#include "ege.h"

namespace ege
{

namespace
{

//此函数调用前，已经有Lock
int
_save_brush(IMAGE* img, int save)
{
	static ::HBRUSH savebrush_hbr;

	yconstraint(img);

	if(save)
	{
		::LOGBRUSH lbr{0, COLORREF(), 0U};

		lbr.lbColor = 0;
		lbr.lbStyle = BS_NULL;
		savebrush_hbr = ::CreateBrushIndirect(&lbr);
		if(savebrush_hbr)
		{
			savebrush_hbr = ::HBRUSH(::SelectObject(img->getdc(),
				savebrush_hbr));
			return 1;
		}
	}
	else if(savebrush_hbr)
	{
		::DeleteObject(::HBRUSH(::SelectObject(img->getdc(), savebrush_hbr)));
		savebrush_hbr = {};
	}
	return 0;
}

} // unnamed namespace;


IMAGE*
gettarget()
{
	return get_pages().get_target();
}

#if 0
void
swappage()
{
	auto pg = &FetchEGEApplication();
	setvisualpage(pg->active_page);
	setactivepage(1 - pg->active_page);
}
#endif

void
setactivepage(int page)
{
	get_pages().set_apage(page);
}

void
setvisualpage(int page)
{
	get_pages().set_vpage(page);
}


int
settarget(IMAGE* pbuf)
{
	return get_pages().set_target(pbuf);
}

void
cleardevice(IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		if(img->getdc())
		{
			color_t c = getbkcolor(img);

			for(color_t* p = reinterpret_cast<color_t*>(img->getbuffer()),
				*e = reinterpret_cast<color_t*>(&img->getbuffer()[
					img->GetWidth() * img->GetHeight()]); p != e; ++p)
				*p = c;
		}
}


void
getlinestyle(int* plinestyle, unsigned short* pupattern, int* pthickness,
	IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	if(plinestyle)
		*plinestyle = img.m_linestyle.linestyle;
	if(pupattern)
		*pupattern = img.m_linestyle.upattern;
	if(pthickness)
		*pthickness = img.m_linestyle.thickness;
}

void
setlinestyle(int linestyle, unsigned short upattern, int thickness, IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	::LOGPEN lpen{0, ::POINT(), COLORREF()};

	lpen.lopnColor = getcolor(pimg);
	img.m_linestyle.thickness = thickness;
	img.m_linewidth = float(thickness);
	img.m_linestyle.linestyle = linestyle;
	img.m_linestyle.upattern = upattern;
	lpen.lopnWidth.x = thickness;
	lpen.lopnStyle   = linestyle;

	::HPEN hpen;

	if(linestyle == PS_USERSTYLE)
	{
		unsigned long style[20]{0};
		::LOGBRUSH lbr;
		int n, bn = 0, len = 1, st = 0;
		lbr.lbColor = lpen.lopnColor;
		lbr.lbStyle = BS_SOLID;
		lbr.lbHatch = 0;
		st = upattern & 1;
		for(n = 1; n < 16; n++)
		{
			if(upattern & (1 << n))
			{
				if(st == 0)
				{
					st = 1;
					style[bn++] = len;
					len = 1;
				}
				else
					++len;
			}
			else
			{
				if(st == 0)
					++len;
				else
				{
					st = 0;
					style[bn++] = len;
					len = 1;
				}
			}
		}
		hpen = ExtCreatePen(PS_GEOMETRIC, thickness, &lbr, bn, style);
	}
	else
		hpen = ::CreatePenIndirect(&lpen);
	if(hpen)
		::DeleteObject(::SelectObject(img.getdc(), hpen));
}

void
setlinewidth(float width, IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	img.m_linestyle.thickness = int(width);
	img.m_linewidth = width;
}

void
setfillstyle(int pattern, color_t color, IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	::LOGBRUSH lbr{0, COLORREF(), ::UINT_PTR()};

	img.m_fillcolor = color;
	lbr.lbColor = color;
	//::SetBkColor(img.getdc(), color);
	if(pattern < SOLID_FILL)
		lbr.lbHatch = BS_NULL;
	else if(pattern == SOLID_FILL)
		lbr.lbHatch = BS_SOLID;
	else if(pattern < USER_FILL)      // dose not finish
	{
		int hatchmap[] =
		{
			HS_VERTICAL,
			HS_BDIAGONAL,
			HS_BDIAGONAL,
			HS_FDIAGONAL,
			HS_FDIAGONAL,
			HS_CROSS,
			HS_DIAGCROSS,
			HS_VERTICAL,
			HS_DIAGCROSS,
			HS_DIAGCROSS
		};
		lbr.lbStyle = BS_HATCHED;
		lbr.lbHatch = hatchmap[pattern - 2];
	}
	else
		lbr.lbHatch = BS_SOLID;
	if(::HBRUSH hbr = ::CreateBrushIndirect(&lbr))
		::DeleteObject(::SelectObject(img.getdc(), hbr));
}

void
setwritemode(int mode, IMAGE* pimg)
{
	::SetROP2(convert_image_ref_c(pimg).getdc(), mode);
}


color_t
getcolor(IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		if(img->getdc())
		{
			return img->m_color;
			/*
			::HPEN hpen_c = (::HPEN)::GetCurrentObject(img->getdc(), OBJ_PEN);
			::LOGPEN logPen;
			GetObject(hpen_c, sizeof(logPen), &logPen);
			return logPen.lopnColor;
			// */
		}
	return 0xFFFFFFFF;
}

color_t
getfillcolor(IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		if(img->getdc())
			return img->m_fillcolor;
	return 0xFFFFFFFF;
}

color_t
getbkcolor(IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		return img->m_bk_color;
	return 0xFFFFFFFF;
}


void
setcolor(color_t color, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		if(img->getdc())
		{
			::LOGPEN lPen;
			::HPEN hpen;

			img->m_color = color;
			lPen.lopnColor = color;
			lPen.lopnStyle = img->m_linestyle.linestyle;
			lPen.lopnWidth.x = img->m_linestyle.thickness;
			::SetTextColor(img->getdc(), color);
			if(lPen.lopnStyle == PS_USERSTYLE)
			{
				unsigned long style[20]{};
				::LOGBRUSH lbr;
				unsigned short upattern = img->m_linestyle.upattern;
				int n, bn = 0, len = 1, st = 0;

				lbr.lbColor = lPen.lopnColor;
				lbr.lbStyle = BS_SOLID;
				lbr.lbHatch = 0;
				st = upattern & 1;
				for(n = 1; n < 16; n++)
					if(upattern & (1 << n))
					{
						if(st == 0)
						{
							st = 1;
							style[bn++] = len;
							len = 1;
						}
						else
							++len;
					}
					else
					{
						if(st == 0)
							++len;
						else
						{
							st = 0;
							style[bn++] = len;
							len = 1;
						}
					}
				hpen = ::ExtCreatePen(PS_GEOMETRIC, img->m_linestyle.thickness,
					&lbr, bn, style);
			}
			else
				hpen = ::CreatePenIndirect(&lPen);
			if(hpen)
				::DeleteObject(::SelectObject(img->getdc(), hpen));
		}
}

void
setfillcolor(color_t color, IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	::LOGBRUSH lbr{0, COLORREF(), ::ULONG_PTR()};

	img.m_fillcolor = color;
	lbr.lbColor = color;
	lbr.lbHatch = BS_SOLID;
	::HBRUSH hbr = ::CreateBrushIndirect(&lbr);
	if(hbr)
		::DeleteObject(::SelectObject(img.getdc(), hbr));
}

void
setbkcolor(color_t color, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		if(img->getdc())
		{
			unsigned long* p = img->getbuffer();
			size_t size = img->GetWidth() * img->GetHeight();
			color_t col = img->m_bk_color;

			img->m_bk_color = color;
			::SetBkColor(img->getdc(), color);
			for(size_t n = 0; n < size; ++n, ++p)
				if(*p == col)
					*p = color;
		}
}



void
setbkcolor_f(color_t color, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		if(img->getdc())
		{
			img->m_bk_color = color;
			::SetBkColor(img->getdc(), color);
		}
}

void
setfontbkcolor(color_t color, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		if(img->getdc())
			::SetBkColor(img->getdc(), color);
}

void
setbkmode(int iBkMode, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		if(img->getdc())
			::SetBkMode(img->getdc(), iBkMode);
}


color_t
getpixel(int x, int y, IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	x += img.m_vpt.left;
	y += img.m_vpt.top;
	return color_t(x < 0 || y < 0 || x >= int(img.GetWidth())
		|| y >= int(img.GetHeight()) ? 0
		: img.getbuffer()[y * img.GetWidth() + x]);
}

void
putpixel(int x, int y, color_t color, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	x += img.m_vpt.left;
	y += img.m_vpt.top;
	if(!(x < 0 || y < 0 || x >= img.m_vpt.right || y >= img.m_vpt.bottom))
		img.getbuffer()[y * img.GetWidth() + x] = color;
}

color_t
getpixel_f(int x, int y, IMAGE* pimg)
{
	auto& img(convert_image_ref_c(pimg));

	return img.getbuffer()[y * img.GetWidth() + x];
}

void
putpixel_f(int x, int y, color_t color, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	img.getbuffer()[y * img.GetWidth() + x] = color;
}

void
putpixels(int nPoint, int* pPoints, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	int x, y, c;
	unsigned long* pb
		= &img.getbuffer()[img.m_vpt.top * img.GetWidth() + img.m_vpt.left];
	int w = img.m_vpt.right - img.m_vpt.left, h = img.m_vpt.bottom
		- img.m_vpt.top;
	int tw = img.GetWidth();

	for(int n = 0; n < nPoint; ++n, pPoints += 3)
	{
		x = pPoints[0], y = pPoints[1], c = pPoints[2];
		if(!(x < 0 || y < 0 || x >= w || y >= h))
			pb[y * tw + x] = c;
	}
}

void
putpixels_f(int nPoint, int* pPoints, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));
	const int tw(img.GetWidth());

	for(int n = 0; n < nPoint; ++n, pPoints += 3)
		img.getbuffer()[pPoints[1] * tw + pPoints[0]] = pPoints[2];
}


void
moveto(int x, int y, IMAGE* pimg)
{
	MoveToEx(convert_image_ref(pimg).getdc(), x, y, {});
}

void
moverel(int dx, int dy, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	::POINT pt;
	::GetCurrentPositionEx(img.getdc(), &pt);
	dx += pt.x;
	dy += pt.y;
	MoveToEx(img.getdc(), dx, dy, {});
}


void
line(int x1, int y1, int x2, int y2, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	::MoveToEx(img.getdc(), x1, y1, {});
	::LineTo(img.getdc(), x2, y2);
}

void
linerel(int dx, int dy, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	::POINT pt;
	::GetCurrentPositionEx(img.getdc(), &pt);
	dx += pt.x;
	dy += pt.y;
	LineTo(img.getdc(), dx, dy);
}

void
lineto(int x, int y, IMAGE* pimg)
{
	LineTo(convert_image_ref(pimg).getdc(), x, y);
}

namespace
{

void
line_base(float x1, float y1, float x2, float y2, IMAGE* img)
{
	int bswap = 2;
	auto col(getcolor(Nonnull(img)));
	color_t endp(0);
	auto pBuffer(reinterpret_cast<color_t*>(img->getbuffer()));
	int rw = img->GetWidth();

	if(x1 > x2)
	{
		std::swap(x1, x2),
		std::swap(y1, y2);
		if(bswap)
			bswap ^= 3;
	}
	if(x2 < img->m_vpt.left || x1 > img->m_vpt.right)
		return;
	if(x1 < img->m_vpt.left)
	{
		if(x2 - x1 < FLT_EPSILON)
			return;
		float d = (x2 - img->m_vpt.left) / (x2 - x1);
		y1 = (y1 - y2) * d + y2;
		x1 = float(img->m_vpt.left);
		if(bswap == 1)
			bswap = 0;
	}
	if(x2 > img->m_vpt.right)
	{
		if(x2 - x1 < FLT_EPSILON) return;
		float d = (img->m_vpt.right - x1) / (x2 - x1);
		y2 = (y2 - y1) * d + y1;
		x2 = float(img->m_vpt.right);
		if(bswap == 2)
			bswap = 0;
	}
	if(y1 > y2)
	{
		std::swap(x1, x2),
		std::swap(y1, y2);
		if(bswap)
			bswap ^= 3;
	}
	if(y2 < img->m_vpt.top)
		return;
	if(y1 > img->m_vpt.bottom)
		return;
	if(y1 < img->m_vpt.top)
	{
		if(y2 - y1 < FLT_EPSILON)
			return;

		float d = (y2 - img->m_vpt.top) / (y2 - y1);

		x1 = (x1 - x2) * d + x2;
		y1 = float(img->m_vpt.top);
		if(bswap == 1)
			bswap = 0;
	}
	if(y2 > img->m_vpt.bottom)
	{
		if(y2 - y1 < FLT_EPSILON)
			return;

		float d = (img->m_vpt.bottom - y1) / (y2 - y1);

		x2 = (x2 - x1) * d + x1;
		y2 = float(img->m_vpt.bottom);
		if(bswap == 2)
			bswap = 0;
	}
	if(bswap)
		endp = pBuffer[bswap == 1 ? int(y1) * rw + int(x1)
			: int(y2) * rw + int(x2)];
	if(y2 - y1 > fabs(x2 - x1))
	{
		int y(y1 + 0.9f);
		int ye(y2);
		float x, dx;

		if(y < y1)
			++y;
		dx = (x2 - x1) / (y2 - y1);
		x = (y - y1) * dx + x1 + 0.5f;
		if(ye >= img->m_vpt.bottom)
			ye = img->m_vpt.bottom - 1;
		if(ye < y2)
			bswap = 0;
		for(; y <= ye; ++y, x += dx)
			pBuffer[y * rw + int(x)] = col;
	}
	else
	{
		if(x1 > x2)
		{
			std::swap(x1, x2),
			std::swap(y1, y2);
			if(bswap)
				bswap ^= 3;
		}

		int x(x1 + 0.9f);
		int xe(x2);
		float y, dy;

		if(x < x1)
			++x;
		dy = (y2 - y1) / (x2 - x1);
		y = (x - x1) * dy + y1 + 0.5f;
		if(xe >= img->m_vpt.right)
			xe = img->m_vpt.right - 1;
		if(xe < x2)
			bswap = 0;
		for(; x <= xe; ++x, y += dy)
			pBuffer[int(y) * rw + x] = col;
	}
	if(bswap)
		pBuffer[bswap == 1 ? int(y1) * rw + int(x1)
			: int(y2) * rw + int(x2)] = endp;
}

} // unnamed namespace;

void
line_f(float x1, float y1, float x2, float y2, IMAGE* pimg)
{
	line_base(x1, y1, x2, y2, &convert_image_ref(pimg));
}

void
linerel_f(float dx, float dy, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	::POINT pt;

	::GetCurrentPositionEx(img.getdc(), &pt);
	line_base(float(pt.x), float(pt.y), float(pt.x) + dx, float(pt.y) + dy,
		&img);
}

void
lineto_f(float x, float y, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	::POINT pt;

	::GetCurrentPositionEx(img.getdc(), &pt);
	line_base(float(pt.x), float(pt.y), x, y, &img);
}


void rectangle(int left, int top, int right, int bottom, IMAGE* pimg)
{
	auto& img(convert_image_ref(pimg));

	if(_save_brush(&img, 1))
	{
		Rectangle(img.getdc(), left, top, right, bottom);
		_save_brush(&img, 0);
	}
}


void
arc(int x, int y, int stangle, int endangle, int radius, IMAGE* pimg)
{
	ellipse(x, y, stangle, endangle, radius, radius, pimg);
}

void
circle(int x, int y, int radius, IMAGE* pimg)
{
	ellipse(x, y, 0, 360, radius, radius, pimg);
}

void
pieslice(int x, int y, int stangle, int endangle, int radius, IMAGE* pimg)
{
	sector(x, y, stangle, endangle, radius, radius, pimg);
}

void
ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		const auto sr(stangle / 180.0 * PI), er(endangle / 180.0 * PI);

		::Arc(img->getdc(), x - xradius, y - yradius, x + xradius, y + yradius,
			x + xradius * std::cos(sr), y - yradius * std::sin(sr),
			x + xradius * std::cos(er), y - yradius * std::sin(er));
	}
}

void
fillellipse(int x, int y, int xradius, int yradius, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		::Ellipse(img->getdc(), x - xradius, y - yradius, x + xradius,
			y + yradius);
}

void
sector(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		const auto sr(stangle / 180.0 * PI), er(endangle / 180.0 * PI);
		::Pie(img->getdc(), x - xradius, y - yradius, x + xradius, y + yradius,
			x + xradius * std::cos(sr), y - yradius * std::sin(sr),
			x + xradius * std::cos(er), y - yradius * std::sin(er));
	}
}


void
arcf(float x, float y, float stangle, float endangle, float radius, IMAGE* pimg)
{
	ellipsef(x, y, stangle, endangle, radius, radius, pimg);
}

void
circlef(float x, float y, float radius, IMAGE* pimg)
{
	ellipsef(x, y, 0.0f, 360.0f, radius, radius, pimg);
}

void
pieslicef(float x, float y, float stangle, float endangle, float radius,
	IMAGE* pimg)
{
	sectorf(x, y, stangle, endangle, radius, radius, pimg);
}

void
ellipsef(float x, float y, float stangle, float endangle, float xradius,
	float yradius, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		const auto sr(stangle / 180.0 * PI), er(endangle / 180.0 * PI);

		::Arc(img->getdc(), x - xradius, y - yradius, x + xradius, y + yradius,
			x + xradius * std::cos(sr), y - yradius * std::sin(sr),
			x + xradius * std::cos(er), y - yradius * std::sin(er));
	}
}

void
fillellipsef(float x, float y, float xradius, float yradius, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		::Ellipse(img->getdc(), x - xradius, y - yradius, x + xradius,
			y + yradius);
}

void
sectorf(float x, float y, float stangle, float endangle, float xradius,
	float yradius, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		const auto sr(stangle / 180.0 * PI), er(endangle / 180.0 * PI);

		::Pie(img->getdc(), x - xradius, y - yradius, x + xradius, y + yradius,
			x + xradius * std::cos(sr), y - yradius * std::sin(sr),
			x + xradius * std::cos(er), y - yradius * std::sin(er));
	}
}


void
bar(int left, int top, int right, int bottom, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		::RECT rect{left, top, right, bottom};

		::FillRect(img->getdc(), &rect,
			::HBRUSH(::GetCurrentObject(img->getdc(), OBJ_BRUSH)));
	}
}

void
bar3d(int x1, int y1, int x2, int y2, int depth, int topflag, IMAGE* pimg)
{
	--x2;
	--y2;

	const int pt[20]{
		x2, y2, x2, y1, x1, y1, x1, y2, x2, y2,
		x2 + depth, y2 - depth, x2 + depth, y1 - depth, x1 + depth, y1 - depth,
		x1, y1,
	};

	bar(x1, y1, x2, y2, pimg);
	if(topflag)
	{
		drawpoly(9, pt, pimg);
		line(x2, y1, x2 + depth, y1 - depth, pimg);
	}
	else
		drawpoly(7, pt, pimg);
}


void
drawpoly(int numpoints, const int* polypoints, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		::Polyline(img->getdc(), reinterpret_cast<const ::POINT*>(polypoints),
			numpoints);
}

void
drawlines(int numlines, const int* polypoints, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		const auto pl(make_unique<unsigned long[]>(numlines));
		for(int i = 0; i < numlines; ++i)
			pl[i] = 2;
		::PolyPolyline(img->getdc(), reinterpret_cast<const ::POINT*>(polypoints), &pl[0], numlines);
	}
}

void
drawbezier(int numpoints, const int* polypoints, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		if(numpoints % 3 != 1)
			numpoints = numpoints - (numpoints + 2) % 3;
		::PolyBezier(img->getdc(), reinterpret_cast<const ::POINT*>(polypoints),
			numpoints);
	}
}

void
fillpoly(int numpoints, const int* polypoints, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		::Polygon(img->getdc(), reinterpret_cast<const ::POINT*>(polypoints),
			numpoints);
}

void
fillpoly_gradient(int numpoints, const ege_colpoint* polypoints, IMAGE* pimg)
{
	if(numpoints >= 3)
		if(const auto img = CONVERT_IMAGE(pimg))
		{
			auto vert(make_unique<::TRIVERTEX[]>(numpoints));
			auto tri(make_unique<::GRADIENT_TRIANGLE[]>(numpoints - 2));

			for(int i = 0; i < numpoints; ++i)
			{
				vert[i].x = long(polypoints[i].x);
				vert[i].y = long(polypoints[i].y);
				vert[i].Red = EGEGET_R(polypoints[i].color) << 8;
				vert[i].Green = EGEGET_G(polypoints[i].color) << 8;
				vert[i].Blue = EGEGET_B(polypoints[i].color) << 8;
				//vert[i].Alpha = EGEGET_A(polypoints[i].color) << 8;
				vert[i].Alpha = 0;
			}
			for(int j = 0; j < numpoints - 2; ++j)
			{
				tri[j].Vertex1 = j;
				tri[j].Vertex2 = j + 1;
				tri[j].Vertex3 = j + 2;
			}
			::GradientFill(img->getdc(), &vert[0], numpoints, &tri[0],
				numpoints - 2, GRADIENT_FILL_TRIANGLE);
		}
}

void
floodfill(int x, int y, int border, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		::FloodFill(img->getdc(), x, y, border);
}

void
floodfillsurface(int x, int y, color_t areacolor, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		::ExtFloodFill(img->getdc(), x, y, areacolor, FLOODFILLSURFACE);
}

} // namespace ege;

