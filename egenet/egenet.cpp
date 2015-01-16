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
	vector_t vt[2]{};

	vector_t
	get_power(double x, double y) // double minx=0, double miny=0
	{
		vector_t vt;

		vt.dx = x - this->x;
		vt.dy = y - this->y;
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
	net(int w, int h, int sw, int sh)
		: pt(2)
	{
		double dtw = sw / (double)(w - 1);
		double dth = sh / (double)(h - 1);
		int x, y, l;
		point_t pt;

		this->layer = 0;
		this->w = w;
		this->h = h;
		this->dtw = dtw;
		this->dth = dth;

		for(y = 0; y <= h + 1; ++y)
			for(x = 0; x <= w + 1; ++x)
				for(l = 0; l < 2; ++l)
				{
					this->pt[l][y][x] = pt;
					this->pt[l][y][x].x = (dtw * (x - 1));
					this->pt[l][y][x].y = (dth * (y - 1));
				}
	}

	void
	move_point(int x, int y)
	{
		double ax = 0;
		double ay = 0;
		vector_t vt;
		int i;
		int l = this->layer;
		int dxy[8][2] =
		{
			{ 1, 0},
			{ 0, 1},
			{ -1, 0},
			{ 0, -1},
		};
		point_t* op = &(this->pt[l][y][x]);

		this->pt[l ^ 1][y][x] = *op;
		for(i = 0; i < 2; ++i)
		{
			point_t* p = &this->pt[l][y + dxy[i][1]][x + dxy[i][0]];
			vt = op->get_power(p->x - dxy[i][0] * this->dtw, p->y - dxy[i][1] * this->dth);
			p->vt[i] = vt;
			ax += vt.dx;
			ay += vt.dy;
		}
		ax -= op->vt[0].dx;
		ay -= op->vt[0].dy;
		ax -= op->vt[1].dx;
		ay -= op->vt[1].dy;

		op = &(this->pt[l ^ 1][y][x]);
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
		int x, y = 0, l = this->layer, i = 1;
		vector_t vt;

		for(x = 1; x <= this->w; ++x)
		{
			point_t* p = &this->pt[l][y + 1][x + 0];

			vt = this->pt[l][y][x].get_power(p->x, p->y - this->dth);
			p->vt[i] = vt;
		}
		i = 0;
		for(y = 1; y <= this->h; ++y)
		{
			x = 0;
			{
				point_t* p = &this->pt[l][y][x + 1];

				vt = this->pt[l][y][x].get_power(p->x - this->dtw, p->y);
				p->vt[i] = vt;
				for(x = 1; x <= this->w; ++x)
					move_point(x, y);
			}
		}
		this->layer ^= 1;
	}


	void
	draw_net()
	{
		int x, y, l = this->layer;
		::POINT pt[NET_W + NET_H];

		for(y = 0; y <= this->h; ++y)
		{
			//if(y % g_mod_show == 0)
			{
				for(x = 0; x <= this->w + 1; ++x)
				{
					pt[x].x = (int)(this->pt[l][y][x].x + 0.5);
					pt[x].y = (int)(this->pt[l][y][x].y + 0.5);
					//line_f(this->pt[l][y][x].x, this->pt[l][y][x].y, this->pt[l][y][x+1].x, this->pt[l][y][x+1].y);
				}
				drawbezier(this->w + 2, (int*)pt);
			}
		}
		for(x = 0; x <= this->w; ++x)
			//if(x % g_mod_show == 0)
			{
				for(y = 0; y <= this->h + 1; ++y)
				{
					pt[y].x = (int)(this->pt[l][y][x].x + 0.5);
					pt[y].y = (int)(this->pt[l][y][x].y + 0.5);
					//line_f(this->pt[l][y][x].x, this->pt[l][y][x].y, this->pt[l][y+1][x].x, this->pt[l][y+1][x].y);
				}
				drawbezier(this->h + 2, (int*)pt);
			}
	}

	int cp_x = 0, cp_y = 0, b_cp = 0;

	void
	cap_pt(int px, int py, bool op)
	{
		if(op)
		{
			int y, x, l = this->layer;
			if(b_cp == 0)
			{
				int mx = 1, my = 1;
				double mdis = 1e9, dis;
				for(y = 1; y < this->h; ++y)
				{
					if(y % g_mod_show == 0)
						continue;
					for(x = 1; x < this->w; ++x)
					{
						if(x % g_mod_show == 0)
							continue;
						dis = fabs(px - this->pt[l][y][x].x) + fabs(py - this->pt[l][y][x].y);
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
				this->pt[l][cp_y][cp_x].x = px;
				this->pt[l][cp_y][cp_x].y = py;
				b_cp = 1;
			}
			else
			{
				this->pt[l][cp_y][cp_x].x = px;
				this->pt[l][cp_y][cp_x].y = py;
			}
			this->pt[l][cp_y][cp_x].vx = 0;
			this->pt[l][cp_y][cp_x].vy = 0;
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

	setrendermode(RENDER_MANUAL);
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

