#include "head.h"
#include "global.h"
#include "image.h"
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include "zlib.h"
#include "ege/sys_edit.h"
#include <utility> // for std::swap;

namespace ege
{

bool
is_run()
{
	return graph_setting._is_run();
}

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

void
setcaption(const char*  caption)
{
	::SetWindowTextA(getHWnd(), caption);
}
void
setcaption(const wchar_t* caption)
{
	::SetWindowTextW(getHWnd(), caption);
}

void
api_sleep(long dwMilliseconds)
{
	if(dwMilliseconds >= 0)
		::Sleep(dwMilliseconds);
}

void
ege_sleep(long ms)
{
	if(ms <= 0)
		return;

	static ::HANDLE hTimer = ::CreateWaitableTimer(nullptr, TRUE, nullptr);
	::LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = ms * (::LONGLONG) - 10000;
	if(hTimer)
	{
		if(::SetWaitableTimer(hTimer, &liDueTime, 0, nullptr, nullptr, FALSE))
			::WaitForSingleObject(hTimer, INFINITE); // != WAIT_OBJECT_0;
		//::CloseHandle(hTimer);
	}
	else
		::Sleep(ms);
}

void
delay(long ms)
{
	ege_sleep(ms);
}

void
delay_ms(long ms)
{
	graph_setting._delay_ms(ms);
}

/*
延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
*/
void
delay_fps(double fps)
{
	graph_setting._delay_fps(fps);
}

/*
延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
*/
void
delay_jfps(double fps)
{
	graph_setting._delay_jfps(fps);
}

int showmouse(int bShow)
{
	auto pg = &graph_setting;
	int ret = pg->mouse_show;
	pg->mouse_show = bShow;
	return ret;
}

int
mousepos(int* x, int* y)
{
	*x = graph_setting.mouse_last_x;
	*y = graph_setting.mouse_last_y;
	return 0;
}

void
setwritemode(int mode, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE_CONST(pimg));

	::SetROP2(img->m_hDC, mode);
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

/* private function */
static
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
		if(x2 - x1 < FLOAT_EPS) return;
		float d = (x2 - img->m_vpt.left) / (x2 - x1);
		y1 = (y1 - y2) * d + y2;
		x1 = (float)img->m_vpt.left;
		if(bswap == 1)
			bswap = 0;
	}
	if(x2 > img->m_vpt.right)
	{
		if(x2 - x1 < FLOAT_EPS) return;
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
		if(y2 - y1 < FLOAT_EPS)
			return;

		float d = (y2 - img->m_vpt.top) / (y2 - y1);

		x1 = (x1 - x2) * d + x2;
		y1 = (float)img->m_vpt.top;
		if(bswap == 1)
			bswap = 0;
	}
	if(y2 > img->m_vpt.bottom)
	{
		if(y2 - y1 < FLOAT_EPS)
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

void
lineto_f(float x, float y, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));
	POINT pt;

	::GetCurrentPositionEx(img->m_hDC, &pt);
	line_base(float(pt.x), float(pt.y), x, y, img);
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
line_f(float x1, float y1, float x2, float y2, IMAGE* pimg)
{
	const auto img(CONVERT_IMAGE(pimg));

	line_base(x1, y1, x2, y2, img);
	CONVERT_IMAGE_END;
}

/*private function*/
static
int
saveBrush(IMAGE* img, int save)   //此函数调用前，已经有Lock
{
	auto pg = &graph_setting;
	if(save)
	{
		::LOGBRUSH lbr{0, COLORREF(), ::ULONG_PTR()};

		lbr.lbColor = 0;
		lbr.lbStyle = BS_NULL;
		pg->savebrush_hbr = ::CreateBrushIndirect(&lbr);
		if(pg->savebrush_hbr)
		{
			pg->savebrush_hbr = ::HBRUSH(::SelectObject(img->m_hDC,
				pg->savebrush_hbr));
			return 1;
		}
	}
	else if(pg->savebrush_hbr)
	{
		pg->savebrush_hbr = (::HBRUSH)::SelectObject(img->m_hDC, pg->savebrush_hbr);
		::DeleteObject(pg->savebrush_hbr);
		pg->savebrush_hbr = nullptr;
	}
	return 0;
}

void rectangle(int left, int top, int right, int bottom, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(saveBrush(img, 1))
	{
		Rectangle(img->m_hDC, left, top, right, bottom);
		saveBrush(img, 0);
	}
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
		lPen.lopnColor   = color;
		lPen.lopnStyle   = img->m_linestyle.linestyle;
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
	{
		::DeleteObject(::SelectObject(img->m_hDC, hbr));
	}
	CONVERT_IMAGE_END;
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
	{
		return img->m_bk_color;
	}
	return 0xFFFFFFFF;
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
		{
			if(*p == col)
			{
				*p = color;
			}
		}
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

void setfontbkcolor(color_t color, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
	{
		::SetBkColor(img->m_hDC, RGBTOBGR(color));
	}
	CONVERT_IMAGE_END;
}

void
setbkmode(int iBkMode, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img && img->m_hDC)
	{
		::SetBkMode(img->m_hDC, iBkMode);
	}
	CONVERT_IMAGE_END;
}

IMAGE* gettarget()
{
	auto pg = &graph_setting;
	return pg->imgtarget_set;
}
int settarget(IMAGE* pbuf)
{
	auto pg = &graph_setting;
	pg->imgtarget_set = pbuf;
	if(!pbuf)
	{
		pg->imgtarget = pg->img_page[graph_setting.active_page];
	}
	else
	{
		pg->imgtarget = pbuf;
	}
	return 0;
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
				p != e;
				++p)
		{
			*p = c;
		}
	}
	CONVERT_IMAGE_END;
}

void
arc(int x, int y, int stangle, int endangle, int radius, IMAGE* pimg)
{
	ellipse(x, y, stangle, endangle, radius, radius, pimg);
}

void
arcf(float x, float y, float stangle, float endangle, float radius, IMAGE* pimg)
{
	ellipsef(x, y, stangle, endangle, radius, radius, pimg);
}

void
circle(int x, int y, int radius, IMAGE* pimg)
{
	ellipse(x, y, 0, 360, radius, radius, pimg);
}

void
circlef(float x, float y, float radius, IMAGE* pimg)
{
	ellipsef(x, y, 0.0f, 360.0f, radius, radius, pimg);
}

void
ellipse(int x, int y, int stangle, int endangle, int xradius, int yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;

	if(img)
	{
		Arc(img->m_hDC,
			x - xradius, y - yradius,
			x + xradius, y + yradius,
			(int)(x + xradius * cos(sr)),
			(int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)),
			(int)(y - yradius * sin(er))
		   );
	}
	CONVERT_IMAGE_END;
}

void
ellipsef(float x, float y, float stangle, float endangle, float xradius, float yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;

	if(img)
	{
		Arc(img->m_hDC,
			(int)(x - xradius), (int)(y - yradius),
			(int)(x + xradius), (int)(y + yradius),
			(int)(x + xradius * cos(sr)),
			(int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)),
			(int)(y - yradius * sin(er))
		   );
	}
	CONVERT_IMAGE_END;
}

void
pieslice(int x, int y, int stangle, int endangle, int radius, IMAGE* pimg)
{
	sector(x, y, stangle, endangle, radius, radius, pimg);
}

void
pieslicef(float x, float y, float stangle, float endangle, float radius, IMAGE* pimg)
{
	sectorf(x, y, stangle, endangle, radius, radius, pimg);
}

void
sector(int x, int y, int stangle, int endangle, int xradius, int yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;
	if(img)
	{
		Pie(img->m_hDC,
			x - xradius, y - yradius,
			x + xradius, y + yradius,
			(int)(x + xradius * cos(sr)), (int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)), (int)(y - yradius * sin(er))
		   );
	}
	CONVERT_IMAGE_END;
}

void
sectorf(float x, float y, float stangle, float endangle, float xradius, float yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	double sr = stangle / 180.0 * PI, er = endangle / 180.0 * PI;
	if(img)
	{
		Pie(img->m_hDC,
			(int)(x - xradius), (int)(y - yradius),
			(int)(x + xradius), (int)(y + yradius),
			(int)(x + xradius * cos(sr)), (int)(y - yradius * sin(sr)),
			(int)(x + xradius * cos(er)), (int)(y - yradius * sin(er))
		   );
	}
	CONVERT_IMAGE_END;
}

void
fillellipse(int x, int y, int xradius, int yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Ellipse(
			img->m_hDC,
			x - xradius, y - yradius,
			x + xradius, y + yradius
		);
	}
	CONVERT_IMAGE_END;
}

void
fillellipsef(float x, float y, float xradius, float yradius, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Ellipse(
			img->m_hDC,
			(int)(x - xradius), (int)(y - yradius),
			(int)(x + xradius), (int)(y + yradius)
		);
	}
	CONVERT_IMAGE_END;
}

void
bar(int left, int top, int right, int bottom, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	::RECT rect{left, top, right, bottom};
	::HBRUSH hbr_last = (::HBRUSH)::GetCurrentObject(img->m_hDC, OBJ_BRUSH); //(::HBRUSH)::SelectObject(pg->g_hdc, hbr);

	if(img)
	{
		FillRect(img->m_hDC, &rect, hbr_last);
	}
	CONVERT_IMAGE_END;
}

void
bar3d(int x1, int y1, int x2, int y2, int depth, int topflag, IMAGE* pimg)
{
	--x2;
	--y2;
	{
		int pt[20] =
		{
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
		{
			drawpoly(7, pt, pimg);
		}
	}
}

void
drawpoly(int numpoints, const int* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		Polyline(img->m_hDC, (POINT*)polypoints, numpoints);
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
fillpoly(int numpoints, const int* polypoints, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		Polygon(img->m_hDC, (POINT*)polypoints, numpoints);
	}
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

/* private function */
static
unsigned int
private_gettextmode(IMAGE* img)
{
	::UINT fMode = TA_NOUPDATECP; //TA_UPDATECP;
	if(img->m_texttype.horiz == RIGHT_TEXT)
	{
		fMode |= TA_RIGHT;
	}
	else if(img->m_texttype.horiz == CENTER_TEXT)
	{
		fMode |= TA_CENTER;
	}
	else
	{
		fMode |= TA_LEFT;
	}
	if(img->m_texttype.vert == BOTTOM_TEXT)
	{
		fMode |= TA_BOTTOM;
	}
	else
	{
		fMode |= TA_TOP;
	}
	return fMode;
}

/* private function */
static
void
private_textout(IMAGE* img, const char* textstring, int x, int y, int horiz, int vert)
{
	if(horiz >= 0 && vert >= 0)
	{
		::UINT fMode = TA_NOUPDATECP; //TA_UPDATECP;
		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
		if(img->m_texttype.horiz == RIGHT_TEXT)
		{
			fMode |= TA_RIGHT;
		}
		else if(img->m_texttype.horiz == CENTER_TEXT)
		{
			fMode |= TA_CENTER;
		}
		else
		{
			fMode |= TA_LEFT;
		}
		if(img->m_texttype.vert == BOTTOM_TEXT)
		{
			fMode |= TA_BOTTOM;
		}
		else
		{
			fMode |= TA_TOP;
		}
		::SetTextAlign(img->m_hDC, fMode);
	}
	else
	{
		::SetTextAlign(img->m_hDC, private_gettextmode(img));
	}
	if(textstring)
	{
		if(img->m_texttype.vert == CENTER_TEXT)
		{
			y -= textheight(textstring, img) / 2;
		}
		TextOutA(img->m_hDC, x, y, textstring, (int)strlen(textstring));
	}
}

/* private function */
static
void
private_textout(IMAGE* img, const wchar_t* textstring, int x, int y, int horiz, int vert)
{
	if(horiz >= 0 && vert >= 0)
	{
		::UINT fMode = TA_NOUPDATECP; //TA_UPDATECP;
		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
		if(img->m_texttype.horiz == RIGHT_TEXT)
		{
			fMode |= TA_RIGHT;
		}
		else if(img->m_texttype.horiz == CENTER_TEXT)
		{
			fMode |= TA_CENTER;
		}
		else
		{
			fMode |= TA_LEFT;
		}
		if(img->m_texttype.vert == BOTTOM_TEXT)
		{
			fMode |= TA_BOTTOM;
		}
		else
		{
			fMode |= TA_TOP;
		}
		::SetTextAlign(img->m_hDC, fMode);
	}
	else
	{
		::SetTextAlign(img->m_hDC, private_gettextmode(img));
	}
	if(textstring)
	{
		if(img->m_texttype.vert == CENTER_TEXT)
		{
			y -= textheight(textstring, img) / 2;
		}
		TextOutW(img->m_hDC, x, y, textstring, (int)::lstrlenW(textstring));
	}
}

void
outtext(const char* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		POINT pt;
		GetCurrentPositionEx(img->m_hDC, &pt);
		private_textout(img, textstring, pt.x, pt.y, -1, -1);
	}
	CONVERT_IMAGE_END;
}

void
outtext(const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		POINT pt;
		GetCurrentPositionEx(img->m_hDC, &pt);
		private_textout(img, textstring, pt.x, pt.y, -1, -1);
	}
	CONVERT_IMAGE_END;
}

void
outtext(CHAR c, IMAGE* pimg)
{
	CHAR str[10]{c};
	outtext(str, pimg);
}

void
outtext(wchar_t c, IMAGE* pimg)
{
	wchar_t str[10]{c};
	outtext(str, pimg);
}

void
outtextxy(int x, int y, const char* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		private_textout(img, textstring, x, y, -1, -1);
	}
	CONVERT_IMAGE_END;
}

void
outtextxy(int x, int y, const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		private_textout(img, textstring, x, y, -1, -1);
	}
	CONVERT_IMAGE_END;
}

void
outtextxy(int x, int y, CHAR c, IMAGE* pimg)
{
	CHAR str[10]{c};
	outtextxy(x, y, str, pimg);
}

void
outtextxy(int x, int y, wchar_t c, IMAGE* pimg)
{
	wchar_t str[10]{c};
	outtextxy(x, y, str, pimg);
}

void
outtextrect(int x, int y, int w, int h, const char*  textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		unsigned int fmode = private_gettextmode(img);
		::RECT rect{x, y, x + w, y + h};
		DrawTextA(img->m_hDC, textstring, -1, &rect, fmode | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_EXPANDTABS);
	}

	CONVERT_IMAGE_END;
}

void
outtextrect(int x, int y, int w, int h, const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		unsigned int fmode = private_gettextmode(img);
		::RECT rect{x, y, x + w, y + h};
		DrawTextW(img->m_hDC, textstring, -1, &rect, fmode | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_EXPANDTABS);
	}

	CONVERT_IMAGE_END;
}

void
xyprintf(int x, int y, const char*  fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	{
		auto pg = &graph_setting;
		char* buff = (char*)pg->g_t_buff;
		std::vsprintf(buff, fmt, v);
		outtextxy(x, y, buff);
	}
	va_end(v);
}

void
xyprintf(int x, int y, const wchar_t* fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	{
		auto pg = &graph_setting;
		wchar_t* buff = (wchar_t*)pg->g_t_buff;
		vswprintf(buff, fmt, v);
		outtextxy(x, y, buff);
	}
	va_end(v);
}

void
rectprintf(int x, int y, int w, int h, const char*  fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	{
		auto pg = &graph_setting;
		char* buff = (char*)pg->g_t_buff;
		vsprintf(buff, fmt, v);
		outtextrect(x, y, w, h, buff);
	}
	va_end(v);
}

void
rectprintf(int x, int y, int w, int h, const wchar_t* fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	{
		auto pg = &graph_setting;
		wchar_t* buff = (wchar_t*)pg->g_t_buff;
		vswprintf(buff, fmt, v);
		outtextrect(x, y, w, h, buff);
	}
	va_end(v);
}

int
textwidth(const char* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32A(img->m_hDC, textstring, (int)strlen(textstring), &sz);
		CONVERT_IMAGE_END;
		return sz.cx;
	}
	CONVERT_IMAGE_END;
	return 0;
}

int
textwidth(const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32W(img->m_hDC, textstring, (int)::lstrlenW(textstring), &sz);
		CONVERT_IMAGE_END;
		return sz.cx;
	}
	CONVERT_IMAGE_END;
	return 0;
}

int
textwidth(CHAR c, IMAGE* pimg)
{
	CHAR str[2]{c};
	return textwidth(str, pimg);
}

int
textwidth(wchar_t c, IMAGE* pimg)
{
	wchar_t str[2]{c};
	return textwidth(str, pimg);
}

int
textheight(const char* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32A(img->m_hDC, textstring, (int)strlen(textstring), &sz);
		CONVERT_IMAGE_END;
		return sz.cy;
	}
	CONVERT_IMAGE_END;
	return 0;
}

int
textheight(const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32W(img->m_hDC, textstring, (int)::lstrlenW(textstring), &sz);
		CONVERT_IMAGE_END;
		return sz.cy;
	}
	CONVERT_IMAGE_END;
	return 0;
}

int
textheight(CHAR c, IMAGE* pimg)
{
	CHAR str[2]{c};
	return textheight(str, pimg);
}

int
textheight(wchar_t c, IMAGE* pimg)
{
	wchar_t str[2]{c};
	return textheight(str, pimg);
}

void
settextjustify(int horiz, int vert, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
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
setfont(
	int nHeight,
	int nWidth,
	const char* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	BYTE fbCharSet,
	BYTE fbOutPrecision,
	BYTE fbClipPrecision,
	BYTE fbQuality,
	BYTE fbPitchAndFamily,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTA lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, fbCharSet,
			fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
	CONVERT_IMAGE_END;
}

void
setfont(
	int nHeight,
	int nWidth,
	const wchar_t* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	BYTE fbCharSet,
	BYTE fbOutPrecision,
	BYTE fbClipPrecision,
	BYTE fbQuality,
	BYTE fbPitchAndFamily,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTW lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, fbCharSet,
			fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
	CONVERT_IMAGE_END;
}

void
setfont(
	int nHeight,
	int nWidth,
	const char* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTA lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
	CONVERT_IMAGE_END;
}

void
setfont(
	int nHeight,
	int nWidth,
	const wchar_t* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTW lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
	CONVERT_IMAGE_END;
}

void
setfont(int nHeight, int nWidth, const char* lpszFace, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTA lf{nHeight, nWidth, 0, 0, FW_DONTCARE, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
	CONVERT_IMAGE_END;
}

void
setfont(int nHeight, int nWidth, const wchar_t* lpszFace, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTW lf{nHeight, nWidth, 0, 0, FW_DONTCARE, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
	CONVERT_IMAGE_END;
}

void
setfont(const ::LOGFONTA* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::DeleteObject(::SelectObject(img->m_hDC, ::CreateFontIndirectA(font)));
	CONVERT_IMAGE_END;
}

void
setfont(const ::LOGFONTW* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::DeleteObject(::SelectObject(img->m_hDC, ::CreateFontIndirectW(font)));
	CONVERT_IMAGE_END;
}

void
getfont(::LOGFONTA* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::GetObjectA((::HFONT)::GetCurrentObject(img->m_hDC, OBJ_FONT),
			sizeof(::LOGFONTA), font);
	CONVERT_IMAGE_END;
}

void
getfont(::LOGFONTW* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::GetObjectW((::HFONT)::GetCurrentObject(img->m_hDC, OBJ_FONT),
			sizeof(::LOGFONTA), font);
	CONVERT_IMAGE_END;
}

void
setrendermode(rendermode_e mode)
{
	if(mode == RENDER_MANUAL)
	{
		auto pg = &graph_setting;
		if(!pg->lock_window)
		{
			::KillTimer(pg->hwnd, RENDER_TIMER_ID);
			pg->timer_stop_mark = true;
			::PostMessageW(pg->hwnd, WM_TIMER, RENDER_TIMER_ID, 0);
			pg->lock_window = true;
			while(pg->timer_stop_mark)
				::Sleep(1);
		}
	}
	else
	{
		auto pg = &graph_setting;
		delay_ms(0);
		::SetTimer(pg->hwnd, RENDER_TIMER_ID, 0, nullptr);
		pg->skip_timer_mark = false;
		pg->lock_window = false;
	}
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
window_getviewport(viewporttype* viewport)
{
	auto pg = &graph_setting;
	viewport->left   = pg->base_x;
	viewport->top    = pg->base_y;
	viewport->right  = pg->base_w + pg->base_x;
	viewport->bottom = pg->base_h + pg->base_y;
}

void
window_getviewport(int* left, int* top, int* right, int* bottom)
{
	auto pg = &graph_setting;
	if(left)   *left   = pg->base_x;
	if(top)    *top    = pg->base_y;
	if(right)  *right  = pg->base_w + pg->base_x;
	if(bottom) *bottom = pg->base_h + pg->base_y;
}

void
window_setviewport(int left, int top, int right, int bottom)
{
	auto pg = &graph_setting;
	int same_xy = 0, same_wh = 0;
	if(pg->base_x == left && pg->base_y == top)
		same_xy = 1;
	if(pg->base_w == bottom - top && pg->base_h == right - left)
		same_wh = 1;
	pg->base_x = left;
	pg->base_y = top;
	pg->base_w = right - left;
	pg->base_h = bottom - top;
	if(same_xy == 0 || same_wh == 0)
	{
		graph_setting.update_mark_count -= 1;
	}
	/*修正窗口大小*/
	if(same_wh == 0)
	{
		::RECT rect, crect;
		int dw, dh;
		::GetClientRect(pg->hwnd, &crect);
		::GetWindowRect(pg->hwnd, &rect);
		dw = pg->base_w - crect.right;
		dh = pg->base_h - crect.bottom;
		{
			::HWND hwnd = GetParent(pg->hwnd);
			if(hwnd)
			{
				POINT pt{0, 0};
				::ClientToScreen(hwnd, &pt);
				rect.left   -= pt.x;
				rect.top    -= pt.y;
				rect.right  -= pt.x;
				rect.bottom -= pt.y;
			}

			::MoveWindow(pg->hwnd, rect.left, rect.top,
				rect.right + dw - rect.left, rect.bottom + dh - rect.top, TRUE);
		}
	}
}

void
getviewport(int* pleft, int* ptop, int* pright, int* pbottom, int* pclip, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(pleft)   *pleft  = img->m_vpt.left;
	if(ptop)   *ptop   = img->m_vpt.top;
	if(pright) *pright = img->m_vpt.right;
	if(pbottom)*pbottom = img->m_vpt.bottom;
	if(pclip)  *pclip  = img->m_vpt.clipflag;
	CONVERT_IMAGE_END;
}

void
setviewport(int left, int top, int right, int bottom, int clip, IMAGE* pimg)
{
	//_graph_setting * pg = &graph_setting;

	const auto img = CONVERT_IMAGE(pimg);

	SetViewportOrgEx(img->m_hDC, 0, 0, nullptr);

	img->m_vpt.left     = left;
	img->m_vpt.top      = top;
	img->m_vpt.right    = right;
	img->m_vpt.bottom   = bottom;
	img->m_vpt.clipflag = clip;

	if(img->m_vpt.left < 0)
		img->m_vpt.left = 0;
	if(img->m_vpt.top < 0)
		img->m_vpt.top = 0;
	if(img->m_vpt.right > img->m_width)
		img->m_vpt.right = img->m_width;
	if(img->m_vpt.bottom > img->m_height)
		img->m_vpt.bottom = img->m_height;

	::HRGN rgn = nullptr;

	if(img->m_vpt.clipflag)
		rgn = ::CreateRectRgn(img->m_vpt.left, img->m_vpt.top, img->m_vpt.right,
			img->m_vpt.bottom);
	else
		rgn = ::CreateRectRgn(0, 0, img->m_width,img->m_height);
	::SelectClipRgn(img->m_hDC, rgn);
	::DeleteObject(rgn);

	//OffsetViewportOrgEx(img->m_hDC, img->m_vpt.left, img->m_vpt.top, nullptr);
	::SetViewportOrgEx(img->m_hDC, img->m_vpt.left, img->m_vpt.top, nullptr);

	CONVERT_IMAGE_END;
}

void
clearviewport(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
	{
		::RECT rect{0, 0, img->m_vpt.right - img->m_vpt.left,
			img->m_vpt.bottom - img->m_vpt.top};
		::HBRUSH hbr_c = (::HBRUSH)::GetCurrentObject(img->m_hDC, OBJ_BRUSH);
		::LOGBRUSH logBrush;

		::GetObject(hbr_c, sizeof(logBrush), &logBrush);
		::HBRUSH hbr = ::CreateSolidBrush(logBrush.lbColor);
		::FillRect(img->m_hDC, &rect, hbr);
		::DeleteObject(hbr);
	}
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
}

void
ege_setpattern_none(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		img->delete_pattern();
	}
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
}

void
ege_gentexture(bool gen, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);
	if(img)
	{
		img->gentexture(gen);
	}
	CONVERT_IMAGE_END;
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
	CONVERT_IMAGE_END;
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
			//graphics.::SetTransform();
			graphics.DrawImage((Gdiplus::Image*)srcimg->m_texture,
				Gdiplus::RectF(dest.x, dest.y, dest.w, dest.h),
				src.x,
				src.y,
				src.w,
				src.h,
				Gdiplus::UnitPixel,
				nullptr
			);
		}
	}
	CONVERT_IMAGE_END;
}

::HWND
getHWnd()
{
	return graph_setting.hwnd;
}

::HINSTANCE
getHInstance()
{
	return graph_setting.instance;
}

int
message_addkeyhandler(void* param, LPMSG_KEY_PROC func)
{
	graph_setting.callback_key = func;
	graph_setting.callback_key_param = param;
	return grOk;
}

int
message_addmousehandler(void* param, LPMSG_MOUSE_PROC func)
{
	graph_setting.callback_mouse = func;
	graph_setting.callback_mouse_param = param;
	return grOk;
}

int
SetCloseHandler(LPCALLBACK_PROC func)
{
	graph_setting.callback_close = func;
	return grOk;
}

/* private funcion */
static
void
draw_frame(IMAGE* img, int l, int t, int r, int b, color_t lc, color_t dc)
{
	setcolor(lc, img);
	line(l, b, l, t, img);
	lineto(r, t, img);
	setcolor(dc, img);
	lineto(r, b, img);
	lineto(l, b, img);
}

int
inputbox_getline(const char* title, const char* text, char* buf, int len)
{
	const auto _buf(static_cast<wchar_t*>(operator new(len * 2)));
	wchar_t _title[256], _text[256];

	::MultiByteToWideChar(CP_ACP, 0, title, -1, _title, 256);
	::MultiByteToWideChar(CP_ACP, 0,  text, -1,  _text, 256);
	buf[0] = 0;

	int ret = inputbox_getline(_title, _text, _buf, len);

	if(ret)
		::WideCharToMultiByte(CP_ACP, 0, _buf, -1, buf, len, nullptr, nullptr);
	operator delete(_buf);
	return ret;
}

int
inputbox_getline(const wchar_t* title, const wchar_t* text, wchar_t* buf, int len)
{
	auto pg = &graph_setting;
	IMAGE bg;
	IMAGE window;
	int w = 400, h = 300, x = (getwidth() - w) / 2, y = (getheight() - h) / 2;
	bool lock_window = false;
	int ret = 0;

	bg.getimage(0, 0, getwidth(), getheight());
	window.createimage(w, h);
	buf[0] = 0;

	lock_window = pg->lock_window;
	if(!lock_window)
		setrendermode(RENDER_MANUAL);

	sys_edit edit(true);
	edit.create(true);
	edit.move(x + 30 + 1, y + 192 + 1);
	edit.size(w - (30 + 1) * 2, h - 40 - 192 - 2);
	edit.setmaxlen(len);
	edit.visable(true);
	edit.setfocus();
	for(int bInit = 0; is_run(); delay_fps(30))
	{
		if(bInit)
		{
			bool exit = false;
			do
			{
				key_msg msg = getkey();
				if(msg.key == key_enter && msg.msg == key_msg_up)
				{
					exit = true;
					break;
				}
			} while(kbmsg());
			if(exit)
				break;
		}
		putimage(0, 0, &bg);
		if(bInit == 0)
		{
			bInit = 1;
			setbkcolor(EGERGB(0x80, 0xA0, 0x80), &window);
			draw_frame(&window, 0, 0, w - 1, h - 1,
				EGERGB(0xA0, 0xC0, 0xA0), EGERGB(0x50, 0x70, 0x50));
			setfillcolor(EGERGB(0, 0, 0xA0), &window);
			for(int dy = 1; dy < 24; dy++)
			{
				setcolor(HSLtoRGB(240.0f, 1.0f,
					0.5f + float(dy / 24.0 * 0.3)), &window);
				line(1, dy, w - 1, dy, &window);
			}
			setcolor(0xFFFFFF, &window);
			setbkmode(TRANSPARENT, &window);
			setfont(18, 0, L"Tahoma", &window);
			outtextxy(3, 3, title, &window);
			setcolor(0x0, &window);

			::RECT rect{30, 32, w - 30, 128 - 3};

			DrawTextW(window.m_hDC, text, -1, &rect, DT_NOPREFIX | DT_LEFT
				| DT_TOP | TA_NOUPDATECP | DT_WORDBREAK | DT_EDITCONTROL
				| DT_EXPANDTABS);
		}
		putimage(x, y, &window);
	}
	edit.gettext(len, buf);
	len = ::lstrlenW(buf);
	while(len > 0 && (buf[len - 1] == '\r' || buf[len - 1] == '\n'))
		buf[--len] = 0;
	ret = len;
	putimage(0, 0, &bg);
	if(!lock_window)
		setrendermode(RENDER_AUTO);
	pg->_getflush();
	return ret;
}

float
getfps()
{
	return graph_setting._get_FPS(0);
}

double
fclock()
{
	auto pg = &graph_setting;

	if(pg->fclock_start == 0)
		pg->fclock_start = ::GetTickCount();
	return (::GetTickCount() - pg->fclock_start) / 1000.0; //pg->_get_highfeq_time_ls();
}

int
ege_compress(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen)
{
	if(sourceLen == 0)
		return -1;
	int ret = compress((Bytef*)dest + sizeof(unsigned long),
		(uLongf*)destLen, (Bytef*)source, (uLong)sourceLen);
	((unsigned long*)dest)[0] = sourceLen;
	*destLen += 4;
	return ret;
}

int
ege_compress2(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen, int level)
{
	if(sourceLen == 0)
		return -1;
	int ret = compress2((Bytef*)dest + sizeof(unsigned long),
		(uLongf*)destLen, (Bytef*)source, (uLong)sourceLen, level);
	*(unsigned long*)dest = sourceLen;
	*destLen += sizeof(unsigned long);
	return ret;
}

unsigned long
ege_uncompress_size(const void* source, unsigned long sourceLen)
{
	return sourceLen > sizeof(unsigned long) ? ((unsigned long*)source)[0] : 0;
}

int
ege_uncompress(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen)
{
	*(uLongf*)destLen = ege_uncompress_size(source, sourceLen);
	return *(uLongf*)destLen > 0 ? uncompress((Bytef*)dest, (uLongf*)destLen,
		(Bytef*)source + sizeof(unsigned long),
		(uLong)sourceLen - sizeof(unsigned long)) : -1;
}

} // namespace ege
