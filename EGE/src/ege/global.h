// EGE 全局状态
#ifndef Inc_ege_global_h_
#define Inc_ege_global_h_

#include "msg.h"
#include "ege/proc.h"
#include "ege/input.h"
#include "ege/env.h"
#include "ege/base.h"
#include <thread>
#include "thread_queue.h"
#include <windows.h>

#define BITMAP_PAGE_SIZE 4
#define MAX_KEY_VCODE 256
#define UPDATE_MAX_CALL     0xFF

namespace ege
{

class IMAGE;
class egeControlBase;
struct msg_createwindow;


extern int _g_initoption;
extern bool _g_initcall;

extern int update_mark_count; //更新标记
/* egeControlBase */
extern egeControlBase* egectrl_root;
extern egeControlBase* egectrl_focus;


void
_set_initmode(int, int, int);


enum class get_input_op
{
	getch,
	kbhit,
	kbmsg
};


// 定义ege全局状态对象
struct _graph_setting
{
private:
	::HWND hwnd;
	::HDC window_dc;
	int dc_w = 640, dc_h = 480;
	bool use_force_exit; //强制关闭进程标记
	thread_queue<EGEMSG> msgkey_queue, msgmouse_queue;
	std::thread ui_thread;

public:
	/*鼠标状态记录*/
	int mouse_state_l = 0, mouse_state_m = 0, mouse_state_r = 0;
	int mouse_last_x = 0, mouse_last_y = 0;
	int mouse_lastclick_x = 0, mouse_lastclick_y = 0;
	int mouse_lastup_x = 0, mouse_lastup_y = 0;
	bool mouse_show = {};

	CALLBACK_PROC* callback_close = {};

	/* 键盘状态记录 */
	int keystatemap[MAX_KEY_VCODE]{};

	_graph_setting(int, int*);
	_graph_setting(const _graph_setting&) = delete;
	~_graph_setting();

	bool
	_is_run() const;

	bool
	_is_window_exit() const;

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

	void
	_on_destroy();

	static void
	_on_ime_control(::HWND, ::WPARAM, ::LPARAM);

	void
	_on_key(unsigned, unsigned long, ::LPARAM);

	void
	_on_mouse_button_up(::HWND, unsigned, ::WPARAM, ::LPARAM);

	void
	_on_paint(::HWND);

	void
	_on_setcursor(::HWND);

	int
	_peekkey();

	int
	_peekallkey(int);

	EGEMSG
	_peekmouse();

	void
	_process_ui_msg(EGEMSG&);

	void
	_push_mouse_msg(unsigned, ::WPARAM, ::LPARAM);

	int
	_show_mouse(bool);

	void
	_update();

	void
	_update_GUI();

	void
	_update_if_necessary();

	bool
	_waitdealmessage();

	static void
	_window_create(msg_createwindow&);

	static void
	_window_destroy(msg_createwindow&);

	static ::HINSTANCE
	get_instance()
	{
		return ::GetModuleHandleW({});
	}
};


struct _pages
{
	_graph_setting& gstate;
	::HDC active_dc;
	int active_page = 0;
	int visual_page = 0;
	IMAGE* imgtarget_set = {};
	mutable IMAGE* img_page[BITMAP_PAGE_SIZE] = {};
	IMAGE* imgtarget = {};
	int base_x = 0, base_y = 0, base_w = 0, base_h = 0;

	_pages();
	~_pages();

	void
	check_page(int) const;

	IMAGE&
	get_apage_ref() const;

	::HDC
	get_image_context() const;

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


_graph_setting&
get_global_state(int = VGA, int* = {});

_pages&
get_pages();

} // namespace ege;

#endif

