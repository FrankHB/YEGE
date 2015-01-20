// julia集计算屏保动画（编译后改为scr后缀使用）
#include "graphics.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <complex>

using std::size_t;
using COMPLEX = std::complex<double>;

template<typename _type, size_t _vBase>
struct log_base
{
	static yconstexpr const _type value = log(_type(_vBase));
};

template<typename _type, size_t _vBase>
yconstfn _type
log_n(_type d)
{
	return log(d) / log_base<_type, _vBase>::value;
}

yconstexpr size_t BF_W(1200), BF_H(1200);

struct state
{
	int iter;
	bool ed;
	COMPLEX z;
	COMPLEX c;

	void
	MandelbrotEx()
	{
		if(iter < 64)
		{
			int k = 64, b = k;

			while(k > 0)
			{
				--k;
				z = z * z + c;
				if(std::norm(z) > 4.0)
				{
					ed = true;
					break;
				}
			}
			iter += b - k;
		}
	}
};

struct updatelist
{
	::POINT* p, *pn;
	::POINT m_list[2][1920 * 1920];
	int nBeg, nLen;
	int nLen_n;

	updatelist()
	{
		clear();
	}

	void
	clear()
	{
		p = m_list[0];
		pn = m_list[1];
		nBeg = nLen = nLen_n = 0;
	}

	void
	push(int x, int y)
	{
		pn[nLen_n].x = x, pn[nLen_n].y = y;
		++nLen_n;
	}

	int
	pop(int* x, int* y)
	{
		if(nBeg == nLen) return 0;
		*x = p[nBeg].x, *y = p[nBeg].y;
		++nBeg;
		return 1;
	}

	void
	flip() ynothrow
	{
		nBeg = 0;
		nLen = nLen_n;
		nLen_n = 0;
		std::swap(p, pn);
	}
};


class gstate
{
private:
	const double bilout = 256;
	const double col_ins = 1.0 / bilout / bilout * 256 * 32;
	double logmap[(18 * 18) * 32];
	size_t width, height;
	unique_ptr<state[]> states;
	state* g_st;
	std::array<double, 6> colors{{28, 16, 32, 0, 0, 128}};
	double sr, cr;

public:
	int g_mi[BF_H][BF_W];
	state pgmap[BF_H][BF_W];
	updatelist udlist;

	gstate()
		: width(getwidth({})), height(getheight({})),
		states(new state[width * height]), g_st(&states[0])
	{
		for(int i = 0; i < (18 * 18) * 32; ++i)
			logmap[i] = 1 - log_n<double, 2>(log_n<double, 2>(i / col_ins) / 2);
	}

	double
	GetRatio() const ynothrow
	{
		return width / double(height);
	}

	void
	init_states()
	{
		double dc = 64, dca = 128, db = 16;

		colors = {randomf() * dc + db, randomf() * dc + db, randomf() * dc + db,
			randomf() * dca, randomf() * dca, randomf() * dca};

		double rotate = randomf() * 3.14169265359 * 2;

		sr = sin(rotate), cr = cos(rotate);
		std::memset(g_st, 0, width * height * sizeof(state));
	}

	static int
	iterToColor(double iter)
	{
		return int(std::fabs(fmod(iter + 255, 510) - 255));
	}

	void
	loop4()
	{
		udlist.flip();
		for(size_t y = 0; y < height; ++y)
			for(size_t x = 0; x < width; ++x)
				if(!g_st[y * width + x].ed)
					udlist.push(x, y);
		udlist.flip();
	}

	::COLORREF
	colorMap(COMPLEX z, int iteration)
	{
		const double r(iteration + logmap[size_t(std::norm(z) * col_ins)]);

		return RGB(iterToColor(r * colors[0] + colors[3]), iterToColor(r
			* colors[1] + colors[5]), iterToColor(r * colors[2] + colors[4]));
	}

	void
	addpoint(int x, int y)
	{
		if(x < 0 || x >= int(BF_W) || y < 0 || y >= int(BF_H))
			return;
		if(!pgmap[y][x].ed)
			udlist.push(x, y);
	}

	int
	MDraw(double fromx, double fromy, double tox, double toy)
	{
		std::clock();

		int ret = 0;

		int x, y;
		while(udlist.pop(&x, &y))
		{
			state& p = pgmap[y][x];
			if(p.iter == 0 && !p.ed)
			{
				p.c = {fromx + (tox - fromx) * (x / (double)BF_W),
					fromy + (toy - fromy) * (y / (double)BF_H)};
				p.z = 0;
			}
			if(!p.ed)
			{
				p.MandelbrotEx();
				if(p.ed)
				{
					++ret;
					addpoint(x, y - 1);
					addpoint(x, y + 1);
					addpoint(x - 1, y);
					addpoint(x + 1, y);
					g_mi[y][x] = p.iter;
				}
				else
					addpoint(x, y);
			}
			if(kbmsg())
				break;
		}
		udlist.flip();
		return ret;
	}

	int g_updatepoint = 0;

	// 绘制 Julia Set
	size_t
	JDraw(COMPLEX c, double fromx, double fromy, double tox, double toy)
	{
		size_t ret(0);
		state* st(&states[0]);
		clock_t tt = std::clock();

		g_updatepoint = 0;
		for(size_t y = 0; y < height; ++y)
		{
			for(size_t x = 0; x < width; ++x, ++st)
			{
				if(st->ed)
					continue;
				COMPLEX& z = st->z;

				if(st->iter == 0)
				{
					const double re = fromx + (tox - fromx)
						* (x / double(width)),
						im = fromy + (toy - fromy) * (y / double(height));

					z = {cr * re + sr * im, sr * re - cr * im};
				}
			//	else
			//		z = st->z;
				++st->iter;
				z = z * z + c;
				if(std::norm(z) > bilout)
					st->ed = true;
				++ret;
				if(st->ed)
				{
					putpixel(x, y, colorMap(z, st->iter));
					g_updatepoint += 1;
				}
				else if(st->iter == 1)
					putpixel_f(x, y, 0);
			}
			if(std::clock() - tt > 10)
			{
				tt = std::clock();
				if(kbmsg())
					return -1;
			}
		}
		return ret;
	}

	size_t
	JDrawA(COMPLEX c)
	{
		std::clock();
		g_updatepoint = 0;

		size_t ret = 0;
		int x, y;

		while(udlist.pop(&x, &y))
		{
			auto& p(states[y * width + x]);
			if(p.ed == 0)
			{
				auto& z(p.z);

				z = z * z + c;
				if(std::norm(z) > bilout)
					p.ed = 1;
				++p.iter, ++ret;
				if(p.ed)
				{
					putpixel(x, y, colorMap(p.z, p.iter));
					g_updatepoint += 1;
				}
				else
					udlist.push(x, y);
			}
		}
		udlist.flip();
		return ret;
	}
};


int
main()
{
	setinitmode(0x005, 0, 0);
	//initgraph(320, 240);
	initgraph(-1, -1);
	showmouse(0);
	flushmouse();
	while(kbhit())
		getch();
	COMPLEX c{0.262, 0.002}, z{0, 0};
	int n_update = 0;
	auto p_gs(make_unique<gstate>());
	auto& gs(*p_gs);
	double ftime = fclock(), r = 1.5, d = gs.GetRatio();

	gs.init_states();
	for(int loop = 1; kbmsg() == 0; ++loop)
	{
		int ret;

		if(loop <= 4)
		{
			ret = gs.JDraw(c, z.real() - r * d, z.imag() - r, z.real() + r * d,
				z.imag() + r);
			if(loop == 4)
				gs.loop4();
		}
		else
		{
			static int t;

			ret = gs.JDrawA(c);
			if(std::clock() - t > 30)
			{
				delay(1);
				t = std::clock();
			}
		}
		if(gs.g_updatepoint == 0)
			++n_update;
		else
			n_update = 0;
		if(ret == 0 || n_update > 8 || loop > 1000)
		{
			loop = 0;

			if(gs.g_mi[0][0] == 0)
			{
				delay(1);
				std::memset(gs.pgmap, 0, BF_W * BF_H * sizeof(state));
				gs.udlist.clear();
				for(size_t i(0); i < BF_W; ++i)
				{
					gs.addpoint(i, 0);
					gs.addpoint(i, BF_H - 1);
				}
				for(size_t i(0); i < 4; )
				{
					if(gs.MDraw(-1.9, -1.2, 0.5, 1.2) == 0)
						++i;
					else
						i = 0;
					if(kbmsg())
						return 0;
				}
			}
			gs.init_states();
			do
			{
				c = {randomf() * 2.4 - 1.9, randomf() * 2.4 - 1.2};
			}
			while(gs.g_mi[(int)((c.imag() - (-1.2)) / (1.2 - (-1.2)) * BF_H)]
				[(int)((c.real() - (-1.9)) / (0.5 - (-1.9)) * BF_W)] < 16);
			n_update = 0;
			if(fclock() - ftime < 3)
				delay_ms((int)((3 - (fclock() - ftime)) * 1000));
			else
				delay(1);
			ftime = fclock();
		}
	}
	closegraph();
}

