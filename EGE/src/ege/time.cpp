#include "ege/time.h"
#include "ege/ctl.h"
#include "ege/viewport.h"
#include "global.h"
#include <windows.h>

namespace ege
{

float
_get_FPS(int);


double
_get_highfeq_time_ls()
{
	static ::LARGE_INTEGER get_highfeq_time_start;
	static ::LARGE_INTEGER llFeq; /* 此实为常数 */
	::LARGE_INTEGER llNow;

	if(get_highfeq_time_start.QuadPart == 0)
	{
		::SetThreadAffinityMask(::GetCurrentThread(), 0);
		::QueryPerformanceCounter(&get_highfeq_time_start);
		::QueryPerformanceFrequency(&llFeq);
		return 0;
	}
	else
	{
		::QueryPerformanceCounter(&llNow);
		llNow.QuadPart -= get_highfeq_time_start.QuadPart;
		return (double)llNow.QuadPart / llFeq.QuadPart;
	}
}

namespace
{

double delay_ms_dwLast;
double delay_fps_dwLast;

} // unnamed namespace;


void
ege_sleep(long ms)
{
	if(ms <= 0)
		return;

	static ::HANDLE hTimer = ::CreateWaitableTimer({}, TRUE, {});
	::LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = ms * (::LONGLONG) - 10000;
	if(hTimer)
	{
		if(::SetWaitableTimer(hTimer, &liDueTime, 0, {}, {}, {}))
			::WaitForSingleObject(hTimer, INFINITE); // != WAIT_OBJECT_0;
		//::CloseHandle(hTimer);
	}
	else
		::Sleep(ms);
}

void
delay(long ms)
{
	ege_sleep(ms);
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
			get_global_state()._update();
			egectrl_root->update();

			int l, t, r, b, c;

			getviewport(&l, &t, &r, &b, &c);
			setviewport(l, t, r, b, c);
		}
		delay_ms_dwLast = _get_highfeq_time_ls() * 1000.0;
	}
	else
	{
		double delay_time(ms);
		double dw(_get_highfeq_time_ls() * 1000.0);
		int f(ms >= 50 ? 0 : 100);

		delay_ms_dwLast = 0;
		if(delay_ms_dwLast == 0)
			delay_ms_dwLast = _get_highfeq_time_ls() * 1000.0;
		if(delay_ms_dwLast + 200.0 > dw)
			dw = delay_ms_dwLast;

		//ege_sleep(1);
		egectrl_root->draw({});
		while(dw + delay_time >= _get_highfeq_time_ls() * 1000.0)
		{
			if(f <= 0 || update_mark_count < UPDATE_MAX_CALL)
			{
				get_global_state()._update();
				f = 256;
			}
			else
				ege_sleep((int)(dw + delay_time - _get_highfeq_time_ls()
					* 1000.0));
			--f;
		}
		get_global_state()._update();
		dw = _get_highfeq_time_ls() * 1000.0;
		get_global_state()._update_GUI();
		egectrl_root->update();
		if(delay_ms_dwLast + 200.0 <= dw || delay_ms_dwLast > dw)
			delay_ms_dwLast = dw;
		else
			delay_ms_dwLast += delay_time;
	}
}

/*
延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
*/
void
delay_fps(double fps)
{
	double delay_time = 1000.0 / fps;
	double avg_max_time = delay_time * 10.0; // 误差时间在这个数值以内做平衡
	double dw = _get_highfeq_time_ls() * 1000.0;
	int nloop = 0;

	if(delay_fps_dwLast == 0)
		delay_fps_dwLast = _get_highfeq_time_ls() * 1000.0;
	if(delay_fps_dwLast + delay_time + avg_max_time > dw)
		dw = delay_fps_dwLast;
	egectrl_root->draw({});
	for(; nloop >= 0; --nloop)
	{
		if((dw + delay_time + (100.0) >= _get_highfeq_time_ls() * 1000.0))
		{
			do
			{
				ege_sleep((int)(dw + delay_time - _get_highfeq_time_ls()
					* 1000.0));
			} while(dw + delay_time >= _get_highfeq_time_ls() * 1000.0);
		}
		get_global_state()._update();
		dw = _get_highfeq_time_ls() * 1000.0;
		get_global_state()._update_GUI();
		egectrl_root->update();
		if(delay_fps_dwLast + delay_time + avg_max_time <= dw
			|| delay_fps_dwLast > dw)
			delay_fps_dwLast = dw;
		else
			delay_fps_dwLast += delay_time;
	}
}

/*
延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
*/
void
delay_jfps(double fps)
{
	double delay_time = 1000.0 / fps;
	double avg_max_time = delay_time * 10.0;
	double dw = _get_highfeq_time_ls() * 1000.0;
	int nloop = 0;

	if(delay_fps_dwLast == 0)
		delay_fps_dwLast = _get_highfeq_time_ls() * 1000.0;
	if(delay_fps_dwLast + delay_time + avg_max_time > dw)
		dw = delay_fps_dwLast;
	egectrl_root->draw({});
	for(; nloop >= 0; --nloop)
	{
		int bSleep = 0;

		while(dw + delay_time >= _get_highfeq_time_ls() * 1000.0)
		{
			ege_sleep((int)(dw + delay_time - _get_highfeq_time_ls()
				* 1000.0));
			bSleep = 1;
		}
		if(bSleep)
			get_global_state()._update();
		else
			_get_FPS(-0x100);
		dw = _get_highfeq_time_ls() * 1000.0;
		get_global_state()._update_GUI();
		egectrl_root->update();
		if(delay_fps_dwLast + delay_time + avg_max_time <= dw
			|| delay_fps_dwLast > dw)
			delay_fps_dwLast = dw;
		else
			delay_fps_dwLast += delay_time;
	}
}

void
api_sleep(long dwMilliseconds)
{
	if(dwMilliseconds >= 0)
		::Sleep(dwMilliseconds);
}

double
fclock()
{
	static ::DWORD fclock_start;

	if(fclock_start == 0)
		fclock_start = ::GetTickCount();
	return (::GetTickCount() - fclock_start) / 1000.0; //get_highfeq_time_ls();
}

} // namespace ege;

