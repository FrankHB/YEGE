#include "graphics.h"
#include <complex>
#include <ctime>

#define USE_MPF 1

#if USE_MPF
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#	include <gmpxx.h>
#	pragma GCC diagnostic pop
#endif

//#include <algorithm>

//using namespace std;

#define BASEITERATIONS 1  //首次迭代次数
#define ITERATIONS 16     //增量迭代次数
#define MAXCOLOR 0x40     //颜色数
#define COLORMASK (0x200 - 1)

#define SC_W 160
#define SC_H 120


#if USE_MPF
using Float = mpf_class;
#else
using Float = double;
#endif

using COMPLEX = std::complex<Float>;
using COMPLEXI = std::complex<int>;

#if USE_MPF
inline double
abs(const COMPLEX& c)
{
	double d1 = mpf_get_d(c.real().get_mpf_t()), d2 = mpf_get_d(c.imag().get_mpf_t());
	return d1 * d1 + d2 * d2;
}

mp_bitcnt_t g_prec = 64;

void
setprec(COMPLEX& c)
{
	c.real().set_prec(g_prec);
	c.imag().set_prec(g_prec);
}

void
setgprec(Float f)
{
	mp_bitcnt_t t = 0;
	while(f < 1)
	{
		f *= 2;
		t += 1;
	}
	t /= 32;
	t += 1;
	t = t + t / 16 + 1;
	t *= 32;
	g_prec = t;
}
#endif

//typedef complex<double> COMPLEX;

#define W_B 10
#define WIDTH (1<<W_B)

struct PIXEL
{
	COMPLEX last;
	int nIter;
	int ed;
	int calc;
};

PIXEL(*pMap)[WIDTH] = new PIXEL[SC_H][WIDTH];

struct updatelist
{
	::POINT* p, *pn;
	::POINT m_list[2][SC_H* SC_W];
	int nBeg, nLen;
	int nLen_n;
	updatelist()
	{
		clear();
	}
	void clear()
	{
		p = m_list[0];
		pn = m_list[1];
		nBeg = nLen = nLen_n = 0;
	}
	void push(int x, int y)
	{
		pn[nLen_n].x = x;
		pn[nLen_n].y = y;
		++nLen_n;
	}
	int pop(int* x, int* y)
	{
		if(nBeg == nLen) return 0;
		*x = p[nBeg].x;
		*y = p[nBeg].y;
		++nBeg;
		return 1;
	}
	void swap()
	{
		nBeg = 0;
		nLen = nLen_n;
		nLen_n = 0;
		::POINT* _p = p;
		p = pn;
		pn = _p;
	}
}* g_pudlist = new updatelist;
updatelist& g_udlist = *g_pudlist;

#if 0
struct calclist
{
	int m_len;
	int m_list[SC_H* SC_W];
	void clear()
	{
		m_len = 0;
	}
	void push(int x, int y)
	{
		++m_len;
		m_list[m_len] = y << W_B + x;
		PIXEL* p = (PIXEL*)pMap;
		int t = m_len;
		for(; t > 1; t >>= 1)
		{
			if(p[m_list[t]].nIter < p[m_list[t >> 1]].nIter)
			{
				std::swap(m_list[t], m_list[t >> 1]);
			}
			else break;
		}
	}
	void update(int x, int y)
	{
		int t = y << W_B + x;
		int t2 = t << 1;
		PIXEL* p = (PIXEL*)pMap;
		for(; t2 < m_len; t2 = (t << 1))
		{
			if(p[m_list[t2]].nIter <= p[m_list[t2 + 1]].nIter)
			{
				if(p[m_list[t]].nIter <= p[m_list[t2]].nIter)
				{
					return ;
				}
				else
				{
					std::swap(m_list[t], m_list[t2]);
					t = t2;
				}
			}
			else
			{
				t2 += 1;
				if(p[m_list[t]].nIter <= p[m_list[t2]].nIter)
				{
					return ;
				}
				else
				{
					std::swap(m_list[t], m_list[t2]);
					t = t2;
				}
			}
		}
		if(t2 <= m_len)
		{
			if(p[m_list[t]].nIter > p[m_list[t2]].nIter)
			{
				std::swap(m_list[t], m_list[t2]);
			}
		}
	}
	int pop(int x, int y)
	{
		if(m_len <= 0) return -1;
		int t = y << W_B + x;
		std::swap(m_list[t], m_list[m_len]);
		--m_len;
		update(x, y);
		return 0;
	}
};
#endif

// 定义颜色
int Color[COLORMASK + 1];

void fixcolor(int* color)
{
	int r = *color & 0xFF;
	int g = (*color >> 8) & 0xFF;
	int b = (*color >> 16) & 0xFF;
	double fr, fg, fb;
	fr = r / 255.0;
	fg = g / 255.0;
	fb = b / 255.0;
	fr *= fr;
	fg *= fg;
	fb *= fb;
	r = (int)(fr * 255);
	g = (int)(fg * 255);
	b = (int)(fb * 255);
	*color = RGB(r, g, b);
}

// 初始化颜色
void setinitcolor(int* color, int len, int h1, int)
{
	int i;
	for(i = 0; i < len / 2; i++)
	{
		color[i] = HSLtoRGB((float)h1, 0.9f, i * 2.0f / len * 0.7f + 0.15f);
		fixcolor(&color[i]);
		color[len - 1 - i] = HSLtoRGB((float)h1, 0.9f, i * 2.0f / len * 0.7f + 0.15f);
		fixcolor(&color[len - 1 - i]);
	}
}
// 初始化颜色
void InitColor()
{
	// 使用 HSL 颜色模式产生角度 h1 到 h2 的渐变色
	int h1, h2 = 0;
	h1 = 240;
	setinitcolor(Color + MAXCOLOR * 0, MAXCOLOR, h1, h2);
	h1 = 30;
	setinitcolor(Color + MAXCOLOR * 1, MAXCOLOR, h1, h2);
	h1 = 330;
	setinitcolor(Color + MAXCOLOR * 2, MAXCOLOR, h1, h2);
	h1 = 180;
	setinitcolor(Color + MAXCOLOR * 3, MAXCOLOR, h1, h2);
	h1 = 270;
	setinitcolor(Color + MAXCOLOR * 4, MAXCOLOR, h1, h2);
	h1 = 0;
	setinitcolor(Color + MAXCOLOR * 5, MAXCOLOR, h1, h2);
	h1 = 300;
	setinitcolor(Color + MAXCOLOR * 6, MAXCOLOR, h1, h2);
	h1 = 150;
	setinitcolor(Color + MAXCOLOR * 7, MAXCOLOR, h1, h2);
#if 0
	h1 = 240, h2 = 60;
	setinitcolor(Color, MAXCOLOR, h1, h2);
	h1 = 270, h2 = 90;
	setinitcolor(Color+MAXCOLOR, MAXCOLOR, h1, h2);
	h1 = 300, h2 = 120;
	setinitcolor(Color+MAXCOLOR*2, MAXCOLOR, h1, h2);
	h1 = 330, h2 = 150;
	setinitcolor(Color+MAXCOLOR*3, MAXCOLOR, h1, h2);
	h1 = 0, h2 = 180;
	setinitcolor(Color+MAXCOLOR*4, MAXCOLOR, h1, h2);
	h1 = 30, h2 = 210;
	setinitcolor(Color+MAXCOLOR*5, MAXCOLOR, h1, h2);
	h1 = 60, h2 = 240;
	setinitcolor(Color+MAXCOLOR*6, MAXCOLOR, h1, h2);
	h1 = 90, h2 = 270;
	setinitcolor(Color+MAXCOLOR*7, MAXCOLOR, h1, h2);
#endif
}

#if 0
#define func(z, c, ed) \
{z*=z; if(z.real() > 4) \
{ed = 1; break;} z+=c;} //(z * z * z * z * z * z + c)
#endif
#define func(z, c) z *= z, z += c;

inline
int Mandelbrot(COMPLEX z, COMPLEX c, int x, int y)
{
	int k = BASEITERATIONS;
	pMap[y][x].ed = 0;
	while(k > 0)
	{
		//func(z, c,  pMap[y][x].ed);
		--k;
		func(z, c);
		if(abs(z) > 4.0)
		{
			pMap[y][x].ed = 1;
			break;
		}
	}
#if USE_MPF
	setprec(pMap[y][x].last);
#endif
	pMap[y][x].last = z;
	pMap[y][x].nIter = BASEITERATIONS - k;
	pMap[y][x].calc = 0;
	return pMap[y][x].nIter;
}

int interations = ITERATIONS;
int g_miniter;
inline
int MandelbrotEx(PIXEL& z, COMPLEX& c)
{
	int k = interations;
	//double r = 1.41403398, mr = -r;
	while(k > 0)
	{
		//func(z.last, c, z.ed);
		--k;
		func(z.last, c);
		//if(z.last.real() > r || z.last.real() < mr || z.last.imag() > r || z.last.imag() < mr)
		{
			if(abs(z.last) > 4.0)
			{
				z.ed = 1;
				int it = z.nIter + interations - k;
				if(it > g_miniter || g_miniter == 0)
				{
					g_miniter = it;
				}
				break;
			}
		}
	}
	z.nIter += interations - k;
	return z.nIter;
}


// 绘制 Mandelbrot Set (曼德布洛特集)

void
Draw(Float fromx, Float fromy, Float tox, Float toy, int mode = 0,
	COMPLEX _c = COMPLEX())
{
	unsigned t = clock();
	COMPLEX z, c;
	g_miniter = 0;
	if(mode == 0)
	{
		for(int y = 0; y < SC_H; y++)
		{
			c.imag(fromy + (toy - fromy) * (y / (double)SC_H));
			for(int x = 0; x < SC_W; x++)
			{
				int k;
				c.real(fromx + (tox - fromx) * (x / (double)SC_W));
				z = COMPLEX();
				k = Mandelbrot(z, c, x, y);
				if(pMap[y][x].ed == 0)
					putpixel_f(x, y, BLACK);
				else
					putpixel_f(x, y, Color[k & COLORMASK]);
			}
			if(clock() - t > 50)
			{
				delay(0);
				t = clock();
			}
		}
	}
	else
	{
		c = _c;
		for(int y = 0; y < SC_H; y++)
		{
			for(int x = 0; x < SC_W; x++)
			{
				z = {fromx + (tox - fromx) * (x / (double)SC_W),
					fromy + (toy - fromy) * (y / (double)SC_H)};
				int k = Mandelbrot(z, c, x, y);
				putpixel_f(x, y, pMap[y][x].ed == 0 ? BLACK : Color[k & COLORMASK]);
			}
			if(clock() - t > 100)
			{
				delay(0);
				t = clock();
			}
		}
	}
	for(int y = 0; y < SC_H; y++)
	{
		if(pMap[y][0].ed == 0) pMap[y][0].calc = 1;
		if(pMap[y][SC_W - 1].ed == 0) pMap[y][SC_W - 1].calc = 1;
	}
	for(int x = 0; x < SC_W; x++)
	{
		if(pMap[0][x].ed == 0) pMap[0][x].calc = 1;
		if(pMap[SC_H - 1][x].ed == 0) pMap[SC_H - 1][x].calc = 1;
	}
	for(int y = 1; y < SC_H - 1; y++)
		for(int x = 1; x < SC_W - 1; x++)
		{
			PIXEL& p = pMap[y][x];
			if(p.ed)
			{
				pMap[y - 1][x].calc = 1;
				pMap[y + 1][x].calc = 1;
				pMap[y][x - 1].calc = 1;
				pMap[y][x + 1].calc = 1;
			}
		}
	g_udlist.clear();
	for(int y = 0; y < SC_H; y++)
		for(int x = 0; x < SC_W; x++)
		{
			PIXEL& p = pMap[y][x];
			if(p.calc)
			{
				g_udlist.push(x, y);
			}
			else if(p.ed == 0 && (random(10000)) == 0)
			{
				g_udlist.push(x, y);
				p.calc = 1;
			}
		}
	int x = SC_W / 2, y = SC_H / 2;
	PIXEL& p = pMap[y][x];
	if(p.ed == 0)
	{
		g_udlist.push(x, y);
		p.calc = 1;
	}
	g_udlist.swap();
}

void
addpoint(int x, int y)
{
	if(x < 0 || x >= SC_W || y < 0 || y >= SC_H) return;
	if(pMap[y][x].ed == 0)
	{
		pMap[y][x].calc = 1;
		g_udlist.push(x, y);
	}
}

void
DrawEx(Float fromx, Float fromy, Float tox, Float toy, int mode = 0,
	COMPLEX _c = COMPLEX())
{
	COMPLEX z, c;
	if(mode == 0)
	{
		int x(0), y(0);
		while(g_udlist.pop(&x, &y))
		{
			PIXEL& p = pMap[y][x];
			if(p.ed == 0)
			{
				c = {fromx + (tox - fromx) * (x / (double)SC_W),
					fromy + (toy - fromy) * (y / (double)SC_H)};
				int k = MandelbrotEx(p, c);
				if(p.ed)
				{
					addpoint(x, y - 1);
					addpoint(x, y + 1);
					addpoint(x - 1, y);
					addpoint(x + 1, y);
					putpixel_f(x, y, Color[k & COLORMASK]);
				}
				else
					addpoint(x, y);
			}
		}
	}
	else
	{
		c = _c;
		int x, y;
		while(g_udlist.pop(&x, &y))
		{
			PIXEL& p = pMap[y][x];
			if(p.ed == 0)
			{
				int k = MandelbrotEx(p, c);

				if(p.ed)
				{
					addpoint(x, y - 1);
					addpoint(x, y + 1);
					addpoint(x - 1, y);
					addpoint(x + 1, y);
					putpixel_f(x, y, Color[k & COLORMASK]);
				}
				else
					addpoint(x, y);
			}
		}
	}
	g_udlist.swap();
}

#include <stdio.h>

// 主函数

int
main()
{
	// 初始化绘图窗口及颜色
	int w = SC_W, h = SC_H, th = 12, tel = 4, tl = tel * 3 + 1;
	int rw = 640, rh = 280;
	freopen("log.txt", "w", stdout);
	setinitmode(0);
	initgraph(rw, rh + th * tl);
	randomize();
	InitColor();
	setfont(12, 0, "宋体");
	::SetWindowTextA(getHWnd(), "Mandelbrot Set -- YEGE");
	//mpf_set_prec(100);


	// 初始化 Mandelbrot Set(曼德布洛特集)坐标系
	COMPLEX from{-2.2, -1.65}, to{2.2, 1.65}, from_b, to_b;
#if 0
	do
	{
		FILE* fp = fopen("MandelbrotSet.ini", "r");
		char str[1024]{0};
		COMPLEX center, delta;
		if(fp)
		{
			g_prec = 32 * 80;
			setprec(center);
			setprec(delta);
			setprec(from);
			setprec(to);

			fgets(str, 1000, fp);
			str[strlen(str) - 1] = 0;
			printf("%s\n", str);
			center.real(stoi(str));
			fgets(str, 1000, fp);
			str[strlen(str) - 1] = 0;
			printf("%s\n", str);
			center.imag(stoi(str));
			fgets(str, 1000, fp);
			str[strlen(str) - 1] = 0;
			printf("%s\n", str);
			delta.real() = str;
			fclose(fp);

			delta.real() /= 2;
			delta.imag() = delta.real() * 0.75;
			from.real() = center.real() - delta.real();
			to.real() = center.real() + delta.real();
			from.imag() = center.imag() - delta.imag();
			to.imag() = center.imag() + delta.imag();
			setgprec(delta.real());
			setprec(from);
			setprec(to);
		}
		else
			g_prec = 64;
	}
	while(0);
#endif

	Draw(from.real(), from.imag(), to.real(), to.imag());


	// 捕获鼠标操作，实现放大鼠标选中区域
	mouse_msg m;
	bool isLDown = {};
	COMPLEXI self, selt, self_b, selt_b;    // 定义选区
	COMPLEX js_c;
	int mode = 0;
#if USE_MPF
	mp_bitcnt_t prec_b = 32;
#endif
	//IMAGE img_def, img_ms;
	//img_def.getimage(0, 0, w, h);

	for(;;)
	{
		int bmsg = 0;
		if(kbhit())
		{
			int k = getch();
			bmsg = 1;
			if(k == 'b' || k == 'B')
			{
				Float x1 = from.real() + (from.real() - to.real()) / 2;
				Float x2 = to.real() - (from.real() - to.real()) / 2;
				Float y1 = from.imag() + (from.imag() - to.imag()) / 2;
				Float y2 = to.imag() - (from.imag() - to.imag()) / 2;
				from = {x1, y1};
				to = {x2, y2};
				Draw(from.real(), from.imag(), to.real(), to.imag(), mode, js_c);
			}
			if(k == 's' || k == 'S')
			{
				IMAGE* img = newimage();
				getimage(img, 0, 0, w, h);
				saveimage(img, "m.bmp");
			}
			//*
			if(k == 'r' || k == 'R')
			{
				FILE* fp = fopen("MandelbrotSet.ini", "w");
				if(fp)
				{
					char str[1000];
					Float x = (from.real() + to.real()) * 0.5,
						  y = (from.imag() + to.imag()) * 0.5,
						  d = to.real() - from.real();
#if USE_MPF
					gmp_sprintf(str, "%.500Ff", x.get_mpf_t());
					fprintf(fp, "%s\n", str);
					gmp_sprintf(str, "%.500Ff", y.get_mpf_t());
					fprintf(fp, "%s\n", str);
					gmp_sprintf(str, "%.500Ff", d.get_mpf_t());
#else
					std::sprintf(str, "%.15lf", x);
					fprintf(fp, "%s\n", str);
					std::sprintf(str, "%.15lf", y);
					fprintf(fp, "%s\n", str);
					std::sprintf(str, "%.15lf", d);
#endif
					fclose(fp);
				}
			}//*/
		}
		while(mousemsg())
		{
			bmsg = 1;
			m = getmouse();    // 获取一条鼠标消息

			switch(int(m.msg))
			{
				// 按鼠标右键恢复原图形坐标系
			case mouse_flag_right | mouse_msg_up:
				if(mode == 0)
				{
					from = COMPLEX{-2.2, -1.65};
					to = COMPLEX{2.2, 1.65};
					Draw(from.real(), from.imag(), to.real(), to.imag());
					//img_def.putimage(0, 0);
				}
				else
				{
					from = COMPLEX{-2.0, -1.5};
					to = COMPLEX{2.0, 1.5};
					Draw(from.real(), from.imag(), to.real(), to.imag(), mode, js_c);
				}
				break;

			case mouse_flag_mid | mouse_msg_up:
				mode = 1 - mode;
				if(mode == 0)
				{
#if USE_MPF
					g_prec = prec_b;
					setprec(from);
					setprec(to);
#endif
					self = self_b;
					selt = selt_b;
					from = from_b;
					to = to_b;
					Draw(from.real(), from.imag(), to.real(), to.imag(), mode, js_c);
					//putimage(0, 0, &img_ms);
				}
				else
				{
#if USE_MPF
					setprec(from_b);
					setprec(to_b);
					setprec(js_c);
					prec_b = g_prec;
					g_prec = 64;
#endif
					self_b = self;
					selt_b = selt;
					js_c = {from.real() + (to.real() - from.real()) * self.real() / w,
						from.imag() + (to.imag() - from.imag()) * self.imag() / h};
					from_b = from;
					to_b = to;
					from = COMPLEX{-2.2, -1.65};
					to = COMPLEX{2.2, 1.65};
#if USE_MPF
					setprec(from);
					setprec(to);
#endif
					//img_ms.getimage(0, 0, w, h);
					Draw(from.real(), from.imag(), to.real(), to.imag(), mode, js_c);
				}
				break;
				// 按鼠标左键并拖动，选择区域
			case mouse_msg_move:
				if(isLDown)
				{
					rectangle(self.real(), self.imag(), selt.real(), selt.imag());
					selt = {m.x, m.y};
					rectangle(self.real(), self.imag(), selt.real(), selt.imag());
				}
				{
					char str[200];
					Float x = from.real() + (to.real() - from.real()) * m.x / w,
						  y = from.imag() + (to.imag() - from.imag()) * m.y / h,
						  d = to.real() - from.real();
					setcolor(0xFFFFFF);
#if USE_MPF
					gmp_sprintf(str, "%+.420Ff", x.get_mpf_t());
					outtextrect(0, rh + th * 0, 640, th * tel, str);
					gmp_sprintf(str, "%+.420Ff", y.get_mpf_t());
					outtextrect(0, rh + th * tel, 640, th * tel, str);
					gmp_sprintf(str, "%+.420Ff", d.get_mpf_t());
					outtextrect(0, rh + th * tel * 2, 640, th * tel, str);
					gmp_sprintf(str, "%-6d %9d", mpf_get_prec(y.get_mpf_t()), g_miniter);;
					outtextxy(0, rh + th * tel * 3, str);
#else
					std::sprintf(str, "%+.15lf", x);
					outtextrect(0, rh + th * 0, 640, th * tel, str);
					std::sprintf(str, "%+.15lf", y);
					outtextrect(0, rh + th * tel, 640, th * tel, str);
					std::sprintf(str, "%+.15lf", d);
					outtextrect(0, rh + th * tel * 2, 640, th * tel, str);
#endif
					//::SetWindowTextA(GetHWnd(), str);
				}
				break;

				// 按鼠标左键并拖动，选择区域
			case mouse_flag_left | mouse_msg_down:
				setcolor(WHITE);
				setwritemode(R2_XORPEN);
				isLDown = true;
				selt = {m.x, m.y};
				self = selt;
				rectangle(self.real(), self.imag(), selt.real(), selt.imag());
				break;

				// 按鼠标左键并拖动，选择区域
			case mouse_flag_left | mouse_msg_up:
				if(isLDown == true)
				{
					rectangle(self.real(), self.imag(), selt.real(), selt.imag());
					setwritemode(R2_COPYPEN);
					isLDown = {};
					selt = {m.x, m.y};
					if(self.real() == selt.real() || self.imag() == selt.imag())
						break;
					// 修正选区为 4:3
					if(self.real() > selt.real())
					{
						int tmp = self.real();
						self.real(selt.real());
						selt.real(tmp);
					}
					if(self.imag() > selt.imag())
					{
						int tmp = self.imag();
						self.imag(selt.imag());
						selt.imag(tmp);
					}
					if((selt.real() - self.real()) * 0.75 < (selt.imag() - self.imag()))
					{
						selt.imag(selt.imag() + (3 - (selt.imag() - self.imag()) % 3));
						self.real(self.real() - ((selt.imag() - self.imag()) / 3 * 4 / 2 - (selt.real() - self.real()) / 2));
						selt.real(self.real() + (selt.imag() - self.imag()) / 3 * 4);
					}
					else
					{
						selt.real(selt.real() + (4 - (selt.real() - self.real()) % 4));
						self.imag(self.imag() - ((selt.real() - self.real()) * 3 / 4 / 2 - (selt.imag() - self.imag()) / 2));
						selt.imag(self.imag() + (selt.real() - self.real()) * 3 / 4);
					}

					// 更新坐标系

					// 更新坐标系
					COMPLEX nf(from.real() + (to.real() - from.real()) * self.real() / w, from.imag() + (to.imag() - from.imag()) * self.imag() / h);
					COMPLEX nt(from.real() + (to.real() - from.real()) * selt.real() / w, from.imag() + (to.imag() - from.imag()) * selt.imag() / h);
					from = nf, to = nt;
#if USE_MPF
					Float f = to.real() - from.real();
					setgprec(f);
					mpf_set_default_prec(g_prec);
					setprec(from);
					setprec(to);
#endif

					// 画图形
					Draw(from.real(), from.imag(), to.real(), to.imag(), mode, js_c);
				}
				break;
			}
		}
		if(!bmsg)
		{
#if 1
			for(int n = 0, t = clock(); g_udlist.nLen > 0 && n < 1024; ++n)
			{
				DrawEx(from.real(), from.imag(), to.real(), to.imag(), mode, js_c);
				if(clock() - t > 100) break;
			}
#endif
			delay_fps(1000);
		}
	}
	getch();
	closegraph();
	return 0;
}

