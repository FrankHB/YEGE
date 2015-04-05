#include "ege/time.h"
#include "ege/ctl.h"
#include "ege/viewport.h"
#include "global.h"
#include <thread>
#include <chrono>

namespace ege
{

using namespace std::chrono;
using dw_t = duration<double, std::milli>;

namespace
{

duration<double>
_get_highfeq_time_ls()
{
	return YSLib::Timers::FetchElapsed();
}

dw_t delay_ms_dwLast;
dw_t delay_fps_dwLast;

void
ege_sleep_(dw_t ms)
{
	if(ms > dw_t::zero())
		std::this_thread::sleep_for(ms);
}

} // unnamed namespace;

void
ege_sleep(long ms)
{
	ege_sleep_(dw_t(ms));
}

void
delay_ms(long ms)
{
	if(ms == 0)
	{
		if(update_mark_count < UPDATE_MAX_CALL)
		{
			ege_sleep(1);
			egectrl_root->draw({});
			FetchEGEApplication()._update();
			egectrl_root->update();

			int l, t, r, b, c;

			getviewport(&l, &t, &r, &b, &c);
			setviewport(l, t, r, b, c);
		}
		delay_ms_dwLast = _get_highfeq_time_ls();
	}
	else
	{
		dw_t delay_time(ms);
		dw_t dw(_get_highfeq_time_ls());
		int f(ms >= 50 ? 0 : 100);

		delay_ms_dwLast = dw_t::zero();
		if(delay_ms_dwLast == dw_t::zero())
			delay_ms_dwLast = _get_highfeq_time_ls();
		if(delay_ms_dwLast + dw_t(200) > dw)
			dw = delay_ms_dwLast;

		//ege_sleep(1);
		egectrl_root->draw({});
		while(dw + delay_time >= _get_highfeq_time_ls())
		{
			if(f <= 0 || update_mark_count < UPDATE_MAX_CALL)
			{
				FetchEGEApplication()._update();
				f = 256;
			}
			else
				ege_sleep_(dw + delay_time - _get_highfeq_time_ls());
			--f;
		}
		FetchEGEApplication()._update();
		dw = _get_highfeq_time_ls();
		FetchEGEApplication()._update_GUI();
		egectrl_root->update();
		if(delay_ms_dwLast + dw_t(200) <= dw || delay_ms_dwLast > dw)
			delay_ms_dwLast = dw;
		else
			delay_ms_dwLast += delay_time;
	}
}

// 延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
void
delay_fps(double fps)
{
	dw_t delay_time(1000.0 / fps),
		avg_max_time = delay_time * 10.0; // 误差时间在这个数值以内做平衡
	dw_t dw(_get_highfeq_time_ls());
	int nloop = 0;

	if(delay_fps_dwLast == dw_t::zero())
		delay_fps_dwLast = _get_highfeq_time_ls();
	if(delay_fps_dwLast + delay_time + avg_max_time > dw)
		dw = delay_fps_dwLast;
	egectrl_root->draw({});
	for(; nloop >= 0; --nloop)
	{
		if((dw + delay_time + dw_t(100) >= _get_highfeq_time_ls()))
			do
			{
				ege_sleep_(dw + delay_time - _get_highfeq_time_ls());
			}while(dw + delay_time >= _get_highfeq_time_ls());
		FetchEGEApplication()._update();
		dw = _get_highfeq_time_ls();
		FetchEGEApplication()._update_GUI();
		egectrl_root->update();
		if(delay_fps_dwLast + delay_time + avg_max_time <= dw
			|| delay_fps_dwLast > dw)
			delay_fps_dwLast = dw;
		else
			delay_fps_dwLast += delay_time;
	}
}

// 延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
void
delay_jfps(double fps)
{
	const dw_t delay_time(1000.0 / fps), avg_max_time(delay_time * 10.0);
	dw_t dw = _get_highfeq_time_ls();
	int nloop = 0;

	if(delay_fps_dwLast == dw_t::zero())
		delay_fps_dwLast = _get_highfeq_time_ls();
	if(delay_fps_dwLast + delay_time + avg_max_time > dw)
		dw = delay_fps_dwLast;
	egectrl_root->draw({});
	for(; nloop >= 0; --nloop)
	{
		int bSleep = 0;

		while(dw + delay_time >= _get_highfeq_time_ls())
		{
			ege_sleep_(dw + delay_time - _get_highfeq_time_ls());
			bSleep = 1;
		}
		if(bSleep)
			FetchEGEApplication()._update();
		else
			_get_FPS(-0x100);
		dw = _get_highfeq_time_ls();
		FetchEGEApplication()._update_GUI();
		egectrl_root->update();
		if(delay_fps_dwLast + delay_time + avg_max_time <= dw
			|| delay_fps_dwLast > dw)
			delay_fps_dwLast = dw;
		else
			delay_fps_dwLast += delay_time;
	}
}

double
fclock()
{
	return _get_highfeq_time_ls().count();
}

} // namespace ege;

