// 烟花特效演示

#include <graphics.h>
#include <ctime>
#include <cmath>

#define myrand(m) ((float)(randomf() * m ))

struct Point
{
	float x, y;
	float vx, vy;
	int color;
};

class AniObj
{
public:
	//初始化，设置坐标
	AniObj()
	{
		Init();
	}

	void Init()
	{
		n = 100;
		float x = myrand(600.0f) + 20.0f;
		float y = myrand(100.0f) + 100.0f;
		for(int i = 0; i < n; i++)
		{
			p[i].x = x;
			p[i].y = y;
			p[i].vx = 1.0f - myrand(2.0f);
			p[i].vy = 1.0f - myrand(2.0f);
			p[i].color = hsv2rgb(myrand(360.0f), 1.0f, 1.0f);
		}
		color = hsv2rgb(myrand(360.0f), 1.0f, 1.0f);
		start = rand() % 300;
		cnt = 0;
	}

	//更新位置等相关属性
	void updateobj()
	{
		if(cnt++ > start)
			for(int i = 0; i < n; i++)
			{
				p[i].vy += 0.01f;
				p[i].x += p[i].vx;
				p[i].y += p[i].vy;
			}
		if(cnt > start + 400) Init();
	}

	//根据属性值绘画
	void drawobj()
	{
		for(int i = 0; i < n; i++)
		{
			putpixel(p[i].x, p[i].y, color);
		}
	}

	//释放这个对象时调用
	~AniObj()
	{
	}

private:
	Point p[100];
	int n;
	int color;
	int start;
	int cnt;
};


#define MAXOBJ 20

int main()
{
	initgraph(640, 480);

	AniObj obj[MAXOBJ]; //定义对象数组
	int n;

	for(; kbhit() == 0; delay_fps(60))
	{
		for(n = 0; n < MAXOBJ; ++n)
			obj[n].updateobj(); //更新位置
		imagefilter_blurring({}, 0x4F, 0x100);
		for(n = 0; n < MAXOBJ; ++n)
			obj[n].drawobj(); //绘画
	}

	closegraph();
}

