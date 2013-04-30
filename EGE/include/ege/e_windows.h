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
// �����Ϣ
EGE_DEPRECATE(MOUSEMSG)
struct MOUSEMSG
{
	UINT uMsg;              // ��ǰ�����Ϣ
	bool mkCtrl;            // Ctrl ���Ƿ���
	bool mkShift;           // Shift ���Ƿ���
	bool mkLButton;         // �������Ƿ���
	bool mkMButton;         // ����м��Ƿ���
	bool mkRButton;         // ����Ҽ��Ƿ���
	short x;                // ��ǰ��� x ����
	short y;                // ��ǰ��� y ����
	short wheel;            // �����ֹ���ֵ(120Ϊ����)
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

void EGEAPI setinitmode(int mode = INIT_DEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT); //���ó�ʼ��ģʽ��mode=0Ϊ��ͨ��1Ϊ�ޱ߿򴰿ڣ�xy�ǳ�ʼ��������

void EGEAPI setcaption(LPCSTR  caption);
void EGEAPI setcaption(LPCWSTR caption);

int  EGEAPI attachHWND(HWND hWnd);
HWND        EGEAPI getHWnd();         // ��ȡ��ͼ���ھ��
HINSTANCE   EGEAPI getHInstance();
PVOID       EGEAPI getProcfunc();

}
