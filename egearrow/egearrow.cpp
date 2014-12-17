// 画箭头算法演示小程序
#include <graphics.h>
#include <cmath>

int
main()
{
	initgraph(640, 480);
	ege_enable_aa(true);
	setcolor(EGEARGB(0xFF, 0xFF, 0xFF, 0xFF));
	setfillcolor(EGEARGB(0xFF, 0xFF, 0x0, 0xFF));
	setlinewidth(2.0f);
	[](float sx, float sy, float ex, float ey, float r, float len){
		const float c(std::cos(r)), s(std::sin(r)),
			dx(sx - ex), dy(sy - ey);
		ege_line(sx, sy, ex, ey);

		ege_point points[3]{ex, ey, len * (dx * c + dy * s) + ex,
			len * (-dx * s + dy * c) + ey, len * (dx * c - dy * s) + ex,
			len * (dx * s + dy * c) + ey};

		ege_fillpoly(3, points);
	}(100.0F, 100.0F, 300.0F, 150.0F, PI / 8, 0.2F);
	getch();
}

