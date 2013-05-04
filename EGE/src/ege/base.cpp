#include "ege/base.h"
#include "global.h"
#include "ege/time.h"
#include "head.h" // for RENDER_TIME_ID
#include <windows.h>

namespace ege
{

void
setrendermode(rendermode_e mode)
{
	if(mode == RENDER_MANUAL)
	{
		auto pg = &graph_setting;
		if(!pg->lock_window)
		{
			::KillTimer(pg->hwnd, RENDER_TIMER_ID);
			pg->timer_stop_mark = true;
			::PostMessageW(pg->hwnd, WM_TIMER, RENDER_TIMER_ID, 0);
			pg->lock_window = true;
			while(pg->timer_stop_mark)
				::Sleep(1);
		}
	}
	else
	{
		auto pg = &graph_setting;
		delay_ms(0);
		::SetTimer(pg->hwnd, RENDER_TIMER_ID, 0, nullptr);
		pg->skip_timer_mark = false;
		pg->lock_window = false;
	}
}

} // namespace ege
