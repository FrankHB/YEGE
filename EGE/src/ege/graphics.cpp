#ifdef _WIN64
#define ARCH "x64"
#else
#define ARCH "x86"
#endif

#define TOSTRING_(x) #x
#define TOSTRING(x) TOSTRING_(x)
#define GCC_VER TEXT(TOSTRING(__GNUC__)) TEXT(".") TEXT(TOSTRING(__GNUC_MINOR__)) TEXT(".") TEXT(TOSTRING(__GNUC_PATCHLEVEL__))

#define EGE_TITLE TEXT("yEGE13.04 ") TEXT("GCC") GCC_VER TEXT(ARCH)

#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "head.h"
#include "global.h"


namespace ege
{

static ::DWORD g_windowstyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
	| WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_VISIBLE;
static ::DWORD g_windowexstyle = WS_EX_LEFT | WS_EX_LTRREADING;

WINAPI ::DWORD messageloopthread(LPVOID lpParameter);


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

BOOL
init_instance(::HINSTANCE hInstance, int nCmdShow)
{
	auto pg = &graph_setting;
	int dw = 0, dh = 0;
	//wchar_t Title[256]{0};
	//wchar_t Title2[256]{0};

	//WideCharToMultiByte(CP_UTF8, 0, pg->window_caption,
	//	::lstrlenW(pg->window_caption), (char*)Title, 256, 0, 0);
	//MultiByteToWideChar(CP_UTF8, 0, (char*)Title, -1, Title2, 256);
	dw = GetSystemMetrics(SM_CXFRAME) * 2;
	dh = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) * 2;
	if(pg->_g_attach_hwnd)
	{
		::LONG_PTR style = ::GetWindowLongPtrW(pg->_g_attach_hwnd, GWL_STYLE);
		style |= WS_CHILDWINDOW | WS_CLIPCHILDREN;
		::SetWindowLongPtrW(pg->_g_attach_hwnd, GWL_STYLE, style);
	}

	pg->hwnd = ::CreateWindowEx(g_windowexstyle, pg->window_class_name,
		pg->window_caption, g_windowstyle & ~WS_VISIBLE, pg->_g_windowpos_x,
		pg->_g_windowpos_y, pg->dc_w + dw, pg->dc_h + dh, pg->_g_attach_hwnd, nullptr,
		hInstance, nullptr);
	if(!pg->hwnd)
		return FALSE;
	if(pg->_g_attach_hwnd)
	{
		//SetParent(pg->hwnd, pg->_g_attach_hwnd);
		wchar_t name[64];

		std::swprintf(name, L"ege_%X",
			::DWORD(::DWORD_PTR(pg->_g_attach_hwnd)));
		if(::CreateEventW(nullptr, FALSE, TRUE, name))
			if(::GetLastError() == ERROR_ALREADY_EXISTS)
				::PostMessage(pg->hwnd, WM_CLOSE, 0, 0);
	}
	//::SetWindowTextA(pg->hwnd, (const char*)Title);
	::SetWindowLongPtrW(pg->hwnd, GWLP_USERDATA, (::LONG_PTR)pg);

	/*{
		::LOGFONTW lf{0};
		lf.lfHeight         = 12;
		lf.lfWidth          = 6;
		lf.lfEscapement     = 0;
		lf.lfOrientation    = 0;
		lf.lfWeight         = FW_DONTCARE;
		lf.lfItalic         = 0;
		lf.lfUnderline      = 0;
		lf.lfStrikeOut      = 0;
		lf.lfCharSet        = DEFAULT_CHARSET;
		lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
		lf.lfQuality        = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH;
		::lstrcpyW(lf.lfFaceName, L"宋体");
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::SendMessage(pg->hwnd, WM_SETFONT, (::WPARAM)hfont, nullptr);
		//::DeleteObject(hfont);
	} //*/

	pg->exit_window = 0;
	::ShowWindow(pg->hwnd, nCmdShow);
	if(g_windowexstyle & WS_EX_TOPMOST)
		::SetWindowPos(pg->hwnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE);
	return TRUE;
}

#if !defined(UNICODE)
CALLBACK BOOL
EnumResNameProc(::HMODULE hModule, const char*, char* lpszName, ::LONG_PTR lParam
)
{
	auto hico = (::HICON)::LoadImage(hModule, lpszName, IMAGE_ICON, 0, 0,
		LR_DEFAULTSIZE);

	if(hico)
	{
		*((::HICON*)lParam) = hico;
		return FALSE;
	}
	return TRUE;
}
#else
CALLBACK BOOL
EnumResNameProc(
	::HMODULE hModule,
	const wchar_t* lpszType,
	wchar_t* lpszName,
	::LONG_PTR lParam
)
{
	::HICON hico = (::HICON)::LoadImageW(hModule, lpszName, IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE);
	if(hico)
	{
		*((::HICON*)lParam) = hico;
		return FALSE;
	}
	return TRUE;
}
#endif

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

namespace
{

ATOM
register_class(_graph_setting* pg, ::HINSTANCE hInstance)
{
	static ::WNDCLASSEX wcex;
	::HICON hico = nullptr;

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = getProcfunc();
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(nullptr, IDI_WINLOGO);
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (::HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = pg->window_class_name;

	EnumResourceNames(hInstance, RT_ANIICON, EnumResNameProc, (::LONG_PTR)&hico);
	if(hico)
	{
		wcex.hIcon = hico;
		goto END_LOAD_ICON;
	}
	EnumResourceNames(hInstance, RT_GROUP_ICON, EnumResNameProc, (::LONG_PTR)&hico);
	if(hico)
	{
		wcex.hIcon = hico;
		goto END_LOAD_ICON;
	}
	EnumResourceNames(hInstance, RT_ICON, EnumResNameProc, (::LONG_PTR)&hico);
	if(hico)
	{
		wcex.hIcon = hico;
		goto END_LOAD_ICON;
	}

END_LOAD_ICON:
	return ::RegisterClassEx(&wcex);
}

}

void
initgraph(int* gdriver, int* gmode, char*)
{
	auto pg = &graph_setting;

	pg->_g_initcall = 0;
	if(pg->_g_has_init)
	{
		pg->exit_flag = 0;
		pg->exit_window = 0;
		pg->_set_mode(*gdriver, *gmode);
		pg->_init_img_page();
		return;
	}
	else
	{
		memset(pg, 0, sizeof(_graph_setting));
		pg->exit_flag = 0;
		pg->exit_window = 0;
		pg->_init_img_page_f();
	}
	pg->_set_mode(*gdriver, *gmode);
	pg->instance = GetModuleHandle(nullptr);
	::lstrcpy(pg->window_class_name, TEXT("Easy Graphics Engine"));
	::lstrcpy(pg->window_caption, EGE_TITLE);
	{
	//	::SECURITY_ATTRIBUTES sa{0};
		::DWORD pid;

		pg->init_finish = false;
		pg->threadui_handle = ::CreateThread(nullptr, 0, messageloopthread,
			pg, CREATE_SUSPENDED, &pid);
		ResumeThread(pg->threadui_handle);
		while(!pg->init_finish)
			::Sleep(1);
	}

	::UpdateWindow(pg->hwnd);
	{
		//初始化鼠标位置数据
		pg->mouse_last_x = pg->dc_w / 2;
		pg->mouse_last_y = pg->dc_h / 2;
	}

	static egeControlBase _egeControlBase;

	if(pg->_g_initoption & INIT_RENDERMANUAL)
		setrendermode(RENDER_MANUAL);
	pg->mouse_show = 1;
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


//private
::DWORD WINAPI
messageloopthread(LPVOID lpParameter)
{
	_graph_setting* pg = (_graph_setting*)lpParameter;
	MSG msg;
	{
		int nCmdShow = SW_SHOW;
		register_class(pg, pg->instance);

		/* 执行应用程序初始化: */
		if(!init_instance(pg->instance, nCmdShow))
			return 0xFFFFFFFF;

		//图形初始化
		pg->_init_graph();
		pg->mouse_show = 0;
		pg->exit_flag = 0;
		pg->use_force_exit = (pg->_g_initoption & INIT_NOFORCEEXIT ? false : true);
		if(pg->_g_initoption & INIT_NOFORCEEXIT)
			SetCloseHandler([]{graph_setting.exit_flag = 1;});
		pg->close_manually = true;
		pg->skip_timer_mark = false;
		::SetTimer(pg->hwnd, RENDER_TIMER_ID, 50, nullptr);
	}
	pg->init_finish = true;

	while(!pg->exit_window)
		if(GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			::Sleep(1);
	return 0;
}

void
setinitmode(int mode, int x, int y)
{
	graph_setting._g_initcall = 1;
	graph_setting._g_initoption = mode;
	if(mode & INIT_NOBORDER)
	{
		g_windowstyle = mode & INIT_CHILD ? WS_CHILDWINDOW | WS_CLIPCHILDREN
			| WS_VISIBLE : WS_POPUP | WS_CLIPCHILDREN | WS_VISIBLE;
		g_windowexstyle = WS_EX_LEFT | WS_EX_LTRREADING;
	}
	else
	{
		g_windowstyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
			| WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_VISIBLE;
		g_windowexstyle = WS_EX_LEFT | WS_EX_LTRREADING;
	}
	if(mode & INIT_TOPMOST)
		g_windowexstyle |= WS_EX_TOPMOST;
	graph_setting._g_windowpos_x = x;
	graph_setting._g_windowpos_y = y;
}

} // namespace ege

