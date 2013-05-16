#include "ege/gapi.h"
#include "global.h"
#include "image.h"
#include <utility> // for std::swap;
#include <cfloat> // for FLT_EPSILON;

namespace ege
{

namespace
{

//此函数调用前，已经有Lock
int
_save_brush(IMAGE* img, int save)
{
	static ::HBRUSH savebrush_hbr;

	if(save)
	{
		::LOGBRUSH lbr{0, COLORREF(), ::ULONG_PTR()};

		lbr.lbColor = 0;
		lbr.lbStyle = BS_NULL;
		savebrush_hbr = ::CreateBrushIndirect(&lbr);
		if(savebrush_hbr)
		{
			savebrush_hbr = ::HBRUSH(::SelectObject(img->m_hDC,
				savebrush_hbr));
			return 1;
		}
	}
	else if(savebrush_hbr)
	{
		savebrush_hbr = (::HBRUSH)::SelectObject(img->m_hDC, savebrush_hbr);
		::DeleteObject(savebrush_hbr);
		savebrush_hbr = nullptr;
	}
	return 0;
}

} // unnamed namespace;


IMAGE*
gettarget()
{
	return graph_setting._get_target();
}

#if 0
void
swappage()
{
	auto pg = &graph_setting;
	setvisualpage(pg->active_page);
	setactivepage(1 - pg->active_page);
}
#endif

void
setactivepage(int page)
{
	graph_setting._set_activepage(page);
}

void
setvisualpage(int page)
{
	graph_setting._set_visualpage(page);
}


int
settarget(IMAGE* pbuf)
{
	return graph_setting._set_target(pbuf);
}

void
cleardevice(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
	{
		color_t c = getbkcolor(img);

		for(color_t* p = (color_t*)img->getbuffer(),
			*e = (color_t*)&img->getbuffer()[img->m_width * img->m_height];
			p != e; ++p)
			*p = c;
	}
	CONVERT_IMAGE_END;
}


void
getlinestyle(int* plinestyle, unsigned short* pupattern, int* pthickness, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(plinestyle)
	{
		*plinestyle = img->m_linestyle.linestyle;
	}
	if(pupattern)
	{
		*pupattern = img->m_linestyle.upattern;
	}
	if(pthickness)
	{
		*pthickness = img->m_linestyle.thickness;
	}
	CONVERT_IMAGE_END;
}

void
setlinestyle(int linestyle, unsigned short upattern, int thickness, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	LOGPEN lpen{0, POINT(), COLORREF()};
	lpen.lopnColor = RGBTOBGR(getcolor(pimg));
	img->m_linestyle.thickness = thickness;
	img->m_linewidth = (float)thickness;
	img->m_linestyle.linestyle = linestyle;
	img->m_linestyle.upattern = upattern;

	lpen.lopnWidth.x = thickness;
	lpen.lopnStyle   = linestyle;

	HPEN hpen;
	if(linestyle == PS_USERSTYLE)
	{
		::DWORD style[20]{0};
		LOGBRUSH lbr;
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
				{
					len++;
				}
			}
			else
			{
				if(st == 0)
				{
					len++;
				}
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
	{
		hpen = ::CreatePenIndirect(&lpen);
	}
	if(hpen)
	{
		::DeleteObject(::SelectObject(img->m_hDC, hpen));
	}
	CONVERT_IMAGE_END;
}

void
setlinewidth(float width, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	img->m_linestyle.thickness = (int)width;
	img->m_linewidth = width;
	CONVERT_IMAGE_END;
}

void
setfillstyle(int pattern, color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	LOGBRUSH lbr{0, COLORREF(), ::UINT_PTR()};
	img->m_fillcolor = color;
	lbr.lbColor = RGBTOBGR(color);
	//::SetBkColor(img->m_hDC, color);
	if(pattern < SOLID_FILL)
	{
		lbr.lbHatch = BS_NULL;
	}
	else if(pattern == SOLID_FILL)
	{
		lbr.lbHatch = BS_SOLID;
	}
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
	{
		lbr.lbHatch = BS_SOLID;
	}
	::HBRUSH hbr = ::CreateBrushIndirect(&lbr);
	if(hbr)
	{
		::DeleteObject(::SelectObject(img->m_hDC, hbr));
	}
	CONVERT_IMAGE_END;
}

void
setwritemode(int mode, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE_CONST(pimg));

	::SetROP2(img->m_hDC, mode);
	CONVERT_IMAGE_END;
}


color_t
getcolor(IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE_CONST(pimg));

	if(img && img->m_hDC)
	{
		CONVERT_IMAGE_END;
		return img->m_color;
		/*
		HPEN hpen_c = (HPEN)::GetCurrentObject(img->m_hDC, OBJ_PEN);
		LOGPEN logPen;
		GetObject(hpen_c, sizeof(logPen), &logPen);
		CONVERT_IMAGE_END;
		return logPen.lopnColor;
		// */
	}
	CONVERT_IMAGE_END;
	return 0xFFFFFFFF;
}

color_t
getfillcolor(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img && img->m_hDC)
	{
		CONVERT_IMAGE_END;
		return img->m_fillcolor;
	}
	CONVERT_IMAGE_END;
	return 0xFFFFFFFF;
}

color_t
getbkcolor(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	CONVERT_IMAGE_END;
	if(img)
		return img->m_bk_color;
	return 0xFFFFFFFF;
}


void
setcolor(color_t color, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));

	if(img && img->m_hDC)
	{
		LOGPEN lPen;
		HPEN hpen;

		img->m_color = color;
		color = RGBTOBGR(color);
		lPen.lopnColor = color;
		lPen.lopnStyle = img->m_linestyle.linestyle;
		lPen.lopnWidth.x = img->m_linestyle.thickness;
		::SetTextColor(img->m_hDC, color);
		if(lPen.lopnStyle == PS_USERSTYLE)
		{
			::DWORD style[20]{};
			LOGBRUSH lbr;
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
			::DeleteObject(::SelectObject(img->m_hDC, hpen));
	}
	CONVERT_IMAGE_END;
}

void
setfillcolor(color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	LOGBRUSH lbr{0, COLORREF(), ::ULONG_PTR()};

	img->m_fillcolor = color;
	color = RGBTOBGR(color);
	lbr.lbColor = color;
	lbr.lbHatch = BS_SOLID;
	::HBRUSH hbr = ::CreateBrushIndirect(&lbr);
	if(hbr)
		::DeleteObject(::SelectObject(img->m_hDC, hbr));
	CONVERT_IMAGE_END;
}

void
setbkcolor(color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
	{
		PDWORD p = img->m_pBuffer;
		int size = img->m_width * img->m_height;
		color_t col = img->m_bk_color;

		img->m_bk_color = color;
		::SetBkColor(img->m_hDC, RGBTOBGR(color));
		for(int n = 0; n < size; n++, p++)
			if(*p == col)
				*p = color;
	}
	CONVERT_IMAGE_END;
}



void
setbkcolor_f(color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
	{
		img->m_bk_color = color;
		::SetBkColor(img->m_hDC, RGBTOBGR(color));
	}
	CONVERT_IMAGE_END;
}

void
setfontbkcolor(color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
		::SetBkColor(img->m_hDC, RGBTOBGR(color));
	CONVERT_IMAGE_END;
}

void
setbkmode(int iBkMode, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img && img->m_hDC)
		::SetBkMode(img->m_hDC, iBkMode);
	CONVERT_IMAGE_END;
}


color_t
getpixel(int x, int y, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE_CONST(pimg));
	CONVERT_IMAGE_END;

	x += img->m_vpt.left;
	y += img->m_vpt.top;
	return (x < 0) || (y < 0) || (x >= img->m_width) || (y >= img->m_height) ? 0
		: color_t(img->m_pBuffer[y * img->m_width + x]);
}

void
putpixel(int x, int y, color_t color, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));

	x += img->m_vpt.left;
	y += img->m_vpt.top;
	if(!(x < 0 || y < 0 || x >= img->m_vpt.right || y >= img->m_vpt.bottom))
		img->m_pBuffer[y * img->m_width + x] = color;
	CONVERT_IMAGE_END;
}

color_t
getpixel_f(int x, int y, IMAGE* pimg)
{
	IMAGE* img = CONVERT_IMAGE_F_CONST(pimg);
	color_t col = img->m_pBuffer[y * img->m_width + x];
	return col;
}

void
putpixel_f(int x, int y, color_t color, IMAGE* pimg)
{
	IMAGE* img = CONVERT_IMAGE_F(pimg);
	img->m_pBuffer[y * img->m_width + x] = color;
}

void
putpixels(int nPoint, int* pPoints, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));
	int x, y, c;
	PDWORD pb = &img->m_pBuffer[img->m_vpt.top * img->m_width + img->m_vpt.left];
	int w = img->m_vpt.right - img->m_vpt.left, h = img->m_vpt.bottom - img->m_vpt.top;
	int tw = img->m_width;

	for(int n = 0; n < nPoint; ++n, pPoints += 3)
	{
		x = pPoints[0], y = pPoints[1], c = pPoints[2];
		if(!(x < 0 || y < 0 || x >= w || y >= h))
			pb[y * tw + x] = RGBTOBGR(c);
	}
	CONVERT_IMAGE_END;
}

void
putpixels_f(int nPoint, int* pPoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	int c;
	int tw = img->m_width;
	for(int n = 0; n < nPoint; ++n, pPoints += 3)
	{
		c = pPoints[2];
		img->m_pBuffer[pPoints[1] * tw + pPoints[0]] = RGBTOBGR(c);
	}
	CONVERT_IMAGE_END;
}


void
moveto(int x, int y, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	MoveToEx(img->m_hDC, x, y, nullptr);
	CONVERT_IMAGE_END;
}

void
moverel(int dx, int dy, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	POINT pt;
	GetCurrentPositionEx(img->m_hDC, &pt);
	dx += pt.x;
	dy += pt.y;
	MoveToEx(img->m_hDC, dx, dy, nullptr);
	CONVERT_IMAGE_END;
}


void
line(int x1, int y1, int x2, int y2, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));
	::MoveToEx(img->m_hDC, x1, y1, nullptr);
	::LineTo(img->m_hDC, x2, y2);
	CONVERT_IMAGE_END;
}

void
linerel(int dx, int dy, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));
	POINT pt;
	GetCurrentPositionEx(img->m_hDC, &pt);
	dx += pt.x;
	dy += pt.y;
	LineTo(img->m_hDC, dx, dy);
	CONVERT_IMAGE_END;
}

void
lineto(int x, int y, IMAGE* pimg)
{
	const auto img (CONVERT_IMAGE(pimg));
	LineTo(img->m_hDC, x, y);
	CONVERT_IMAGE_END;
}

namespace
{

void
line_base(float x1, float y1, float x2, float y2, IMAGE* img)
{
	int bswap = 2;
	auto col = getcolor(img);
	color_t endp = 0;
	auto pBuffer = (color_t*)img->m_pBuffer;
	int rw = img->m_width;

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
		x1 = (float)img->m_vpt.left;
		if(bswap == 1)
			bswap = 0;
	}
	if(x2 > img->m_vpt.right)
	{
		if(x2 - x1 < FLT_EPSILON) return;
		float d = (img->m_vpt.right - x1) / (x2 - x1);
		y2 = (y2 - y1) * d + y1;
		x2 = (float)img->m_vpt.right;
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
		y1 = (float)img->m_vpt.top;
		if(bswap == 1)
			bswap = 0;
	}
	if(y2 > img->m_vpt.bottom)
	{
		if(y2 - y1 < FLT_EPSILON)
			return;

		float d = (img->m_vpt.bottom - y1) / (y2 - y1);

		x2 = (x2 - x1) * d + x1;
		y2 = (float)img->m_vpt.bottom;
		if(bswap == 2)
			bswap = 0;
	}
	if(bswap)
		endp = pBuffer[bswap == 1 ? (int)y1 * rw + (int)x1
			: (int)y2 * rw + (int)x2];
	if(y2 - y1 > fabs(x2 - x1))
	{
		int y = (int)(y1 + 0.9f);
		int ye = (int)(y2);
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
			pBuffer[y * rw + (int)x] = col;
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

		int x = (int)(x1 + 0.9f);
		int xe = (int)(x2);
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
			pBuffer[(int)y * rw + x] = col;
	}
	if(bswap)
		pBuffer[bswap == 1 ? (int)y1 * rw + (int)x1
			: (int)y2 * rw + (int)x2] = endp;
}

} // unnamed namespace;

void
line_f(float x1, float y1, float x2, float y2, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));

	line_base(x1, y1, x2, y2, img);
	CONVERT_IMAGE_END;
}

void
linerel_f(float dx, float dy, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));
	POINT pt;

	::GetCurrentPositionEx(img->m_hDC, &pt);
	line_base(float(pt.x), float(pt.y), float(pt.x) + dx, float(pt.y) + dy,
		img);
	CONVERT_IMAGE_END;
}

void
lineto_f(float x, float y, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));
	POINT pt;

	::GetCurrentPositionEx(img->m_hDC, &pt);
	line_base(float(pt.x), float(pt.y), x, y, img);
	CONVERT_IMAGE_END;
}


void rectangle(int left, int top, int right, int bottom, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(_save_brush(img, 1))
	{
		Rectangle(img->m_hDC, left, top, right, bottom);
		_save_brush(img, 0);
	}
	CONVERT_IMAGE_END;
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
	const auto img = CONVERT_IMAGE(pimg);
	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;

	if(img)
		Arc(img->m_hDC, x - xradius, y - yradius, x + xradius, y + yradius,
			(int)(x + xradius * cos(sr)), (int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)), (int)(y - yradius * sin(er)));
	CONVERT_IMAGE_END;
}

void
fillellipse(int x, int y, int xradius, int yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
		Ellipse(img->m_hDC, x - xradius, y - yradius, x + xradius, y + yradius);
	CONVERT_IMAGE_END;
}

void
sector(int x, int y, int stangle, int endangle, int xradius, int yradius,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;

	if(img)
		Pie(img->m_hDC, x - xradius, y - yradius, x + xradius, y + yradius,
			(int)(x + xradius * cos(sr)), (int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)), (int)(y - yradius * sin(er)));
	CONVERT_IMAGE_END;
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
	const auto img = CONVERT_IMAGE(pimg);
	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;

	if(img)
		Arc(img->m_hDC, (int)(x - xradius), (int)(y - yradius),
			(int)(x + xradius), (int)(y + yradius),
			(int)(x + xradius * cos(sr)), (int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)), (int)(y - yradius * sin(er)));
	CONVERT_IMAGE_END;
}

void
fillellipsef(float x, float y, float xradius, float yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
		Ellipse(img->m_hDC, (int)(x - xradius), (int)(y - yradius),
			(int)(x + xradius), (int)(y + yradius));
	CONVERT_IMAGE_END;
}

void
sectorf(float x, float y, float stangle, float endangle, float xradius, float yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;
	if(img)
		Pie(img->m_hDC, (int)(x - xradius), (int)(y - yradius),
			(int)(x + xradius), (int)(y + yradius),
			(int)(x + xradius * cos(sr)), (int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)), (int)(y - yradius * sin(er)));
	CONVERT_IMAGE_END;
}


void
bar(int left, int top, int right, int bottom, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	::RECT rect{left, top, right, bottom};
	::HBRUSH hbr_last = (::HBRUSH)::GetCurrentObject(img->m_hDC, OBJ_BRUSH);

	if(img)
		FillRect(img->m_hDC, &rect, hbr_last);
	CONVERT_IMAGE_END;
}

void
bar3d(int x1, int y1, int x2, int y2, int depth, int topflag, IMAGE* pimg)
{
	--x2;
	--y2;

	int pt[20]{
		x2, y2,
		x2, y1,
		x1, y1,
		x1, y2,
		x2, y2,
		x2 + depth, y2 - depth,
		x2 + depth, y1 - depth,
		x1 + depth, y1 - depth,
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
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
		Polyline(img->m_hDC, (POINT*)polypoints, numpoints);
	CONVERT_IMAGE_END;
}

void
drawlines(int numlines, const int* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		::DWORD* pl = (::DWORD*) malloc(sizeof(::DWORD) * numlines);
		for(int i = 0; i < numlines; ++i) pl[i] = 2;
		PolyPolyline(img->m_hDC, (POINT*)polypoints, pl, numlines);
		free(pl);
	}
	CONVERT_IMAGE_END;
}

void
drawbezier(int numpoints, const int* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		if(numpoints % 3 != 1)
		{
			numpoints = numpoints - (numpoints + 2) % 3;
		}
		PolyBezier(img->m_hDC, (POINT*)polypoints, numpoints);
	}
	CONVERT_IMAGE_END;
}

void
fillpoly(int numpoints, const int* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
		Polygon(img->m_hDC, (POINT*)polypoints, numpoints);
	CONVERT_IMAGE_END;
}

void
fillpoly_gradient(int numpoints, const ege_colpoint* polypoints, IMAGE* pimg)
{
	if(numpoints < 3) return;
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		TRIVERTEX* vert = (TRIVERTEX*)malloc(sizeof(TRIVERTEX) * numpoints);
		if(vert)
		{
			GRADIENT_TRIANGLE* tri = (GRADIENT_TRIANGLE*)malloc(sizeof(GRADIENT_TRIANGLE) * (numpoints - 2));
			if(tri)
			{
				for(int i = 0; i < numpoints; ++i)
				{
					vert[i].x = (long)polypoints[i].x;
					vert[i].y = (long)polypoints[i].y;
					vert[i].Red     = EGEGET_R(polypoints[i].color) << 8;
					vert[i].Green   = EGEGET_G(polypoints[i].color) << 8;
					vert[i].Blue    = EGEGET_B(polypoints[i].color) << 8;
					//vert[i].Alpha   = EGEGET_A(polypoints[i].color) << 8;
					vert[i].Alpha   = 0;
				}
				for(int j = 0; j < numpoints - 2; ++j)
				{
					tri[j].Vertex1 = j;
					tri[j].Vertex2 = j + 1;
					tri[j].Vertex3 = j + 2;
				}
				::GradientFill(img->getdc(), vert, numpoints, tri, numpoints - 2, GRADIENT_FILL_TRIANGLE);
				free(tri);
			}
			free(vert);
		}
	}
	CONVERT_IMAGE_END;
}

void
floodfill(int x, int y, int border, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		FloodFill(img->m_hDC, x, y, RGBTOBGR(border));
	}
	CONVERT_IMAGE_END;
}

void
floodfillsurface(int x, int y, color_t areacolor, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		ExtFloodFill(img->m_hDC, x, y, RGBTOBGR(areacolor), FLOODFILLSURFACE);
	}
	CONVERT_IMAGE_END;
}

} // namespace ege;

