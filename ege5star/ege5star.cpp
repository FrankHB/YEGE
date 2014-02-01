// 旋转五角星动画演示程序
#include <graphics.h>
#include <ege/fps.h>
#include <ctime>
#include <cmath>

int
main()
{
	initgraph(640, 480);
	setrendermode(RENDER_MANUAL);

	double r = 0;
	fps f;

	for(; is_run(); delay_fps(100000))
	{
		r += 0.02;
		if(r > PI * 2)
			r -= PI * 2;
		cleardevice();
		setcolor(EGERGB(0xFF, 0xFF, 0xFF));
		setfillcolor(EGERGB(0, 0, 0xFF));
		[](double x, double y, double r, double a){
			int pt[10];

			for(int n = 0; n < 5; ++n)
			{
				pt[n * 2] = -std::cos(PI * 4 / 5 * n + a) * r + x,
				pt[n * 2 + 1] = std::sin(PI * 4 / 5 * n + a) * r + y;
			}
			fillpoly(5, pt);
		}(300, 200, 100, r);
	}
	return 0;
}

