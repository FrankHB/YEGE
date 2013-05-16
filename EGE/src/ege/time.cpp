#include "ege/time.h"
#include "global.h"
#include <windows.h>

namespace ege
{

void
ege_sleep(long ms)
{
	if(ms <= 0)
		return;

	static ::HANDLE hTimer = ::CreateWaitableTimer(nullptr, TRUE, nullptr);
	::LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = ms * (::LONGLONG) - 10000;
	if(hTimer)
	{
		if(::SetWaitableTimer(hTimer, &liDueTime, 0, nullptr, nullptr, FALSE))
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
	graph_setting._delay_ms(ms);
}

/*
延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
*/
void
delay_fps(double fps)
{
	graph_setting._delay_fps(fps);
}

/*
延迟1/fps的时间，调用间隔不大于200ms时能保证每秒能返回fps次
*/
void
delay_jfps(double fps)
{
	graph_setting._delay_jfps(fps);
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
	auto pg = &graph_setting;

	if(pg->fclock_start == 0)
		pg->fclock_start = ::GetTickCount();
	return (::GetTickCount() - pg->fclock_start) / 1000.0; //pg->_get_highfeq_time_ls();
}

} // namespace ege;

