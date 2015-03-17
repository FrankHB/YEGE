// 指针式时钟显示+抗锯齿演示
#include <ege.h>
#include <ctime>
#include <cmath>
#include <cstdio>

using namespace ege;

ege_point getpos(ege_point center, float rad, float r)
{
	ege_point pos;
	pos.x = std::sin(rad) * r + center.x;
	pos.y = -std::cos(rad) * r + center.y;
	return pos;
}

void draw()
{
	float pi2 = PI * 2;
	ege_point center = {200, 200};
	float r = 150;

	settextjustify(CENTER_TEXT, CENTER_TEXT);
	setfont(24, 0, "Courier New");
	setbkmode(TRANSPARENT);
	ege_enable_aa(true);
	setfillcolor(EGEARGB(0xff, 0x40, 0x40, 0x40));
	setcolor(EGEARGB(0xff, 0x80, 0x00, 0xf0));
	ege_fillellipse(center.x - r * 1.2f, center.y - r * 1.2f, r * 1.2f * 2.0f,
		r * 1.2f * 2.0f);
	setcolor(WHITE);
	for(int num = 1; num <= 12; ++num)
	{
		char str[8];
		ege_point p = getpos(center, float(num * pi2 / 12), r);
		std::sprintf(str, "%d", num);
		outtextxy((int)p.x, (int)p.y, str);
	}

	time_t t_now;

	time(&t_now);

	tm* t = localtime(&t_now);

	setcolor(EGEARGB(0xff, 0x0, 0x0, 0xff));
	setlinewidth(10.0f);

	float h = float(t->tm_hour + t->tm_min / 60.0);

	{
		ege_point p = getpos(center, float(h * pi2 / 12), r * 0.5f);

		ege_line(p.x, p.y, center.x, center.y);
	}
	setcolor(EGEARGB(0xff, 0xff, 0x0, 0xff));
	setlinewidth(5.0f);

	float m = float(t->tm_min + t->tm_sec / 60.0);

	{
		ege_point p = getpos(center, float(m * pi2 / 60), r * 0.9f);

		ege_line(p.x, p.y, center.x, center.y);
	}
	setcolor(EGEARGB(0xff, 0xff, 0xff, 0));
	setfillcolor(EGEARGB(0xff, 0xff, 0xff, 0));
	setlinewidth(1.0f);

	float s = float(t->tm_sec);

	{
		ege_point p = getpos(center, float(s * pi2 / 60), r * 1.0f);

		ege_line(p.x, p.y, center.x, center.y);
	}
	ege_fillellipse(center.x - r * 0.05f, center.y - r * 0.05f,
						 r * 0.1f, r * 0.1f);

	char str[32];

	std::sprintf(str, "%d/%02d/%02d %2d:%02d:%02d", t->tm_year + 1900,
		t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	setcolor(EGERGB(0xff, 0xff, 0));
	outtextxy((int)center.x, (int)(center.y + r * 1.4f), str);
}

int main()
{
	setinitmode(INIT_ANIMATION);
	initgraph(400, 480);
	randomize();
	for(; is_run(); delay_fps(60))
	{
		cleardevice();
		draw();
	}
	closegraph();
}


