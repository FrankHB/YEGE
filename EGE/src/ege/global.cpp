#include "global.h"
#include "image.h" // for IMAGE;
#include "ege/time.h" // for delay_ms;
#include "ege/ctl.h"
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>
#include <windows.h>
#include <memory> // for std::unique_ptr;
#include <functional> // for std::bind;
#include <mutex> // for std::once_flag, std::call_once;
#include "head.h"

#ifdef _WIN64
#define ARCH "x64"
#else
#define ARCH "x86"
#endif

#define TOSTRING_(x) #x
#define TOSTRING(x) TOSTRING_(x)
#define GCC_VER TEXT(TOSTRING(__GNUC__)) TEXT(".") TEXT(TOSTRING(__GNUC_MINOR__)) TEXT(".") TEXT(TOSTRING(__GNUC_PATCHLEVEL__))

#define EGE_TITLE TEXT("yEGE13.04 ") TEXT("GCC") GCC_VER TEXT(ARCH)

namespace ege
{

namespace
{

GUIApplication&
FetchGUIApplicationInstance()
{
	static GUIApplication app;

	return app;
}

unique_ptr<Panel> ys_pnl;
std::thread ys_thrd;
Host::Window* ys_window;

} // unnamed namespace;


int _g_initoption(INIT_DEFAULT);
bool _g_initcall;

int update_mark_count; //更新标记
egeControlBase* egectrl_root;
egeControlBase* egectrl_focus;


namespace
{

::DWORD g_windowstyle(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
	| WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_VISIBLE);
::DWORD g_windowexstyle(WS_EX_LEFT | WS_EX_LTRREADING);
int _g_windowpos_x(CW_USEDEFAULT), _g_windowpos_y(CW_USEDEFAULT);

CALLBACK ::BOOL
EnumResNameProc(::HMODULE hModule, ::LPCTSTR, ::LPTSTR lpszName,
	::LONG_PTR lParam)
{
	if(const auto hico = ::HICON(::LoadImage(hModule, lpszName, IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE)))
	{
		*(reinterpret_cast<::HICON*>(lParam)) = hico;
		return {};
	}
	return true;
}

} // unnamed namespace;

float
_get_FPS(int);

void
_set_initmode(int mode, int x, int y)
{
	_g_initcall = true;
	_g_initoption = mode;
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
	_g_windowpos_x = x;
	_g_windowpos_y = y;
}


const ::TCHAR _graph_setting::window_class_name[32]
	{TEXT("Easy Graphics Engine")};
const ::TCHAR _graph_setting::window_caption[128]{EGE_TITLE};

_graph_setting::_graph_setting(int gdriver_n, int* gmode)
{
	static std::once_flag init_flag;

	std::call_once(init_flag, [this]{
		static ::ULONG_PTR g_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;

		Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, {});

		static ::WNDCLASSEX wcex;

		wcex.cbSize = sizeof(wcex);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = getProcfunc();
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = get_instance();
		wcex.hIcon = ::LoadIcon({}, IDI_WINLOGO);
		wcex.hCursor = ::LoadCursor({}, IDC_ARROW);
		wcex.hbrBackground = ::HBRUSH(COLOR_WINDOW + 1);
		wcex.lpszClassName = window_class_name;

		const auto load([&](::LPCTSTR rt){
			::HICON hico = {};

			EnumResourceNames(get_instance(), rt, EnumResNameProc,
				::LONG_PTR(&hico));
			if(hico)
				wcex.hIcon = hico;
			return hico;
		});
		do
		{
			if(load(RT_ANIICON))
				break;
			if(load(RT_GROUP_ICON))
				break;
			if(load(RT_ICON))
				break;
		}while(0);
		::RegisterClassEx(&wcex);
	});

	assert(gdriver_n);

	if(gdriver_n == TRUECOLORSIZE)
	{
		assert(gmode);
		::RECT rect;

		::GetWindowRect(GetDesktopWindow(), &rect);
		dc_w = short(*gmode & 0xFFFF);
		dc_h = short(unsigned(*gmode >> 16));
		if(dc_w < 0)
			dc_w = rect.right;
		if(dc_h < 0)
			dc_h = rect.bottom;
	}

	FetchGUIApplicationInstance();
	ys_pnl.reset(new Panel(Size(SDst(dc_w), SDst(dc_h))));
	ShowTopLevel(*ys_pnl, g_windowstyle, g_windowexstyle);
	ys_window
		= dynamic_cast<HostRenderer&>(ys_pnl->GetRenderer()).GetWindowPtr();
}

_graph_setting::~_graph_setting()
{
	yassume(!ys_thrd.joinable());

	ys_pnl.reset();
}

bool
_graph_setting::_is_run() const
{
	return ui_thread.joinable() || ys_thrd.joinable();
}

bool
_graph_setting::_is_window_exit() const
{
	return !_is_run();
}

void
_graph_setting::_flushkey()
{
	EGEMSG msg;

	if(msgkey_queue.empty())
		_update_if_necessary();
	if(!msgkey_queue.empty())
		while(msgkey_queue.pop(msg))
			;
}

void
_graph_setting::_flushmouse()
{
	EGEMSG msg;

	if(msgmouse_queue.empty())
		_update_if_necessary();
	if(!msgmouse_queue.empty())
		while(msgmouse_queue.pop(msg))
			;
}

int
_graph_setting::_getch()
{
	if(_is_window_exit())
		return grNoInitGraph;
	{
		int key;
		EGEMSG msg;
		::DWORD dw = GetTickCount();
		do
		{
			key = _kbhit();
			if(key < 0)
				break;
			if(key > 0)
				if((key = _getkey_p()))
				{
					msg = msgkey_queue.last();
					if(dw < msg.time + 1000)
					{
						if(((key & KEYMSG_DOWN) && (msg.wParam >= 0x70
							&& msg.wParam < 0x80)) || (msg.wParam > ' '
							&& msg.wParam < '0'))
							key |= 0x100;
						return key & 0xFFFF;
					}
				}
		}while(_is_run() && _waitdealmessage());
	}
	return 0;
}

int
_graph_setting::_getflush()
{
	EGEMSG msg;
	int lastkey = 0;

	if(msgkey_queue.empty())
		_update_if_necessary();
	if(!msgkey_queue.empty())
		while(msgkey_queue.pop(msg))
			if(msg.message == WM_CHAR)
				lastkey = int(msg.wParam);
	return lastkey;
}

key_msg
_graph_setting::_getkey()
{
	key_msg ret{0, key_msg_none, 0};

	if(_is_run())
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
		}while(_is_run() && _waitdealmessage());
	}
	return ret;
}

int
_graph_setting::_getkey_p()
{
	EGEMSG msg;

	while(msgkey_queue.pop(msg))
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

	if(_is_window_exit())
		return mmsg;

	EGEMSG msg;

	do
	{
		msgmouse_queue.pop(msg);
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
	}while(_is_run() && _waitdealmessage());
	return mmsg;
}

void
_graph_setting::_init_graph_x()
{
	static std::once_flag init_flag;

	yassume(ys_window);

	const auto native_ys_window(ys_window->GetNativeHandle());

	yassume(ys_window->GetNativeHandle());

	std::call_once(init_flag, [this, native_ys_window]{
		ys_thrd = std::thread([&]{
			Execute(FetchGUIApplicationInstance());
		});

	//	::SECURITY_ATTRIBUTES sa{};
	//	::DWORD pid;
		bool init_finish{};

		ui_thread = std::thread([this, native_ys_window, &init_finish]{
			//执行应用程序初始化
			::SetWindowText(native_ys_window, window_caption),
		//	ys_window->Move(Point(_g_windowpos_x, _g_windowpos_y)),
			ys_window->Resize(Size(dc_w + ::GetSystemMetrics(SM_CXFRAME) * 2,
				dc_h + ::GetSystemMetrics(SM_CYFRAME)
				+ ::GetSystemMetrics(SM_CYCAPTION) * 2));
			hwnd = ::CreateWindowEx(0, window_class_name, window_caption,
				WS_CHILD, _g_windowpos_x, _g_windowpos_y,
				dc_w + ::GetSystemMetrics(SM_CXFRAME) * 2,
				dc_h + ::GetSystemMetrics(SM_CYFRAME)
				+ ::GetSystemMetrics(SM_CYCAPTION) * 2, native_ys_window,
				{}, get_instance(), {});
			if(!hwnd)
				return ::DWORD(0xFFFFFFFF);
			//图形初始化
			window_dc = ::GetDC(hwnd);
			//::ReleaseDC(hwnd, window_dc);
			mouse_show = {};
			use_force_exit = !(_g_initoption & INIT_NOFORCEEXIT);
			if(!use_force_exit)
				FetchGUIApplicationInstance().GetHost()
					.ExitOnAllWindowThreadCompleted = {};
			init_finish = true;

			::MSG msg;

			while(_is_run())
				if(::GetMessage(&msg, {}, 0, 0))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				else
					::Sleep(1);
			return ::DWORD(0);
		});
		while(!init_finish)
			::Sleep(1);
		::ShowWindow(hwnd, SW_SHOW);
		::UpdateWindow(hwnd);
		//初始化鼠标位置数据
		mouse_last_x = dc_w / 2;
		mouse_last_y = dc_h / 2;

		static egeControlBase _egeControlBase;

		if(_g_initoption & INIT_RENDERMANUAL)
			setrendermode(RENDER_MANUAL);
		mouse_show = true;
	});
	window_setviewport(0, 0, dc_w, dc_h);
	::SetWindowLongPtrW(hwnd, GWLP_USERDATA, ::LONG_PTR(this));
	::UpdateWindow(native_ys_window);
	if(g_windowexstyle & WS_EX_TOPMOST)
		::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE);
}

int
_graph_setting::_kbhit()
{
	return _is_window_exit() ? int(grNoInitGraph) : int(_peekkey());
}

int
_graph_setting::_kbmsg()
{
	return _is_window_exit() ? int(grNoInitGraph) : int(_peekallkey(1));
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

bool
_graph_setting::_mousemsg()
{
	return _is_window_exit() ? false : bool(_peekmouse().hwnd);
}

void
_graph_setting::_on_destroy()
{
	assert(_is_run());

	if(get_pages().active_dc)
		::ReleaseDC(hwnd, window_dc);
		// release objects, not finish
	::PostQuitMessage(0);
	ui_thread.detach();
	if(use_force_exit)
	{
		YSLib::PostQuitMessage(0);
		ys_thrd.join();
	//	::ExitProcess(0);
	}
}

void
_graph_setting::_on_ime_control(::HWND hwnd, ::WPARAM wparam, ::LPARAM lparam)
{
	if(wparam == IMC_SETSTATUSWINDOWPOS)
	{
		::HIMC hImc = ImmGetContext(hwnd);

		COMPOSITIONFORM cpf{0, ::POINT(), ::RECT()};

		cpf.dwStyle = CFS_POINT;
		cpf.ptCurrentPos = *reinterpret_cast<LPPOINT>(lparam);
		::ImmSetCompositionWindow(hImc, &cpf);
	}
}

void
_graph_setting::_on_key(::UINT message, unsigned long keycode, ::LPARAM keyflag)
{
	if(message == WM_KEYDOWN && keycode < MAX_KEY_VCODE)
		keystatemap[keycode] = 1;
	if(message == WM_KEYUP && keycode < MAX_KEY_VCODE)
		keystatemap[keycode] = 0;
	msgkey_queue.push(EGEMSG{hwnd, message, keycode, keyflag,
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
	mouse_lastup_x = short(l_param & 0xFFFF);
	mouse_lastup_y = short(::UINT(l_param) >> 16);
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
	::PAINTSTRUCT ps;
	::HDC dc(::BeginPaint(hwnd, &ps));
	get_pages().paint(dc);
	::EndPaint(hwnd, &ps);
}

void
_graph_setting::_on_setcursor(::HWND hwnd)
{
	if(mouse_show)
		::SetCursor(::LoadCursor({}, IDC_ARROW));
	else
	{
		::RECT rect;
		::POINT pt;

		::GetCursorPos(&pt);
		::ScreenToClient(hwnd, &pt);
		::GetClientRect(hwnd, &rect);
		if(pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top && pt.y <= rect.bottom)
			::SetCursor({});
		else
			::SetCursor(::LoadCursor({}, IDC_ARROW));
	}
}

int
_graph_setting::_peekkey()
{
	EGEMSG msg;

	while(msgkey_queue.pop(msg))
	{
		if(msg.message == WM_CHAR || msg.message == WM_KEYDOWN)
		{
			if(msg.message == WM_KEYDOWN)
				if(msg.wParam <= key_space || (msg.wParam >= key_0
					&& msg.wParam < key_f1) || (msg.wParam >= key_semicolon
					&& msg.wParam <= key_quote))
					continue;
			msgkey_queue.unpop();
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

	while(msgkey_queue.pop(msg))
	{
		if((msg.message == WM_CHAR && (flag & KEYMSG_CHAR_FLAG)) ||
			(msg.message == WM_KEYUP && (flag & KEYMSG_UP_FLAG)) ||
			(msg.message == WM_KEYDOWN && (flag & KEYMSG_DOWN_FLAG)))
		{
			msgkey_queue.unpop();
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

	if(msgmouse_queue.empty())
		_update_if_necessary();
	while(msgmouse_queue.pop(msg))
	{
		msgmouse_queue.unpop();
		return msg;
	}
	return msg;
}

void
_graph_setting::_process_ui_msg(EGEMSG& qmsg)
{
	if(qmsg.flag & 1)
		return;
	qmsg.flag |= 1;
	if(qmsg.message >= WM_KEYFIRST && qmsg.message <= WM_KEYLAST)
		switch(qmsg.message)
		{
		case WM_KEYDOWN:
			egectrl_root->keymsgdown(unsigned(qmsg.wParam), 0); // 以后补加flag
			break;
		case WM_KEYUP:
			egectrl_root->keymsgup(unsigned(qmsg.wParam), 0); // 以后补加flag
			break;
		case WM_CHAR:
			egectrl_root->keymsgchar(unsigned(qmsg.wParam), 0); // 以后补加flag
		default:
			break;
		}
	else if(qmsg.message >= WM_MOUSEFIRST && qmsg.message <= WM_MOUSELAST)
	{
		int x = short(qmsg.lParam & 0xFFFF),
			y = short(::UINT(qmsg.lParam) >> 16);

		switch(qmsg.message)
		{
		case WM_LBUTTONDOWN:
			egectrl_root->mouse(x, y, mouse_msg_down | mouse_flag_left);
			break;
		case WM_LBUTTONUP:
			egectrl_root->mouse(x, y, mouse_msg_up | mouse_flag_left);
			break;
		case WM_RBUTTONDOWN:
			egectrl_root->mouse(x, y, mouse_msg_down | mouse_flag_right);
			break;
		case WM_RBUTTONUP:
			egectrl_root->mouse(x, y, mouse_msg_up | mouse_flag_right);
			break;
		case WM_MOUSEMOVE:
			{
				int flag = 0;

				if(keystatemap[VK_LBUTTON])
					flag |= mouse_flag_left;
				if(keystatemap[VK_RBUTTON])
					flag |= mouse_flag_right;
				egectrl_root->mouse(x, y, mouse_msg_move | flag);
			}
		default:
			break;
		}
	}
}

void
_graph_setting::_push_mouse_msg(::UINT message, ::WPARAM wparam,
	::LPARAM lparam)
{
	msgmouse_queue.push(EGEMSG{hwnd, message, wparam, lparam, ::GetTickCount(),
		::UINT(mouse_state_m << 2 | mouse_state_r << 1 | mouse_state_l << 0),
		::UINT()});
}

int
_graph_setting::_show_mouse(bool bShow)
{
	std::swap(bShow, mouse_show);
	return bShow;
}

int
_graph_setting::_update()
{
	if(_is_window_exit())
		return grNoInitGraph;

	if(IsWindowVisible(hwnd))
	{
		if(!window_dc)
			return grNullPointer;
		get_pages().update();
	}
	update_mark_count = UPDATE_MAX_CALL;
	_get_FPS(0x100);

	::RECT crect;

	::GetClientRect(hwnd, &crect);

	int _dw = dc_w - (crect.right - crect.left),
		_dh = dc_h - (crect.bottom - crect.top);
	if(_dw != 0 || _dh != 0)
	{
		::RECT rect;

		::GetWindowRect(hwnd, &rect);
		if(::HWND h_parent_wnd = ::GetParent(hwnd))
		{
			::POINT pt{0, 0};

			::ClientToScreen(h_parent_wnd, &pt);
			rect.left -= pt.x;
			rect.top -= pt.y;
			rect.right -= pt.x;
			rect.bottom -= pt.y;
		}
		::SetWindowPos(hwnd, {}, 0, 0, rect.right  + _dw - rect.left,
			rect.bottom + _dh - rect.top,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
	return grOk;
}

void
_graph_setting::_update_if_necessary()
{
	if(update_mark_count <= 0)
		_update();
}

void
_graph_setting::_update_GUI()
{
	using namespace std;
	using namespace placeholders;

	msgkey_queue.process(bind(&_graph_setting::_process_ui_msg, this, _1));
	msgmouse_queue.process(bind(&_graph_setting::_process_ui_msg, this, _1));
}

int
_graph_setting::_waitdealmessage()
{
	if(update_mark_count < UPDATE_MAX_CALL)
	{
		egectrl_root->draw({});
		_update();
		_update_GUI();
		egectrl_root->update();
	}
	ege_sleep(1);
	return _is_run();
}

void
_graph_setting::_window_create(msg_createwindow& msg)
{
	msg.hwnd = ::CreateWindowExW(msg.exstyle, msg.classname, {},
		msg.style, 0, 0, 0, 0, getHWnd(), HMENU(msg.id), getHInstance(), {});
	if(msg.hEvent)
		::SetEvent(msg.hEvent);
}

void
_graph_setting::_window_destroy(msg_createwindow& msg)
{
	if(msg.hwnd)
		::DestroyWindow(msg.hwnd);
	if(msg.hEvent)
		::SetEvent(msg.hEvent);
}


_pages::_pages()
	: gstate(get_global_state()), active_dc(gstate._get_window_dc())
{
	check_page(0);
	active_dc = img_page[0]->getdc();
	imgtarget = img_page[active_page];
	update_mark_count = 0;
}

void
_pages::check_page(int page) const
{
	if(!img_page[page])
	{
		const int dc_w(gstate._get_dc_w());
		const int dc_h(gstate._get_dc_h());

		img_page[page] = new IMAGE(active_dc, dc_w, dc_h);
	}
}

IMAGE&
_pages::get_apage_ref() const
{
	check_page(active_page);

	return *img_page[active_page];
}

::HDC
_pages::get_image_context() const
{
	return imgtarget ? imgtarget->getdc() : active_dc;
}

IMAGE&
_pages::get_vpage_ref() const
{
	check_page(visual_page);

	return *img_page[visual_page];
}

void
_pages::paint(::HDC dc)
{
	auto& vpage(get_vpage_ref());
	const int left(vpage.m_vpt.left), top(vpage.m_vpt.top);

	::BitBlt(dc, 0, 0, base_w, base_h, vpage.getdc(),
		base_x - left, base_y - top, SRCCOPY);
}

void
_pages::set_apage(int page)
{
	check_page(page);
	active_page = page;
	active_dc = img_page[page]->getdc();
}

int
_pages::set_target(IMAGE* pbuf)
{
	imgtarget_set = pbuf;
	imgtarget = pbuf ? pbuf : img_page[active_page];
	return 0;
}

void
_pages::set_vpage(int page)
{
	check_page(page);
	visual_page = page;
	update_mark_count = 0;
}

void
_pages::update()
{
	auto& vpage(get_vpage_ref());

	::BitBlt(gstate._get_window_dc(), 0, 0, base_w, base_h, vpage.getdc(),
		base_x - vpage.m_vpt.left, base_y - vpage.m_vpt.top, SRCCOPY);
}


_graph_setting&
get_global_state(int gdriver_n, int* gmode)
{
	static std::unique_ptr<_graph_setting>
		p(new _graph_setting(gdriver_n, gmode));

	return *p;
}

_pages&
get_pages()
{
	static std::unique_ptr<_pages> p(new _pages());

	return *p;
}

} // namespace ege;

