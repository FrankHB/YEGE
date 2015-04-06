// EGE 全局状态
#ifndef Inc_ege_global_h_
#define Inc_ege_global_h_

#include "msg.h"
#include "ege/proc.h"
#include "ege/input.h"
#include "ege/env.h"
#include "ege/base.h"
#include <chrono>
#include <thread>
#include "thread_queue.h"
#if YEGE_Use_YSLib
#	include YFM_Helper_HostedUI
#	include YFM_Helper_Environment
#else
#	include <windows.h>
#endif

#define BITMAP_PAGE_SIZE 4
#define MAX_KEY_VCODE 256
#define UPDATE_MAX_CALL 0xFF

namespace ege
{

class IMAGE;
class egeControlBase;
struct msg_createwindow;


extern bool _g_initcall;

extern int update_mark_count; //更新标记
/* egeControlBase */
extern egeControlBase* egectrl_root;
extern egeControlBase* egectrl_focus;


#if YEGE_Use_YSLib
using YSLib::Timers::FetchEpoch;
#else
template<class _tClock = std::chrono::high_resolution_clock>
inline typename _tClock::time_point
FetchEpoch()
{
	static auto start_time(_tClock::now());

	return start_time;
}

void
_set_initmode(int, int, int);
#endif

enum class get_input_op
{
	getch,
	kbhit,
	kbmsg
};

float
_get_FPS(int);

// EGE 全局状态
class EGEApplication
#if YEGE_Use_YSLib
	: public YSLib::GUIApplication
#endif
{
private:
#if YEGE_Use_YSLib
	std::thread ys_thrd;
	unique_ptr<YSLib::UI::Panel> ys_pnl;
	YSLib::Host::Window* ys_window;
#endif
	::HWND hwnd;
	::HDC window_dc;
	int dc_w = 640, dc_h = 480;
	bool use_force_exit; //强制关闭进程标记
	thread_queue<EGEMSG> msgkey_queue, msgmouse_queue;
	std::thread ui_thread;

public:
#if !YEGE_Use_YSLib
	CALLBACK_PROC* callback_close = {};
#endif

	// 鼠标状态
	int mouse_state_l = 0, mouse_state_m = 0, mouse_state_r = 0;
	int mouse_last_x = 0, mouse_last_y = 0;
	int mouse_lastclick_x = 0, mouse_lastclick_y = 0;
	int mouse_lastup_x = 0, mouse_lastup_y = 0;
	bool mouse_show = {};
	// 键盘状态
	int keystatemap[MAX_KEY_VCODE]{};

	EGEApplication(int, int*);
	EGEApplication(const EGEApplication&) = delete;
	~EGEApplication();

#if YEGE_Use_YSLib
	static DefGetter(ynothrow, ::HINSTANCE, Instance, ::GetModuleHandleW({}))
#else
	static ::HINSTANCE
	GetInstance()
	{
		return ::GetModuleHandleW({});
	}
#endif

	bool
	_is_run() const;

	void
	_flush_key_mouse(bool);

	int
	_get_dc_w() const
	{
		return dc_w;
	}

	int
	_get_dc_h() const
	{
		return dc_h;
	}

	::HWND
	_get_hwnd() const
	{
		return hwnd;
	}

	int
	_get_input(get_input_op);

	::HDC
	_get_window_dc() const
	{
		return window_dc;
	}

	int
	_getflush();

	key_msg
	_getkey();

	int
	_getkey_p();

	mouse_msg
	_getmouse();

	void
	_init_graph_x();

	int
	_keystate(int);

	bool
	_mousemsg();

#if !YEGE_Use_YSLib
	void
	_on_destroy();
#endif

	void
	_on_key(unsigned, unsigned long, ::LPARAM);

	void
	_on_mouse_button_up(::HWND, unsigned, ::WPARAM, ::LPARAM);

	void
	_on_paint(::HWND);

	::HCURSOR
	_on_setcursor(::HWND);

	int
	_peekkey();

	int
	_peekallkey(int);

	EGEMSG
	_peekmouse();

	void
	_process_queues();

	void
	_process_ui_msg(EGEMSG&);

	void
	_push_mouse_msg(unsigned, ::WPARAM, ::LPARAM);

	int
	_show_mouse(bool);

	void
	_uninit();

	void
	_update();

	void
	_update_if_necessary();

	bool
	_waitdealmessage();

	static void
	_window_handle_wm_user_1(::LPARAM, ::WPARAM);
};


class _pages
{
public:
	EGEApplication& gstate;

private:
	int active_page = 0;
	int visual_page = 0;
	IMAGE* imgtarget_set = {};
	mutable unique_ptr<IMAGE> img_page[BITMAP_PAGE_SIZE];

public:
	int base_x = 0, base_y = 0, base_w = 0, base_h = 0;
	IMAGE* imgtarget = {};

	_pages();

	void
	check_page(int) const;

	IMAGE&
	get_apage_ref() const;

	IMAGE&
	get_vpage_ref() const;

	IMAGE*
	get_target() const
	{
		return imgtarget_set;
	}

	void
	paint(::HDC);

	void
	set_apage(int);

	int
	set_target(IMAGE*);

	void
	set_vpage(int);

	void
	update();
};


EGEApplication&
FetchEGEApplication(int = VGA, int* = {});

_pages&
get_pages();

} // namespace ege;

#endif

