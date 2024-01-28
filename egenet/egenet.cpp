//碧波荡漾（鼠标拖动弹力物理模拟演示）
#include <graphics.h>
#include <ege/fps.h>

#include <cmath>
#include <ctime>
#include <cstdio>
#include <vector>
#include <array>

double s_sqrt(double d)
{
	return d < 0 ? -sqrt(-d) : sqrt(d);
}

double
s_pow(double a, double p)
{
	if(fabs(a) < 1)
		return 0;
	if(a < 0)
		return -pow(-a, p);
	return pow(a, p);
}

double
s_minus(double a, double b)
{
	if(b >= 0)
	{
		if(a >= b)
			return a - b;
		if(a <= -b)
			return a + b;
		return 0;
	}
	else
		return a >= 0 ? a - b : a + b;
}

const int g_mod_show = 3;
const double g_d_friction = 0.001;
const double g_k = 0.03; /* 劲度系数，不能大于等于0.5 */

struct vector_t
{
	double dx = 0;
	double dy = 0;
};

struct point_t
{
	double x = 0;
	double y = 0;
	double vx = 0;
	double vy = 0;
	double ax = 0;
	double ay = 0;
	vector_t v[2]{};

	vector_t
	get_power(double xx, double yy) // double minx=0, double miny=0
	{
		vector_t vt;

		vt.dx = xx - x;
		vt.dy = yy - y;
		//vt.dx = s_minus(vt.dx, minx);
		//vt.dy = s_minus(vt.dy, miny);
		vt.dx *= g_k;
		vt.dy *= g_k;
		//vt.dx *= fabs(vt.dx);
		//vt.dy *= fabs(vt.dy);
		return vt;
	}
};

using std::array;
using std::vector;

const int NET_W = 256;
const int NET_H = 256;

class net
{
private:
	vector<array<array<point_t, NET_W>, NET_H>> pt;
	int layer = 0;
	int w = 0;
	int h = 0;
	double dtw = 0;
	double dth = 0;
	double dmw = 0;
	double dmh = 0;

public:
	net(int b, int sw, int sh)
		: net(b * 4, b * 3, sw, sh)
	{}
	net(int w_, int h_, int sw, int sh)
		: pt(2)
	{
		const double dtw_ = sw / double(w_ - 1), dth_ = sh / double(h_ - 1);
		int x, y, l;
		point_t pt_;

		layer = 0;
		w = w_;
		h = h_;
		dtw = dtw_;
		dth = dth_;

		for(y = 0; y <= h_ + 1; ++y)
			for(x = 0; x <= w_ + 1; ++x)
				for(l = 0; l < 2; ++l)
				{
					pt[l][y][x] = pt_;
					pt[l][y][x].x = dtw_ * (x - 1);
					pt[l][y][x].y = dth_ * (y - 1);
				}
	}

	void
	move_point(int x, int y)
	{
		double ax = 0;
		double ay = 0;
		vector_t vt;
		int i;
		int l = layer;
		int dxy[8][2] =
		{
			{1, 0},
			{0, 1},
			{-1, 0},
			{0, -1},
		};
		point_t* op = &pt[l][y][x];

		pt[l ^ 1][y][x] = *op;
		for(i = 0; i < 2; ++i)
		{
			point_t* p = &pt[l][y + dxy[i][1]][x + dxy[i][0]];
			vt = op->get_power(p->x - dxy[i][0] * dtw,
				p->y - dxy[i][1] * dth);
			p->v[i] = vt;
			ax += vt.dx;
			ay += vt.dy;
		}
		ax -= op->v[0].dx;
		ay -= op->v[0].dy;
		ax -= op->v[1].dx;
		ay -= op->v[1].dy;

		op = &pt[l ^ 1][y][x];
		op->ax = ax;
		op->ay = ay;

		op->vx += op->ax;
		op->vy += op->ay;

		op->x += op->vx;
		op->y += op->vy;

		op->vx *= 1 - g_d_friction;
		op->vy *= 1 - g_d_friction;
		//op->vx = s_minus(op->vx, g_d_friction);
		//op->vy = s_minus(op->vy, g_d_friction);
	}

	void
	move_net()
	{
		int x, y = 0, l = layer, i = 1;
		vector_t vt;

		for(x = 1; x <= w; ++x)
		{
			point_t* p = &pt[l][y + 1][x + 0];

			vt = pt[l][y][x].get_power(p->x, p->y - dth);
			p->v[i] = vt;
		}
		i = 0;
		for(y = 1; y <= h; ++y)
		{
			x = 0;
			{
				point_t* p = &pt[l][y][x + 1];

				vt = pt[l][y][x].get_power(p->x - dtw, p->y);
				p->v[i] = vt;
				for(x = 1; x <= w; ++x)
					move_point(x, y);
			}
		}
		layer ^= 1;
	}


	void
	draw_net()
	{
		int x, y, l = layer;
		::POINT pt_[NET_W + NET_H];

		for(y = 0; y <= h; ++y)
		{
			//if(y % g_mod_show == 0)
			{
				for(x = 0; x <= w + 1; ++x)
				{
					pt_[x].x = int(pt[l][y][x].x + 0.5);
					pt_[x].y = int(pt[l][y][x].y + 0.5);
					//line_f(pt[l][y][x].x, pt[l][y][x].y,
					//	pt[l][y][x+1].x, pt[l][y][x+1].y);
				}
				drawbezier(w + 2, reinterpret_cast<int*>(pt_));
			}
		}
		for(x = 0; x <= w; ++x)
			//if(x % g_mod_show == 0)
			{
				for(y = 0; y <= h + 1; ++y)
				{
					pt_[y].x = int(pt[l][y][x].x + 0.5);
					pt_[y].y = int(pt[l][y][x].y + 0.5);
					//line_f(pt[l][y][x].x, pt[l][y][x].y,
					//	pt[l][y+1][x].x, pt[l][y+1][x].y);
				}
				drawbezier(h + 2, reinterpret_cast<int*>(pt_));
			}
	}

	int cp_x = 0, cp_y = 0, b_cp = 0;

	void
	cap_pt(int px, int py, bool op)
	{
		if(op)
		{
			int y, x, l = layer;
			if(b_cp == 0)
			{
				int mx = 1, my = 1;
				double mdis = 1e9, dis;
				for(y = 1; y < h; ++y)
				{
					if(y % g_mod_show == 0)
						continue;
					for(x = 1; x < w; ++x)
					{
						if(x % g_mod_show == 0)
							continue;
						dis = fabs(px - pt[l][y][x].x)
							+ fabs(py - pt[l][y][x].y);
						if(dis < mdis)
						{
							mx = x;
							my = y;
							mdis = dis;
						}
					}
				}
				cp_x = mx;
				cp_y = my;
				pt[l][cp_y][cp_x].x = px;
				pt[l][cp_y][cp_x].y = py;
				b_cp = 1;
			}
			else
			{
				pt[l][cp_y][cp_x].x = px;
				pt[l][cp_y][cp_x].y = py;
			}
			pt[l][cp_y][cp_x].vx = 0;
			pt[l][cp_y][cp_x].vy = 0;
		}
		else
			b_cp = 0;
	}
};

int
main()
{
//	int g = TRUECOLORSIZE, m = (g_height << 16) | g_width;

//	initgraph(&g, &m, "碧波荡漾");
//	setinitmode(3);
	initgraph(640, 480);
	setbkmode(TRANSPARENT);

	net g_net(20, getwidth(), getheight());
	char str[100] = "";
	fps fps_obj;

	signal(SIGABRT, [](int){
		;
	});

	for(; kbhit() == 0; delay_fps(60))
	{
		cleardevice();
		setcolor(0x8000);
		g_net.draw_net();
		setcolor(0xFFFFFF);
		outtextxy(0, 0, str);
		g_net.move_net();
		g_net.move_net();

		int x, y;

		mousepos(&x, &y);
		g_net.cap_pt(x, y, keystate(key_mouse_l) != 0);
	}
	closegraph();
}

