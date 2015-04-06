#include "ege/fpsbase.h"
#include "global.h"

namespace ege
{

using namespace std::chrono;

namespace
{

duration<double>
_get_highfeq_time_ls()
{
#if YEGE_Use_YSLib
	return YSLib::Timers::FetchElapsed();
#else
	return high_resolution_clock::now() - FetchEpoch<high_resolution_clock>();
#endif
}

} // unnamed namespace;

float
_get_FPS(int add)
{
	static int fps = 0;
	static int fps_inv = 0;
	static duration<double> time;
	static float flret = 0;
	static float fret = 0;
	static float fret_inv = 0;
	auto cur(_get_highfeq_time_ls());

	if(add == 0x100)
		++fps;
	else if(add == -0x100)
		++fps, ++fps_inv;
	if(cur - time >= duration<double>(0.5))
	{
		flret = fret;
		fret = float(fps / (cur - time).count());
		fret_inv = float((fps - fps_inv) / (cur - time).count());
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

