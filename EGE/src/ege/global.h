// EGE 全局状态
#ifndef Inc_ege_global_h_
#define Inc_ege_global_h_

#include "msg.h"
#include "ege/proc.h"
#include "ege/base.h"
#include "thread_queue.h"
#include <windows.h>

#define BITMAP_PAGE_SIZE 4
#define MAX_KEY_VCODE 256

namespace ege
{

class IMAGE;
class egeControlBase;

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

	int writemode;

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
	TCHAR   window_class_name[32];
	TCHAR   window_caption[128];
	int     exit_flag;
	int     exit_window;
	int     update_mark_count; //更新标记
	bool    close_manually;
	bool    use_force_exit; //强制关闭进程标记
	bool    lock_window;
	bool    init_finish;
	bool    timer_stop_mark;
	bool    skip_timer_mark;

	thread_queue<EGEMSG>* msgkey_queue, *msgmouse_queue;

	HANDLE threadui_handle;


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

	/* egeControlBase */
	egeControlBase* egectrl_root;
	egeControlBase* egectrl_focus;

	/* 私用全局变量 */
	::ULONG_PTR g_gdiplusToken;
	::LARGE_INTEGER get_highfeq_time_start;
	::DWORD         fclock_start;
	//double delay_dwLast;
	double delay_ms_dwLast;
	double delay_fps_dwLast;
	int getch_last_key;

	::HBRUSH savebrush_hbr;

	/* 函数用临时缓冲区 */
	::DWORD g_t_buff[1024 * 8];

	bool
	_is_run() const;

	//获取帧数
	float
	_get_FPS(int);

	double
	_get_highfeq_time_ls();

	void
	_set_activepage(int);

	void
	_set_visualpage(int);

	int
	_dealmessage(bool);

	void
	_delay_ms(long);

	void
	_delay_update();

	void
	_flushkey();

	int
	_getch_ex(int);

	key_msg
	_getkey();

	int
	_getkey_p();

	int
	_kbhit_ex(int);

	int
	_keystate(int);

	void
	_on_paint(::HWND);

	void
	_on_repaint(::HWND, ::HDC);

	int
	_peekkey();

	int
	_peekallkey(int);

	int
	_redraw_window(::HDC);

	int
	_update();

	int
	_waitdealmessage();
};

void guiupdate(_graph_setting* pg, egeControlBase*& root);

int getflush();


extern _graph_setting& graph_setting;

} // namespace ege

#endif

