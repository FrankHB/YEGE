#include "ege/fpsbase.h"
#include "global.h"

namespace ege
{

double
_get_highfeq_time_ls();

float
_get_FPS(int add)
{
	static int fps = 0;
	static int fps_inv = 0;
	static double time = 0;
	static float flret = 0;
	static float fret = 0;
	static float fret_inv = 0;

	double cur = _get_highfeq_time_ls();
	if(add == 0x100)
		++fps;
	else if(add == -0x100)
		++fps, ++fps_inv;
	if(cur - time >= 0.5)
	{
		flret = fret;
		fret = float(fps / (cur - time));
		fret_inv = float((fps - fps_inv) / (cur - time));
		fps = 0;
		fps_inv = 0;
		time = cur;
	}
	return add > 0 ? (fret + flret) / 2 : fret_inv;
}


float
getfps()
{
	return _get_FPS(0);
}

} // namespace ege;

