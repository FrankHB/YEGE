// 弹球物理碰撞演示
#include <graphics.h>
#include <ege/fps.h>
#include <cmath>
#include <vector>
#include <algorithm>

template<typename _type>
_type
square(_type x)
{
	return x * x;
}

template<typename _type>
_type
dist(_type x1, _type y1, _type x2, _type y2)
{
	return square(x1 - x2) + square(y1 - y2);
}

struct Obj
{
	float x, y;
	float vx = 0.F, vy = 0.F;
	unsigned r;
	float m;
	int color;

	Obj()
		: color(hsv2rgb(random(10000) * 360.F / 10000, 1.F, 1.F))
	{}

	void
	init_xyr()
	{
		do{
			x = random(getwidth());
			y = random(getheight());
			r = random(40) + 20;
		}while(x < r || x >= 640 - r || y < r || y >= 480 - r);
		m = square(r);
	}

	void
	draw() const
	{
		setfillcolor(color);
		ege_fillellipse(x - r, y - r, r * 2, r * 2);
	}

	void
	update()
	{
		vy += 0.05F;
		x += vx;
		y += vy;
		if(y >= 480 - r && vy > 0)
		{
			y -= vy;
			vy = -vy;
		}
		if(x < r && vx < 0)
			vx = -vx;
		if(x >= 640 - r && vx > 0)
			vx = -vx;
	}
};

bool
crash(const Obj& a, const Obj& b)
{
	return dist(a.x, a.y, b.x, b.y) < square(a.r + b.r);
}

void
check_crash(Obj& a, Obj& b)
{
	const float mdiff(a.m - b.m), msum(a.m + b.m);
	const float sx(a.x - b.x), sy(a.y - b.y), hysxy(hypot(sx, sy)),
		s1x(sx / hysxy), s1y(sy / hysxy), t1x(-s1y), t1y(s1x),
		vas(a.vx * s1x + a.vy * s1y), vat(a.vx * t1x + a.vy * t1y),
		vbs(b.vx * s1x + b.vy * s1y), vbt(b.vx * t1x + b.vy * t1y),
		vasf((2 * b.m * vbs + vas * mdiff) / msum),
		vbsf((2 * a.m * vas - vbs * mdiff) / msum);

	a.vx = vasf * s1x + vat * t1x;
	a.vy = vasf * s1y + vat * t1y;
	b.vx = vbsf * s1x + vbt * t1x;
	b.vy = vbsf * s1y + vbt * t1y;
}

struct AniObj
{
	std::vector<Obj> objs;

	AniObj(size_t num)
		: objs(num)
	{
		for(auto& a : objs)
			do
			{
				a.init_xyr();
			}while(std::any_of(&objs[0], &a, [&a, this](Obj& b){
				return crash(a, b);
			}));
	}

	void
	update()
	{
		for(auto& o : objs)
			o.update();
		for(auto& a : objs)
			std::for_each(&objs[0], &a, [&a, this](Obj& b){
				if(crash(a, b) && (dist(a.x, a.y, b.x, b.y)
					> dist(a.x + a.vx, a.y + a.vy, b.x + b.vx, b.y + b.vy)))
					check_crash(a, b);
			});
		cleardevice();
		for(auto& o : objs)
			o.draw();
	}
};


int
main()
{
	setinitmode(INIT_ANIMATION);
	initgraph(640, 480);

	AniObj ani(8);
	fps f;

	ege_enable_aa(true);
	for(; is_run(); delay_fps(120))
		ani.update();
	closegraph();
}

