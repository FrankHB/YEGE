#include "global.h"
#include "image.h" // for IMAGE;
#include "ege/time.h" // for delay_ms;
#include "ege/ctl.h"
#include "head.h"
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>
#include <windows.h>
#include <memory> // for std::unique_ptr;
#include <functional> // for std::bind;
#include <cassert>

#ifdef _WIN64
#define ARCH "x64"
#else
#define ARCH "x86"
#endif

#define TOSTRING_(x) #x
#define TOSTRING(x) TOSTRING_(x)
#define GCC_VER TEXT(TOSTRING(__GNUC__)) TEXT(".") TEXT(TOSTRING(__GNUC_MINOR__)) TEXT(".") TEXT(TOSTRING(__GNUC_PATCHLEVEL__))

#define EGE_TITLE TEXT("yEGE13.04 ") TEXT("GCC") GCC_VER TEXT(ARCH)

#define UPDATE_MAX_CALL     0xFF


namespace ege
{

bool
_graph_setting::_is_run() const
{
	return !(exit_window || exit_flag);
}

float
_graph_setting::_get_FPS(int add)
{
	static int fps = 0;
	static int fps_inv = 0;
	static double time = 0;
	static float flret = 0;
	static float fret = 0;
	static float fret_inv = 0;

	double cur = _get_highfeq_time_ls();
	if(add == 0x100)
		++fps;
	else if(add == -0x100)
		++fps, ++fps_inv;
	if(cur - time >= 0.5)
	{
		flret = fret;
		fret = (float)(fps / (cur - time));
		fret_inv = (float)((fps - fps_inv) / (cur - time));
		fps = 0;
		fps_inv = 0;
		time = cur;
	}
	return add > 0 ? (fret + flret) / 2 : fret_inv;
}

double
_graph_setting::_get_highfeq_time_ls()
{
	static ::LARGE_INTEGER llFeq; /* 此实为常数 */
	::LARGE_INTEGER llNow;

	if(get_highfeq_time_start.QuadPart == 0)
	{
		::SetThreadAffinityMask(::GetCurrentThread(), 0);
		::QueryPerformanceCounter(&get_highfeq_time_start);
		::QueryPerformanceFrequency(&llFeq);
		return 0;
	}
	else
	{
		::QueryPerformanceCounter(&llNow);
		llNow.QuadPart -= get_highfeq_time_start.QuadPart;
		return (double)llNow.QuadPart / llFeq.QuadPart;
	}
}

void
_graph_setting::_set_activepage(int page)
{
	active_page = page;
	if(!img_page[page])
	{
		img_page[page] = new IMAGE;
		img_page[page]->createimage(dc_w, dc_h);
		dc = img_page[page]->m_hDC;
	}
}

void
_graph_setting::_set_mode(int gdriver, int gmode)
{
	if(gdriver == TRUECOLORSIZE)
	{
		::RECT rect;

		if(_g_attach_hwnd)
			::GetClientRect(_g_attach_hwnd, &rect);
		else
			::GetWindowRect(GetDesktopWindow(), &rect);
		dc_w = short(gmode & 0xFFFF);
		dc_h = short((unsigned int)gmode >> 16);
		if(dc_w < 0)
			dc_w = rect.right;
		if(dc_h < 0)
			dc_h = rect.bottom;
	}
	else
	{
		dc_w = 640;
		dc_h = 480;
	}
}

void
_graph_setting::_set_visualpage(int page)
{
	visual_page = page;
	if(!img_page[page])
	{
		img_page[page] = new IMAGE;
		img_page[page]->createimage(dc_w, dc_h);
	}
	update_mark_count = 0;
}

int
_graph_setting::_dealmessage(bool force_update)
{
	if(force_update || update_mark_count <= 0)
		_update();
	return !exit_window;
}

void
_graph_setting::_delay_ms(long ms)
{
	skip_timer_mark = true;
	if(ms == 0)
		_delay_update();
	else
	{
		double delay_time(ms);
		double dw(_get_highfeq_time_ls() * 1000.0);
		int f(ms >= 50 ? 0 : 100);

		delay_ms_dwLast = 0;
		if(delay_ms_dwLast == 0)
			delay_ms_dwLast = _get_highfeq_time_ls() * 1000.0;
		if(delay_ms_dwLast + 200.0 > dw)
			dw = delay_ms_dwLast;

		//ege_sleep(1);
		egectrl_root->draw(nullptr);
		while(dw + delay_time >= _get_highfeq_time_ls() * 1000.0)
		{
			if(f <= 0 || update_mark_count < UPDATE_MAX_CALL)
			{
				_dealmessage(FORCE_UPDATE);
				f = 256;
			}
			else
				ege_sleep((int)(dw + delay_time - _get_highfeq_time_ls()
					* 1000.0));
			--f;
		}
		_dealmessage(FORCE_UPDATE);
		dw = _get_highfeq_time_ls() * 1000.0;
		_update_GUI();
		egectrl_root->update();
		if(delay_ms_dwLast + 200.0 <= dw || delay_ms_dwLast > dw)
			delay_ms_dwLast = dw;
		else
			delay_ms_dwLast += delay_time;
	}
	skip_timer_mark = false;
}

void
_graph_setting::_delay_fps(double fps)
{
	skip_timer_mark = true;

	double delay_time = 1000.0 / fps;
	double avg_max_time = delay_time * 10.0; // 误差时间在这个数值以内做平衡
	double dw = _get_highfeq_time_ls() * 1000.0;
	int nloop = 0;

	if(delay_fps_dwLast == 0)
		delay_fps_dwLast = _get_highfeq_time_ls() * 1000.0;
	if(delay_fps_dwLast + delay_time + avg_max_time > dw)
		dw = delay_fps_dwLast;
	egectrl_root->draw(nullptr);
	for(; nloop >= 0; --nloop)
	{
		if((dw + delay_time + (100.0) >= _get_highfeq_time_ls() * 1000.0))
		{
			do
			{
				ege_sleep((int)(dw + delay_time - _get_highfeq_time_ls()
					* 1000.0));
			} while(dw + delay_time >= _get_highfeq_time_ls() * 1000.0);
		}
		_dealmessage(FORCE_UPDATE);
		dw = _get_highfeq_time_ls() * 1000.0;
		_update_GUI();
		egectrl_root->update();
		if(delay_fps_dwLast + delay_time + avg_max_time <= dw
			|| delay_fps_dwLast > dw)
			delay_fps_dwLast = dw;
		else
			delay_fps_dwLast += delay_time;
	}
	skip_timer_mark = false;
}

void
_graph_setting::_delay_jfps(double fps)
{
	skip_timer_mark = true;
	double delay_time = 1000.0 / fps;
	double avg_max_time = delay_time * 10.0;
	double dw = _get_highfeq_time_ls() * 1000.0;
	int nloop = 0;

	if(delay_fps_dwLast == 0)
		delay_fps_dwLast = _get_highfeq_time_ls() * 1000.0;
	if(delay_fps_dwLast + delay_time + avg_max_time > dw)
		dw = delay_fps_dwLast;
	egectrl_root->draw(nullptr);
	for(; nloop >= 0; --nloop)
	{
		int bSleep = 0;

		while(dw + delay_time >= _get_highfeq_time_ls() * 1000.0)
		{
			ege_sleep((int)(dw + delay_time - _get_highfeq_time_ls()
				* 1000.0));
			bSleep = 1;
		}
		if(bSleep)
			_dealmessage(FORCE_UPDATE);
		else
			graph_setting._get_FPS(-0x100);
		dw = _get_highfeq_time_ls() * 1000.0;
		_update_GUI();
		egectrl_root->update();
		if(delay_fps_dwLast + delay_time + avg_max_time <= dw
			|| delay_fps_dwLast > dw)
			delay_fps_dwLast = dw;
		else
			delay_fps_dwLast += delay_time;
	}
	skip_timer_mark = false;
}

void
_graph_setting::_delay_update()
{
	if(update_mark_count < UPDATE_MAX_CALL)
	{
		ege_sleep(1);
		egectrl_root->draw(nullptr);
		_dealmessage(FORCE_UPDATE);
		egectrl_root->update();

		int l, t, r, b, c;

		getviewport(&l, &t, &r, &b, &c);
		setviewport(l, t, r, b, c);
	}
	delay_ms_dwLast = _get_highfeq_time_ls() * 1000.0;
	skip_timer_mark = false;
}

void
_graph_setting::_flushkey()
{
	EGEMSG msg;

	if(msgkey_queue->empty())
		_dealmessage(NORMAL_UPDATE);
	if(!msgkey_queue->empty())
		while(msgkey_queue->pop(msg))
			;
}

void
_graph_setting::_flushmouse()
{
	EGEMSG msg;

	if(msgmouse_queue->empty())
		_dealmessage(NORMAL_UPDATE);
	if(!msgmouse_queue->empty())
		while(msgmouse_queue->pop(msg))
			;
}

int
_graph_setting::_getch_ex(int flag)
{
	if(exit_window)
		return grNoInitGraph;
	{
		int key;
		EGEMSG msg;
		::DWORD dw = GetTickCount();
		do
		{
			key = _kbhit_ex(flag);
			if(key < 0)
				break;
			if(key > 0)
			{
				key = _getkey_p();
				if(key)
				{
					msg = msgkey_queue->last();
					if(dw < msg.time + 1000)
					{
						int ogn_key = key;

						key &= 0xFFFF;

						int ret = key;

						if(flag)
							ret = ogn_key;
						else if(((ogn_key & KEYMSG_DOWN) && (msg.wParam >= 0x70
							&& msg.wParam < 0x80)) || (msg.wParam > ' '
							&& msg.wParam < '0'))
							ret |= 0x100;
						return ret;
					}
				}
			}
		} while(!exit_window && !exit_flag && _waitdealmessage());
	}
	return 0;
}

int
_graph_setting::_getflush()
{
	EGEMSG msg;
	int lastkey = 0;

	if(msgkey_queue->empty())
		_dealmessage(NORMAL_UPDATE);
	if(!msgkey_queue->empty())
		while(msgkey_queue->pop(msg))
			if(msg.message == WM_CHAR)
				if(msg.message == WM_CHAR)
					lastkey = int(msg.wParam);
	return lastkey;
}

key_msg
_graph_setting::_getkey()
{
	key_msg ret{0, key_msg_none, 0};

	if(!exit_window)
	{
		int key = 0;

		do
		{
			if((key = _getkey_p()))
			{
				key_msg msg{0, key_msg_none, 0};

				if(key & KEYMSG_DOWN)
					msg.msg = key_msg_down;
				else if(key & KEYMSG_UP)
					msg.msg = key_msg_up;
				else if(key & KEYMSG_CHAR)
					msg.msg = key_msg_char;
				msg.key = key & 0xFFFF;
				if(keystate(VK_CONTROL))
					msg.flags |= key_flag_ctrl;
				if(keystate(VK_SHIFT))
					msg.flags |= key_flag_shift;
				return msg;
			}
		} while(!exit_window && !exit_flag && _waitdealmessage());
	}
	return ret;
}

int
_graph_setting::_getkey_p()
{
	EGEMSG msg;

	while(msgkey_queue->pop(msg))
		switch(msg.message)
		{
		case WM_CHAR:
			return KEYMSG_CHAR | (int(msg.wParam) & 0xFFFF);
		case WM_KEYDOWN:
			return KEYMSG_DOWN | (int(msg.wParam) & 0xFFFF)
				| (msg.lParam & 0x40000000 ? 0 : KEYMSG_FIRSTDOWN);
		case WM_KEYUP:
			return KEYMSG_UP | (int(msg.wParam) & 0xFFFF);
		default:
			break;
		}
	return 0;
}

mouse_msg
_graph_setting::_getmouse()
{
	auto mmsg = mouse_msg();

	if(exit_window)
		return mmsg;

	EGEMSG msg;

	do
	{
		msgmouse_queue->pop(msg);
		if(msg.hwnd)
		{
			mmsg.flags |= ((msg.wParam & MK_CONTROL) != 0
				? mouse_flag_ctrl : 0);
			mmsg.flags |= ((msg.wParam & MK_SHIFT) != 0 ? mouse_flag_shift : 0);
			mmsg.x = short(int(msg.lParam) & 0xFFFF);
			mmsg.y = short(unsigned(msg.lParam) >> 16);
			mmsg.msg = mouse_msg_move;
			if(msg.message == WM_LBUTTONDOWN)
			{
				mmsg.msg = mouse_msg_down;
				mmsg.flags |= mouse_flag_left;
			}
			else if(msg.message == WM_RBUTTONDOWN)
			{
				mmsg.msg = mouse_msg_down;
				mmsg.flags |= mouse_flag_right;
			}
			else if(msg.message == WM_MBUTTONDOWN)
			{
				mmsg.msg = mouse_msg_down;
				mmsg.flags |= mouse_flag_mid;
			}
			else if(msg.message == WM_LBUTTONUP)
			{
				mmsg.msg = mouse_msg_up;
				mmsg.flags |= mouse_flag_left;
			}
			else if(msg.message == WM_RBUTTONUP)
			{
				mmsg.msg = mouse_msg_up;
				mmsg.flags |= mouse_flag_right;
			}
			else if(msg.message == WM_MBUTTONUP)
			{
				mmsg.msg = mouse_msg_up;
				mmsg.flags |= mouse_flag_mid;
			}
			else if(msg.message == WM_MOUSEWHEEL)
			{
				mmsg.msg = mouse_msg_wheel;
				mmsg.wheel = short(unsigned(msg.wParam) >> 16);
			}
			return mmsg;
		}
	} while(!exit_window && !exit_flag && _waitdealmessage());
	return mmsg;
}

#if 0
EGEMSG
_graph_setting::_getmouse_p()
{
	auto msg = EGEMSG();

	msgmouse_queue->pop(msg);
	return msg;
}
#endif

void
_graph_setting::_init_graph()
{
	auto hDC(::GetDC(hwnd));

	dc = hDC;
	window_dc = hDC;
	img_timer_update = newimage();
	msgkey_queue = new thread_queue<EGEMSG>;
	msgmouse_queue = new thread_queue<EGEMSG>;
	setactivepage(0);
	settarget(nullptr);
	setvisualpage(0);
	window_setviewport(0, 0, dc_w, dc_h);
	//::ReleaseDC(hwnd, hDC);
}


namespace
{

::DWORD g_windowstyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
	| WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_VISIBLE;
::DWORD g_windowexstyle = WS_EX_LEFT | WS_EX_LTRREADING;


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
EnumResNameProc(::HMODULE hModule, const char*, char* lpszName,
	::LONG_PTR lParam)
{
	if(const auto hico = (::HICON)::LoadImage(hModule, lpszName, IMAGE_ICON, 0,
		0, LR_DEFAULTSIZE))
	{
		*((::HICON*)lParam) = hico;
		return FALSE;
	}
	return TRUE;
}
#else
CALLBACK BOOL
EnumResNameProc(::HMODULE hModule, const wchar_t* lpszType, wchar_t* lpszName,
	::LONG_PTR lParam)
{
	if(const auto hico = (::HICON)::LoadImageW(hModule, lpszName, IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE))
	{
		*((::HICON*)lParam) = hico;
		return FALSE;
	}
	return TRUE;
}
#endif

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

void
_graph_setting::_init_graph_x(int* gdriver, int* gmode)
{
	_g_initcall = 0;
	if(_g_has_init)
	{
		exit_flag = 0;
		exit_window = 0;
		_set_mode(*gdriver, *gmode);
		_init_img_page();
		return;
	}
	else
	{
		memset(this, 0, sizeof(_graph_setting));
		exit_flag = 0;
		exit_window = 0;
		_init_img_page_f();
	}
	_set_mode(*gdriver, *gmode);
	instance = GetModuleHandle(nullptr);
	::lstrcpy(window_class_name, TEXT("Easy Graphics Engine"));
	::lstrcpy(window_caption, EGE_TITLE);
	{
	//	::SECURITY_ATTRIBUTES sa{0};
		::DWORD pid;

		init_finish = false;
		threadui_handle = ::CreateThread(nullptr, 0, messageloopthread,
			this, CREATE_SUSPENDED, &pid);
		ResumeThread(threadui_handle);
		while(!init_finish)
			::Sleep(1);
	}
	::UpdateWindow(hwnd);
	//初始化鼠标位置数据
	mouse_last_x = dc_w / 2;
	mouse_last_y = dc_h / 2;

	static egeControlBase _egeControlBase;

	if(_g_initoption & INIT_RENDERMANUAL)
		setrendermode(RENDER_MANUAL);
	mouse_show = 1;
}

void
_graph_setting::_init_img_page()
{
	for(int page = 0; page < BITMAP_PAGE_SIZE; ++page)
		if(img_page[page])
			img_page[page]->createimage(dc_w, dc_h);
	_init_graph();
	::ShowWindow(hwnd, SW_SHOW);
}

void
_graph_setting::_init_img_page_f()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
	_g_has_init = true;
}

int
_graph_setting::_kbhit_ex(int flag)
{
	if(exit_window)
		return grNoInitGraph;
	if(flag == 0)
		return _peekkey();
	else
		return _peekallkey(flag);
}

int
_graph_setting::_keystate(int key)
{
	if(key < 0 || key >= MAX_KEY_VCODE)
		return -1;
	if(!(::USHORT(::GetKeyState(key)) & 0x8000))
		keystatemap[key] = 0;
	return keystatemap[key];
}

void
_graph_setting::_on_destroy()
{
	exit_window = 1;
	if(dc)
		::ReleaseDC(hwnd, window_dc);
		// release objects, not finish
	::PostQuitMessage(0);
	if(close_manually && use_force_exit)
		::ExitProcess(0);
}

void
_graph_setting::_on_ime_control(::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	if(wparam == IMC_SETSTATUSWINDOWPOS)
	{
		::HIMC hImc = ImmGetContext(hwnd);

		COMPOSITIONFORM cpf{0, POINT(), ::RECT()};

		cpf.dwStyle = CFS_POINT;
		cpf.ptCurrentPos = *(LPPOINT)lparam;
		::ImmSetCompositionWindow(hImc, &cpf);
	}
}

void
_graph_setting::_on_key(::UINT message, unsigned long keycode, ::LPARAM keyflag)
{
	unsigned msg = 0;

	if(message == WM_KEYDOWN && keycode < MAX_KEY_VCODE)
	{
		msg = 1;
		keystatemap[keycode] = 1;
	}
	if(message == WM_KEYUP && keycode < MAX_KEY_VCODE)
		keystatemap[keycode] = 0;
	if(callback_key)
	{
		int ret;

		if(message == WM_CHAR)
			msg = 2;
		ret = callback_key(callback_key_param, msg, (int)keycode);
		if(ret == 0)
			return;
	}
	msgkey_queue->push(EGEMSG{hwnd, message, keycode, keyflag,
		::GetTickCount(), 0, 0});
}

void
_graph_setting::_on_mouse_button_up(::HWND h_wnd, ::UINT msg, ::WPARAM w_param,
	::LPARAM l_param)
{
	auto* l = &mouse_state_l;
	auto vk = VK_LBUTTON;

	switch(msg)
	{
	case WM_LBUTTONUP:
		break;
	case WM_MBUTTONUP:
		l = &mouse_state_m;
		vk = VK_MBUTTON;
		break;
	case WM_RBUTTONUP:
		l = &mouse_state_r;
		vk = VK_RBUTTON;
		break;
	default:
		assert(false);
		return;
	}
	mouse_lastup_x = (short int)((::UINT)l_param & 0xFFFF);
	mouse_lastup_y = (short int)((::UINT)l_param >> 16);
	*l = 0;
	keystatemap[vk] = 0;
	if(mouse_state_l == 0 && mouse_state_m == 0
		&& mouse_state_r == 0)
		::ReleaseCapture();
	if(h_wnd == hwnd)
		_push_mouse_msg(msg, w_param, l_param);
}

void
_graph_setting::_on_paint(::HWND hwnd)
{
	if(!lock_window)
	{
		::PAINTSTRUCT ps;

		_on_repaint(hwnd, ::BeginPaint(hwnd, &ps));
	}
	else
	{
		::ValidateRect(hwnd, nullptr);
		--update_mark_count;
	}
}

void
_graph_setting::_on_repaint(::HWND hwnd, ::HDC dc)
{
	bool release = false;

	img_timer_update->copyimage(img_page[visual_page]);
	if(!dc)
	{
		dc = ::GetDC(hwnd);
		release = true;
	}

	int left = img_timer_update->m_vpt.left, top = img_timer_update->m_vpt.top;

	::BitBlt(dc, 0, 0, base_w, base_h, img_timer_update->m_hDC,
		base_x - left, base_y - top, SRCCOPY);
	if(release)
		::ReleaseDC(hwnd, dc);
}

void
_graph_setting::_on_setcursor(::HWND hwnd)
{
	if(mouse_show)
		::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
	else
	{
		::RECT rect;
		POINT pt;

		GetCursorPos(&pt);
		::ScreenToClient(hwnd, &pt);
		::GetClientRect(hwnd, &rect);
		if(pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
			::SetCursor(nullptr);
		else
			::SetCursor(::LoadCursor(nullptr, IDC_ARROW));
	}
}

void
_graph_setting::_on_timer(::HWND hwnd, unsigned id)
{
	if(!skip_timer_mark && id == RENDER_TIMER_ID)
	{
		if(update_mark_count <= 0)
		{
			update_mark_count = UPDATE_MAX_CALL;
			_on_repaint(hwnd, nullptr);
		}
		if(timer_stop_mark)
		{
			timer_stop_mark = false;
			skip_timer_mark = true;
		}
	}
}

int
_graph_setting::_peekkey()
{
	EGEMSG msg;

	while(msgkey_queue->pop(msg))
	{
		if(msg.message == WM_CHAR || msg.message == WM_KEYDOWN)
		{
			if(msg.message == WM_KEYDOWN)
				if(msg.wParam <= key_space || (msg.wParam >= key_0
					&& msg.wParam < key_f1) || (msg.wParam >= key_semicolon
					&& msg.wParam <= key_quote))
					continue;
			msgkey_queue->unpop();
			if(msg.message == WM_CHAR)
				return KEYMSG_CHAR | (int(msg.wParam) & 0xFFFF);
			if(msg.message == WM_KEYDOWN)
			{
				if(msg.wParam >= 0x70 && msg.wParam < 0x80)
					return (KEYMSG_DOWN | (int(msg.wParam) + 0x100));
				return (KEYMSG_DOWN | (int(msg.wParam) & 0xFFFF));
			}
			else if(msg.message == WM_KEYUP)
				return KEYMSG_UP | (int(msg.wParam) & 0xFFFF);
		}
	}
	return 0;
}

int
_graph_setting::_peekallkey(int flag)
{
	EGEMSG msg;

	while(msgkey_queue->pop(msg))
	{
		if((msg.message == WM_CHAR && (flag & KEYMSG_CHAR_FLAG)) ||
			(msg.message == WM_KEYUP && (flag & KEYMSG_UP_FLAG)) ||
			(msg.message == WM_KEYDOWN && (flag & KEYMSG_DOWN_FLAG)))
		{
			msgkey_queue->unpop();
			if(msg.message == WM_CHAR)
				return (KEYMSG_CHAR | (int(msg.wParam) & 0xFFFF));
			else if(msg.message == WM_KEYDOWN)
				return (KEYMSG_DOWN | (int(msg.wParam) & 0xFFFF)
					| (msg.lParam & 0x40000000 ? 0 : KEYMSG_FIRSTDOWN));
			else if(msg.message == WM_KEYUP)
				return KEYMSG_UP | (int(msg.wParam) & 0xFFFF);
		}
	}
	return 0;
}

EGEMSG
_graph_setting::_peekmouse()
{
	auto msg = EGEMSG();

	if(msgmouse_queue->empty())
		_dealmessage(NORMAL_UPDATE);
	while(msgmouse_queue->pop(msg))
	{
		msgmouse_queue->unpop();
		return msg;
	}
	return msg;
}

void
_graph_setting::_process_ui_msg(EGEMSG& qmsg)
{
	if((qmsg.flag & 1))
		return;
	qmsg.flag |= 1;
	if(qmsg.message >= WM_KEYFIRST && qmsg.message <= WM_KEYLAST)
	{
		if(qmsg.message == WM_KEYDOWN)
			egectrl_root->keymsgdown(unsigned(qmsg.wParam), 0); // 以后补加flag
		else if(qmsg.message == WM_KEYUP)
			egectrl_root->keymsgup(unsigned(qmsg.wParam), 0); // 以后补加flag
		else if(qmsg.message == WM_CHAR)
			egectrl_root->keymsgchar(unsigned(qmsg.wParam), 0); // 以后补加flag
	}
	else if(qmsg.message >= WM_MOUSEFIRST && qmsg.message <= WM_MOUSELAST)
	{
		int x = (short int)((::UINT)qmsg.lParam & 0xFFFF),
			y = (short int)((::UINT)qmsg.lParam >> 16);
		if(qmsg.message == WM_LBUTTONDOWN)
			egectrl_root->mouse(x, y, mouse_msg_down | mouse_flag_left);
		else if(qmsg.message == WM_LBUTTONUP)
			egectrl_root->mouse(x, y, mouse_msg_up | mouse_flag_left);
		else if(qmsg.message == WM_RBUTTONDOWN)
			egectrl_root->mouse(x, y, mouse_msg_down | mouse_flag_right);
		else if(qmsg.message == WM_RBUTTONUP)
			egectrl_root->mouse(x, y, mouse_msg_up | mouse_flag_right);
		else if(qmsg.message == WM_MOUSEMOVE)
		{
			int flag = 0;

			if(keystatemap[VK_LBUTTON])
				flag |= mouse_flag_left;
			if(keystatemap[VK_RBUTTON])
				flag |= mouse_flag_right;
			egectrl_root->mouse(x, y, mouse_msg_move | flag);
		}
	}
}

void
_graph_setting::_push_mouse_msg(::UINT message, ::WPARAM wparam,
	::LPARAM lparam)
{
	msgmouse_queue->push(EGEMSG{hwnd, message, wparam, lparam, ::GetTickCount(),
		::UINT(mouse_state_m << 2 | mouse_state_r << 1 | mouse_state_l << 0),
		::UINT()});
}

int
_graph_setting::_redraw_window(::HDC dc)
{
	int page = visual_page;
	::HDC hDC = img_page[page]->m_hDC;
	int left = img_page[page]->m_vpt.left,
		top = img_page[page]->m_vpt.top;
	//::HRGN rgn = img_page[page]->m_rgn;

	::BitBlt(dc, 0, 0, base_w, base_h, hDC, base_x - left, base_y - top,
		SRCCOPY);
	update_mark_count = UPDATE_MAX_CALL;
	return 0;
}

int
_graph_setting::_update()
{
	if(exit_window)
		return grNoInitGraph;

	::HDC hdc;
	if(IsWindowVisible(hwnd))
	{
		hdc = window_dc;

		if(!hdc)
			return grNullPointer;
		_redraw_window(hdc);
	}
	else
		update_mark_count = UPDATE_MAX_CALL;
	_get_FPS(0x100);

	::RECT rect, crect;
	::HWND h_wnd;
	int _dw, _dh;

	::GetClientRect(hwnd, &crect);
	::GetWindowRect(hwnd, &rect);

	int w = dc_w, h = dc_h;
	_dw = w - (crect.right - crect.left);
	_dh = h - (crect.bottom - crect.top);
	if(_dw != 0 || _dh != 0)
	{
		h_wnd = ::GetParent(hwnd);
		if(h_wnd)
		{
			::POINT pt{0, 0};

			::ClientToScreen(h_wnd, &pt);
			rect.left -= pt.x;
			rect.top -= pt.y;
			rect.right -= pt.x;
			rect.bottom -= pt.y;
		}
		::SetWindowPos(hwnd, nullptr, 0, 0, rect.right  + _dw - rect.left,
			rect.bottom + _dh - rect.top,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	return grOk;
}

void
_graph_setting::_update_GUI()
{
	using namespace std;
	using namespace placeholders;

	msgkey_queue->process(bind(&_graph_setting::_process_ui_msg, this, _1));
	msgmouse_queue->process(bind(&_graph_setting::_process_ui_msg, this, _1));
}

int
_graph_setting::_waitdealmessage()
{
	if(update_mark_count < UPDATE_MAX_CALL)
	{
		egectrl_root->draw(nullptr);
		_update();
		_update_GUI();
		egectrl_root->update();
	}
	ege_sleep(1);
	return !exit_window;
}

void
_graph_setting::_windowmanager(bool create, struct msg_createwindow* msg)
{
	if(create)
	{
		msg->hwnd = ::CreateWindowExW(msg->exstyle, msg->classname, nullptr,
			msg->style, 0, 0, 0, 0, getHWnd(), (HMENU)msg->id, getHInstance(),
			nullptr);
		if(msg->hEvent)
			::SetEvent(msg->hEvent);
	}
	else
	{
		if(msg->hwnd)
			::DestroyWindow(msg->hwnd);
		if(msg->hEvent)
			::SetEvent(msg->hEvent);
	}
}


namespace
{

_graph_setting&
get_global_state()
{
	static std::unique_ptr<_graph_setting> p(new _graph_setting());

	return *p;
}

}

_graph_setting& graph_setting(get_global_state());

} // namespace ege

