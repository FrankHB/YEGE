#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "head.h"
#include "global.h"
#include "image.h"


namespace ege
{

namespace
{

int
redraw_window(_graph_setting* pg, ::HDC dc)
{
	int page = pg->visual_page;
	::HDC hDC = pg->img_page[page]->m_hDC;
	int left = pg->img_page[page]->m_vpt.left,
		top = pg->img_page[page]->m_vpt.top;
	//::HRGN rgn = pg->img_page[page]->m_rgn;
	::BitBlt(dc, 0, 0, pg->base_w, pg->base_h, hDC, pg->base_x - left,
		pg->base_y - top, SRCCOPY);
	pg->update_mark_count = UPDATE_MAX_CALL;
	return 0;
}

}

int
kbmsg()
{
	return graph_setting.exit_window ? int(grNoInitGraph)
		: int(graph_setting._peekallkey(1));
}

int
kbhit()
{
	return graph_setting._kbhit_ex(0);
}

int
getch()
{
	return graph_setting._getch_ex(0);
}

key_msg
getkey()
{
	return graph_setting._getkey();
}

void
flushkey()
{
	graph_setting._flushkey();
}

int
keystate(int key)
{
	return graph_setting._keystate(key);
}

void
flushmouse()
{
	graph_setting._flushmouse();
}

int
mousemsg()
{
	if(graph_setting.exit_window)
		return 0;

	EGEMSG msg(graph_setting._peekmouse());

	return bool(msg.hwnd);
}

mouse_msg
getmouse()
{
	return graph_setting._getmouse();
}


namespace
{

LRESULT CALLBACK
wndproc(::HWND hWnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
{
	auto pg = &graph_setting;
	//int wmId, wmEvent;
	auto pg_w = (_graph_setting*)::GetWindowLongPtrW(hWnd, GWLP_USERDATA);

	if(!pg_w)
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	switch(message)
	{
	case WM_TIMER:
		if(pg == pg_w)
			pg->_on_timer(hWnd, (unsigned int)wParam);
		break;
	case WM_PAINT:
		if(pg == pg_w)
			pg->_on_paint(hWnd);
		break;
	case WM_CLOSE:
		if(pg == pg_w)
		{
			if(pg->callback_close)
				pg->callback_close();
			else
				return ::DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		if(pg == pg_w)
			pg->_on_destroy();
		break;
	case WM_ERASEBKGND:
		if(pg == pg_w)
			return TRUE;
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	//	if(hWnd == pg->hwnd)
		pg->_on_key(message, (unsigned long)wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		pg->mouse_lastclick_x = (short int)((::UINT)lParam & 0xFFFF);
		pg->mouse_lastclick_y = (short int)((::UINT)lParam >> 16);
		pg->keystatemap[VK_LBUTTON] = 1;
		SetCapture(hWnd);
		pg->mouse_state_l = 1;
		if(hWnd == pg->hwnd)
			pg->_push_mouse_msg( message, wParam, lParam);
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		pg->mouse_lastclick_x = (short int)((::UINT)lParam & 0xFFFF);
		pg->mouse_lastclick_y = (short int)((::UINT)lParam >> 16);
		pg->keystatemap[VK_MBUTTON] = 1;
		::SetCapture(hWnd);
		pg->mouse_state_m = 1;
		if(hWnd == pg->hwnd)
			pg->_push_mouse_msg( message, wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		pg->mouse_lastclick_x = (short int)((::UINT)lParam & 0xFFFF);
		pg->mouse_lastclick_y = (short int)((::UINT)lParam >> 16);
		pg->keystatemap[VK_RBUTTON] = 1;
		SetCapture(hWnd);
		pg->mouse_state_r = 1;
		if(hWnd == pg->hwnd) pg->_push_mouse_msg( message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		pg->_on_mouse_button_up(hWnd, message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		pg->mouse_last_x = (short int)((::UINT)lParam & 0xFFFF);
		pg->mouse_last_y = (short int)((::UINT)lParam >> 16);
		if(hWnd == pg->hwnd && (pg->mouse_lastup_x != pg->mouse_last_x
			|| pg->mouse_lastup_y != pg->mouse_last_y))
			pg->_push_mouse_msg( message, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		{
			POINT pt{(short int)((::UINT)lParam & 0xFFFF),
				(short int)((::UINT)lParam >> 16)};

			::ScreenToClient(pg->hwnd, &pt);
			pg->mouse_last_x = pt.x;
			pg->mouse_last_y = pt.y;
			lParam = ((unsigned short)(short int)pg->mouse_last_y << 16)
				| (unsigned short)(short int)pg->mouse_last_x;
		}
		if(hWnd == pg->hwnd)
			pg->_push_mouse_msg( message, wParam, lParam);
		break;
	case WM_SETCURSOR:
		if(pg == pg_w)
		{
			pg->_on_setcursor(hWnd);
			return TRUE;
		}
		break;
	case WM_IME_CONTROL:
		_graph_setting::_on_ime_control(hWnd, wParam, lParam);
		break;
	case WM_USER + 1:
		_graph_setting::_windowmanager(wParam != 0,
			(struct msg_createwindow*)lParam);
		break;
	case WM_USER + 2:
		::SetFocus((::HWND)lParam);
		break;
	case WM_CTLCOLOREDIT:
		return ((egeControlBase*)::GetWindowLongPtrW((::HWND)lParam,
			GWLP_USERDATA))->onMessage(message, wParam, lParam);
		break;
	default:
		if(pg != pg_w)
			return ((egeControlBase*)pg_w)->onMessage(message, wParam, lParam);
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	if(pg != pg_w)
		return ((egeControlBase*)pg_w)->onMessage(message, wParam, lParam);
	return 0;
}

}

::WNDPROC
getProcfunc()
{
	return wndproc;
}


void
initgraph(int* gdriver, int* gmode, char*)
{
	graph_setting._init_graph_x(gdriver, gmode);
}

void
initgraph(int Width, int Height, int Flag)
{
	int g = TRUECOLORSIZE, m = (Width) | (Height << 16);

	if(graph_setting._g_initcall == 0)
		setinitmode(Flag);
	initgraph(&g, &m, (char*)"");
	//using flag;
}

void
detectgraph(int* gdriver, int* gmode)
{
	*gdriver = VGA;
	*gmode = VGAHI;
}

void
closegraph()
{
	::ShowWindow(graph_setting.hwnd, SW_HIDE);
}

int
attachHWND(::HWND hWnd)
{
	graph_setting._g_attach_hwnd = hWnd;
	return 0;
}

} // namespace ege

