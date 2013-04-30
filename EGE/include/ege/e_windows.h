#pragma once

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

#if 0
// 鼠标消息
EGE_DEPRECATE(MOUSEMSG)
struct MOUSEMSG
{
	UINT uMsg;              // 当前鼠标消息
	bool mkCtrl;            // Ctrl 键是否按下
	bool mkShift;           // Shift 键是否按下
	bool mkLButton;         // 鼠标左键是否按下
	bool mkMButton;         // 鼠标中键是否按下
	bool mkRButton;         // 鼠标右键是否按下
	short x;                // 当前鼠标 x 坐标
	short y;                // 当前鼠标 y 坐标
	short wheel;            // 鼠标滚轮滚动值(120为基数)
};
#endif

struct msg_createwindow
{
	HANDLE hEvent;
	HWND hwnd;
	LPCWSTR classname;
	DWORD style;
	DWORD exstyle;
	unsigned id;
	LPVOID param;
};

void EGEAPI setinitmode(int mode = INIT_DEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT); //设置初始化模式，mode=0为普通，1为无边框窗口，xy是初始窗口坐标

void EGEAPI setcaption(LPCSTR  caption);
void EGEAPI setcaption(LPCWSTR caption);

int  EGEAPI attachHWND(HWND hWnd);
HWND        EGEAPI getHWnd();         // 获取绘图窗口句柄
HINSTANCE   EGEAPI getHInstance();
PVOID       EGEAPI getProcfunc();

}
