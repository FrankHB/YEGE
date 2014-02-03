// 弹球物理碰撞演示
#include <graphics.h>
#include <ege/fps.h>
#include <cmath>

struct Obj
{
	float x, y;
	float vx, vy;
	int r;
	int color;
};


//定义一个AniObj类
class AniObj
{
private:
	Obj obj[20];
	int n = 8;

public:
	AniObj()
	{
		for(int i = 0; i < n; i++)
		{
			bool goon;

			do
			{
				goon = {};
				obj[i].x = random(getwidth());
				obj[i].y = random(getheight());
				obj[i].r = random(40) + 20;
				if(obj[i].x < obj[i].r || obj[i].x >= 640 - obj[i].r
					|| obj[i].y < obj[i].r || obj[i].y >= 480 - obj[i].r)
					goon = true;
				else if(i != 0)
					for(int j = i - 1; j >= 0; j--)
						if(IsCrash(obj[i], obj[j]))
						{
							goon = true;
							break;
						}
			}while(goon);
			obj[i].vx = obj[i].vy = 0.F;
			obj[i].color = EGEAGRAY(0xFF,
				HSVtoRGB(random(10000) * 360.F / 10000, 1.F, 1.F));
		}
	}

	//更新位置等相关属性
	void
	updateobj()
	{
		int i, j;
		for(i = 0; i < n; i++)
		{
			obj[i].vy += 0.05f;
			obj[i].x += obj[i].vx;
			obj[i].y += obj[i].vy;
			if(obj[i].y >= 480 - obj[i].r && obj[i].vy > 0)
			{
				obj[i].y -= obj[i].vy;
				obj[i].vy = - obj[i].vy;
			}
			if(obj[i].x < obj[i].r && obj[i].vx < 0)
				obj[i].vx = - obj[i].vx;
			if(obj[i].x >= 640 - obj[i].r && obj[i].vx > 0)
				obj[i].vx = - obj[i].vx;
		}
		for(i = 1; i < n; i++)
			for(j = i - 1; j >= 0; j--)
				if(IsCrash(obj[i], obj[j])
					&& (Distance(obj[i].x, obj[i].y, obj[j].x, obj[j].y)
					> Distance(obj[i].x + obj[i].vx, obj[i].y + obj[i].vy,
					obj[j].x + obj[j].vx, obj[j].y + obj[j].vy)))
					Crash(obj[i], obj[j]);
	}

	//根据属性值绘画
	void
	drawobj()
	{
		for(int i = 0; i < n; i++)
		{
			setfillcolor(obj[i].color);
			ege_fillellipse(obj[i].x - obj[i].r, obj[i].y - obj[i].r,
				obj[i].r * 2, obj[i].r * 2);
		}
	}

private:
	static bool
	IsCrash(const Obj& a, const Obj& b)
	{
		return Distance(a.x, a.y, b.x, b.y) < (a.r + b.r) * (a.r + b.r);
	}

	static float
	Distance(float x1, float y1, float x2, float y2)
	{
		return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
	}

	void
	Crash(Obj& a, Obj& b)
	{
		float ma(a.r * a.r), mb(b.r * b.r), sx(a.x - b.x), sy(a.y - b.y),
			s1x(sx / sqrt(sx * sx + sy * sy)),
			s1y(sy / sqrt(sx * sx + sy * sy)),
			t1x(-s1y), t1y(s1x), vas(a.vx * s1x + a.vy * s1y),
			vat(a.vx * t1x + a.vy * t1y), vbs(b.vx * s1x + b.vy * s1y),
			vbt(b.vx * t1x + b.vy * t1y),
			vasf((2 * mb * vbs + vas * (ma - mb)) / (ma + mb)),
			vbsf((2 * ma * vas - vbs * (ma - mb)) / (ma + mb)), nsx(vasf * s1x),
			nsy(vasf * s1y), ntx(vat * t1x), nty(vat * t1y);

		a.vx = nsx + ntx;
		a.vy = nsy + nty;
		nsx = vbsf * s1x;
		nsy = vbsf * s1y;
		ntx = vbt * t1x;
		nty = vbt * t1y;
		b.vx = nsx + ntx;
		b.vy = nsy + nty;
	}
};


int
main()
{
	setinitmode(INIT_ANIMATION);
	initgraph(640, 480);
	randomize(); //初始化随机种子

	AniObj aniobj; //定义对象
	fps f;

	ege_enable_aa(true);
	for(; is_run(); delay_fps(120))
	{
		aniobj.updateobj(); //更新位置
		cleardevice();
		aniobj.drawobj(); //绘画
	}
	closegraph();
}

