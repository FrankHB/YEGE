#ifndef Inc_ege_windows_h_
#define Inc_ege_windows_h_

#include "ege/def.h"
#include <windows.h>

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

namespace ege
{

enum line_styles        /* Line styles for get/setlinestyle */
{
	SOLID_LINE      = PS_SOLID,
	CENTER_LINE     = PS_DASH,
	DOTTED_LINE     = PS_DOT,
	DASHED_LINE     = PS_DASHDOT,
	NULL_LINE       = PS_NULL,
	USERBIT_LINE    = PS_USERSTYLE,   /* User defined line style */
};

struct msg_createwindow
{
	HANDLE hEvent;
	::HWND hwnd;
	const wchar_t* classname;
	::DWORD style;
	::DWORD exstyle;
	unsigned id;
	LPVOID param;
};

EGEAPI void setinitmode(int mode = INIT_DEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT); //设置初始化模式，mode=0为普通，1为无边框窗口，xy是初始窗口坐标

EGEAPI void setcaption(const char*  caption);
EGEAPI void setcaption(const wchar_t* caption);

int  EGEAPI attachHWND(::HWND hWnd);
::HWND        EGEAPI getHWnd();         // 获取绘图窗口句柄
::HINSTANCE   EGEAPI getHInstance();
EGEAPI ::WNDPROC getProcfunc();

}

#endif

