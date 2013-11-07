// EGE 全局状态
#ifndef Inc_ege_global_h_
#define Inc_ege_global_h_

#include "msg.h"
#include "ege/proc.h"
#include "ege/input.h"
#include "ege/env.h"
#include "thread_queue.h"
#include <windows.h>

#define BITMAP_PAGE_SIZE 4
#define MAX_KEY_VCODE 256
#define UPDATE_MAX_CALL     0xFF

namespace ege
{

class IMAGE;
class egeControlBase;


extern int update_mark_count; //更新标记
extern bool timer_stop_mark;
extern bool skip_timer_mark;
/* egeControlBase */
extern egeControlBase* egectrl_root;
extern egeControlBase* egectrl_focus;


// 定义ege全局状态对象
struct _graph_setting
{
	struct _graph
	{
		int width;
		int height;
	} graph;
	struct _aspectratio
	{
		float xasp, yasp;
	} aspectratio;

	::HDC dc;
	::HDC window_dc;
	int dc_w, dc_h;
	IMAGE* img_page[BITMAP_PAGE_SIZE];
	int base_x, base_y, base_w, base_h;

	int     visual_page;
	int     active_page;
	IMAGE*  imgtarget;
	IMAGE*  imgtarget_set;
	IMAGE*  img_timer_update;

	::HINSTANCE instance;
	::HWND    hwnd;
	::TCHAR   window_class_name[32];
	::TCHAR   window_caption[128];
	int     exit_flag;
	int     exit_window;
	bool    close_manually;
	bool    use_force_exit; //强制关闭进程标记
	bool    lock_window;

	thread_queue<EGEMSG>* msgkey_queue, *msgmouse_queue;

	::HANDLE threadui_handle;


	/*鼠标状态记录*/
	int mouse_state_l, mouse_state_m, mouse_state_r;
	int mouse_last_x, mouse_last_y;
	int mouse_lastclick_x, mouse_lastclick_y;
	int mouse_lastup_x, mouse_lastup_y;
	int mouse_show;

	LPMSG_KEY_PROC callback_key;
	void* callback_key_param;
	LPMSG_MOUSE_PROC callback_mouse;
	void* callback_mouse_param;
	LPCALLBACK_PROC callback_close;

	/* 键盘状态记录 */
	int keystatemap[MAX_KEY_VCODE];

	/* 函数用临时缓冲区 */
	::DWORD g_t_buff[1024 * 8];
	::HWND _g_attach_hwnd = nullptr;
	int _g_windowpos_x = CW_USEDEFAULT, _g_windowpos_y = CW_USEDEFAULT;
	int _g_initoption = INIT_DEFAULT;
	bool _g_initcall = false;
	static ::DWORD g_windowstyle;
	static ::DWORD g_windowexstyle;

	bool
	_is_run() const;

	IMAGE*
	_get_target()
	{
		return imgtarget_set;
	}

	void
	_set_activepage(int);

	void
	_set_initmode(int, int, int);

	void
	_set_mode(int, int);

	int
	_set_target(IMAGE*);

	void
	_set_visualpage(int);

	int
	_dealmessage(bool);

	void
	_flushkey();

	void
	_flushmouse();

	int
	_getch_ex(int);

	int
	_getflush();

	key_msg
	_getkey();

	int
	_getkey_p();

	mouse_msg
	_getmouse();

#if 0
	EGEMSG
	_getmouse_p();
#endif

	void
	_init_graph();

	void
	_init_graph_p();

	void
	_init_graph_x(int*, int*);

	int
	_kbhit_ex(int);

	int
	_keystate(int);

	void
	_on_destroy();

	static void
	_on_ime_control(::HWND, ::WPARAM, ::LPARAM);

	void
	_on_key(::UINT, unsigned long, ::LPARAM);

	void
	_on_mouse_button_up(::HWND, ::UINT, ::WPARAM, ::LPARAM);

	void
	_on_paint(::HWND);

	void
	_on_repaint(::HWND, ::HDC);

	void
	_on_setcursor(::HWND);

	void
	_on_timer(::HWND, unsigned);

	int
	_peekkey();

	int
	_peekallkey(int);

	EGEMSG
	_peekmouse();

	void
	_process_ui_msg(EGEMSG&);

	void
	_push_mouse_msg(::UINT, ::WPARAM, ::LPARAM);

	int
	_redraw_window(::HDC);

	void
	_render_normal();

	void
	_render_manual();

	int
	_show_mouse(int);

	int
	_update();

	void
	_update_GUI();

	int
	_waitdealmessage();

	static void
	_windowmanager(bool, struct msg_createwindow*);
};


extern _graph_setting& graph_setting;

} // namespace ege;

#endif

