#include "global.h"
#include "image.h" // for IMAGE;
#include "ege/time.h" // for delay_ms;
#include "ege/ctl.h"
#include "ege.h"
#include <wtypes.h> // for ::PROPID required by <gdiplus.h>;
#include <gdiplus.h>
#include <memory> // for std::unique_ptr;
#include <functional> // for std::bind;
#include <mutex> // for std::once_flag, std::call_once;
#include "head.h"

#ifdef _WIN64
#define ARCH L"x64"
#else
#define ARCH L"x86"
#endif

#define TOSTRING_(x) #x
#define TOSTRING(x) TOSTRING_(x)
#define GCC_VER TOSTRING(__GNUC__) L"." TOSTRING(__GNUC_MINOR__) L"." TOSTRING(__GNUC_PATCHLEVEL__)

#define EGE_TITLE L"yEGE13.04 GCC" GCC_VER ARCH

namespace ege
{

using namespace Host;

namespace
{

const wchar_t window_class_name[32]
	{L"Easy Graphics Engine"};
const wchar_t window_caption[128]{EGE_TITLE};

int _g_initoption(INIT_DEFAULT);

::LRESULT CALLBACK
wndproc(::HWND hWnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
{
	auto& app(FetchEGEApplication());

	switch(message)
	{
	case WM_PAINT:
		app._on_paint(hWnd);
		break;
	case WM_CLOSE:
		if(app.callback_close)
			app.callback_close();
		else
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	//	if(hWnd == app.hwnd)
		app._on_key(message, static_cast<unsigned long>(wParam), lParam);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		app.mouse_lastclick_x = short(lParam & 0xFFFF);
		app.mouse_lastclick_y = short(::UINT(lParam) >> 16);
		app.keystatemap[VK_LBUTTON] = 1;
		::SetCapture(hWnd);
		app.mouse_state_l = 1;
		if(hWnd == app._get_hwnd())
			app._push_mouse_msg( message, wParam, lParam);
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		app.mouse_lastclick_x = short(lParam & 0xFFFF);
		app.mouse_lastclick_y = short(::UINT(lParam) >> 16);
		app.keystatemap[VK_MBUTTON] = 1;
		::SetCapture(hWnd);
		app.mouse_state_m = 1;
		if(hWnd == app._get_hwnd())
			app._push_mouse_msg(message, wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		app.mouse_lastclick_x = short(lParam & 0xFFFF);
		app.mouse_lastclick_y = short(::UINT(lParam) >> 16);
		app.keystatemap[VK_RBUTTON] = 1;
		::SetCapture(hWnd);
		app.mouse_state_r = 1;
		if(hWnd == app._get_hwnd())
			app._push_mouse_msg( message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		app._on_mouse_button_up(hWnd, message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		app.mouse_last_x = short(lParam & 0xFFFF);
		app.mouse_last_y = short(::UINT(lParam) >> 16);
		if(hWnd == app._get_hwnd() && (app.mouse_lastup_x
			!= app.mouse_last_x || app.mouse_lastup_y
			!= app.mouse_last_y))
			app._push_mouse_msg(message, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		{
			::POINT pt{short(lParam & 0xFFFF), short(::UINT(lParam) >> 16)};

			::ScreenToClient(app._get_hwnd(), &pt);
			app.mouse_last_x = pt.x;
			app.mouse_last_y = pt.y;
			lParam = static_cast<unsigned short>(short(app.mouse_last_y))
				<< 16 | static_cast<unsigned short>(short(app.mouse_last_x));
		}
		if(hWnd == app._get_hwnd())
			app._push_mouse_msg(message, wParam, lParam);
		break;
	case WM_SETCURSOR:
		app._on_setcursor(hWnd);
		return TRUE;
	case WM_USER + 1:
		EGEApplication::_window_handle_wm_user_1(lParam, wParam);
		break;
	case WM_USER + 2:
		::SetFocus(::HWND(lParam));
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

} // unnamed namespace;


bool _g_initcall;
int update_mark_count; //更新标记
egeControlBase* egectrl_root;
egeControlBase* egectrl_focus;


namespace
{

yconstexpr unsigned long c_style_base(WS_CLIPCHILDREN | WS_VISIBLE),
	c_style_border(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
	| c_style_base), c_style_ex(WS_EX_LEFT | WS_EX_LTRREADING);

unsigned long g_wstyle(c_style_border), g_wstyle_ex(c_style_ex);
int g_wpos_x(CW_USEDEFAULT), g_wpos_y(CW_USEDEFAULT);

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

::ULONG_PTR g_gdiplusToken;

} // unnamed namespace;

void
setinitmode(int mode, int x, int y)
{
	_g_initcall = true;
	_g_initoption = mode;
	g_wstyle = mode & INIT_NOBORDER ? ((mode & INIT_CHILD
		? WS_CHILDWINDOW : WS_POPUP) | c_style_ex) : c_style_border;
	g_wstyle_ex = c_style_ex;
	if(mode & INIT_TOPMOST)
		g_wstyle_ex |= WS_EX_TOPMOST;
	g_wpos_x = x;
	g_wpos_y = y;
}


EGEApplication::EGEApplication(int gdriver_n, int* gmode)
{
	static std::unique_ptr<ImageCodec> p(new ImageCodec());
	static ::ULONG_PTR g_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, {});

	static ::WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = wndproc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetInstance();
	wcex.hIcon = ::LoadIcon({}, IDI_WINLOGO);
	wcex.hCursor = ::LoadCursor({}, IDC_ARROW);
	wcex.hbrBackground = ::HBRUSH(COLOR_WINDOW + 1);
	wcex.lpszClassName = window_class_name;

	const auto load([&](::LPCTSTR rt){
		::HICON hico = {};

		EnumResourceNames(GetInstance(), rt, EnumResNameProc,
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
	::RegisterClassExW(&wcex);

	assert(gdriver_n != 0);

	if(gdriver_n == TRUECOLORSIZE)
	{
		::RECT rect;

		::GetWindowRect(GetDesktopWindow(), &rect);
		dc_w = short(Deref(gmode) & 0xFFFF);
		dc_h = short(unsigned(*gmode >> 16));
		if(dc_w < 0)
			dc_w = rect.right;
		if(dc_h < 0)
			dc_h = rect.bottom;
	}
	ys_pnl.reset(new Panel({Point::Invalid, SDst(dc_w), SDst(dc_h)}));
	ys_window = dynamic_cast<HostRenderer&>(ShowTopLevel(*ys_pnl, g_wstyle,
		g_wstyle_ex)).GetWindowPtr();
	ys_window->MessageMap[WM_DESTROY] += [this]{
		_uninit();
	};
}

EGEApplication::~EGEApplication()
{
	_uninit();
	ys_pnl.reset();
	Gdiplus::GdiplusShutdown(g_gdiplusToken);
}

bool
EGEApplication::_is_run() const
{
	return ui_thread.joinable() || ys_thrd.joinable();
}

void
EGEApplication::_flush_key_mouse(bool key)
{
	auto& q(key ? msgkey_queue : msgmouse_queue);

	if(q.empty())
		_update_if_necessary();
	q.clear();
}

int
EGEApplication::_get_input(get_input_op op)
{
	if(_is_run())
	{
		switch(op)
		{
		case get_input_op::getch:
			do
			{
				int key = _peekkey();
				if(key < 0)
					break;
				if(key > 0)
					key = _getkey_p();
			} while(_is_run() && _waitdealmessage());
			break;
		case get_input_op::kbhit:
			return _peekkey();
		case get_input_op::kbmsg:
			return _peekallkey(1);
		}
		return 0;
	}
	return grNoInitGraph;
}

int
EGEApplication::_getflush()
{
	int lastkey = 0;

	if(msgkey_queue.empty())
		_update_if_necessary();
	while(!msgkey_queue.empty())
	{
		const auto& msg(msgkey_queue.top());

		if(msg.message == WM_CHAR)
			lastkey = int(msg.wParam);
		msgkey_queue.pop();
	}
	return lastkey;
}

key_msg
EGEApplication::_getkey()
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
		} while(_is_run() && _waitdealmessage());
	}
	return ret;
}

int
EGEApplication::_getkey_p()
{
	while(!msgkey_queue.empty())
	{
		const auto msg(msgkey_queue.top());

		msgkey_queue.pop();
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
	}
	return 0;
}

mouse_msg
EGEApplication::_getmouse()
{
	auto mmsg = mouse_msg();

	if(_is_run())
	{
		EGEMSG msg;

		do
		{
			if(!msgmouse_queue.empty())
			{
				msg = msgkey_queue.top();
				msgkey_queue.pop();
			}
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
	}
	return mmsg;
}

void
EGEApplication::_init_graph_x()
{
	static std::once_flag init_flag;

	const auto native_ys_window(Deref(ys_window).GetNativeHandle());

	std::call_once(init_flag, [this, native_ys_window]{
		ys_thrd = std::thread([this]{
			Execute(*this);
		});

	//	::SECURITY_ATTRIBUTES sa{};
	//	unsigned long pid;
		std::atomic<bool> init_finish{{}};

		ui_thread = std::thread([this, native_ys_window, &init_finish]{
			::SetWindowTextW(Nonnull(native_ys_window), window_caption),
		//	ys_window->Move(Point(g_wpos_x, g_wpos_y)),
			hwnd = ::CreateWindowExW(0, window_class_name, window_caption,
				WS_CHILD, g_wpos_x, g_wpos_y,
				dc_w + ::GetSystemMetrics(SM_CXFRAME) * 2,
				dc_h + ::GetSystemMetrics(SM_CYFRAME)
				+ ::GetSystemMetrics(SM_CYCAPTION) * 2, native_ys_window,
				{}, GetInstance(), {});
			if(!hwnd)
			{
				init_finish = true;
				// NOTE: 'return unsigned long(0xFFFFFFFFUL)' in lambda cause
				//	G++ 4.9 to generate wrong code and fail in runtime.
				return 0xFFFFFFFFUL;
			}
			window_dc = ::GetDC(hwnd);
			mouse_show = {};
			use_force_exit = !(_g_initoption & INIT_NOFORCEEXIT);
			if(!use_force_exit)
				FetchEnvironment().ExitOnAllWindowThreadCompleted = {};
			init_finish = true;

			::MSG msg;

			while(ys_thrd.joinable())
				if(::GetMessage(&msg, {}, 0, 0))
				{
					if(msg.message == WM_QUIT)
						break;
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
				else
					::Sleep(1);
			::ReleaseDC(hwnd, window_dc);
			window_dc = {};
			if(::IsWindow(hwnd))
				::DestroyWindow(hwnd);
			hwnd = {};
			return 0UL;
		});
		while(!init_finish)
			::Sleep(1);
		::ShowWindow(hwnd, SW_SHOW);
		::UpdateWindow(hwnd);
		//初始化鼠标位置数据
		mouse_last_x = dc_w / 2;
		mouse_last_y = dc_h / 2;

		static egeControlBase _egeControlBase;

		mouse_show = true;
	});
	window_setviewport(0, 0, dc_w, dc_h);
	::SetWindowLongPtrW(hwnd, GWLP_USERDATA, ::LONG_PTR(this));
	::UpdateWindow(native_ys_window);
	if(g_wstyle_ex & WS_EX_TOPMOST)
		::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

int
EGEApplication::_keystate(int key)
{
	if(key < 0 || key >= MAX_KEY_VCODE)
		return -1;
	if(!(::USHORT(::GetKeyState(key)) & 0x8000))
		keystatemap[key] = 0;
	return keystatemap[key];
}

bool
EGEApplication::_mousemsg()
{
	return _is_run() && bool(_peekmouse().hwnd);
}

void
EGEApplication::_on_key(unsigned message, unsigned long keycode, ::LPARAM keyflag)
{
	if(message == WM_KEYDOWN && keycode < MAX_KEY_VCODE)
		keystatemap[keycode] = 1;
	if(message == WM_KEYUP && keycode < MAX_KEY_VCODE)
		keystatemap[keycode] = 0;
	msgkey_queue.push(EGEMSG{hwnd, message, keycode, keyflag,
		::GetTickCount(), 0, 0});
}

void
EGEApplication::_on_mouse_button_up(::HWND h_wnd, unsigned msg, ::WPARAM w_param,
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
		yassume(false);
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
EGEApplication::_on_paint(::HWND hwnd)
{
	::PAINTSTRUCT ps;
	::HDC dc(::BeginPaint(hwnd, &ps));
	get_pages().paint(dc);
	::EndPaint(hwnd, &ps);
}

void
EGEApplication::_on_setcursor(::HWND hwnd)
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
EGEApplication::_peekkey()
{
	while(!msgkey_queue.empty())
	{
		const auto& msg(msgkey_queue.top());

		if(msg.message == WM_CHAR || msg.message == WM_KEYDOWN)
		{
			if(msg.message == WM_KEYDOWN)
				if(msg.wParam <= key_space || (msg.wParam >= key_0
					&& msg.wParam < key_f1) || (msg.wParam >= key_semicolon
					&& msg.wParam <= key_quote))
				{
					msgkey_queue.pop();
					continue;
				}
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
		else
			msgkey_queue.pop();
	}
	return 0;
}

int
EGEApplication::_peekallkey(int flag)
{
	while(!msgkey_queue.empty())
	{
		const auto& msg(msgkey_queue.top());

		if((msg.message == WM_CHAR && (flag & KEYMSG_CHAR_FLAG)) ||
			(msg.message == WM_KEYUP && (flag & KEYMSG_UP_FLAG)) ||
			(msg.message == WM_KEYDOWN && (flag & KEYMSG_DOWN_FLAG)))
		{
			if(msg.message == WM_CHAR)
				return (KEYMSG_CHAR | (int(msg.wParam) & 0xFFFF));
			else if(msg.message == WM_KEYDOWN)
				return (KEYMSG_DOWN | (int(msg.wParam) & 0xFFFF)
					| (msg.lParam & 0x40000000 ? 0 : KEYMSG_FIRSTDOWN));
			else if(msg.message == WM_KEYUP)
				return KEYMSG_UP | (int(msg.wParam) & 0xFFFF);
		}
		else
			msgkey_queue.pop();
	}
	return 0;
}

EGEMSG
EGEApplication::_peekmouse()
{
	if(msgmouse_queue.empty())
		_update_if_necessary();
	if(!msgmouse_queue.empty())
		return msgmouse_queue.top();
	return {};
}

void
EGEApplication::_process_queues()
{
	using namespace std;
	using namespace placeholders;

	msgkey_queue.process(bind(&EGEApplication::_process_ui_msg, this, _1));
	msgmouse_queue.process(bind(&EGEApplication::_process_ui_msg, this, _1));
}

void
EGEApplication::_process_ui_msg(EGEMSG& qmsg)
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
			y = short(unsigned(qmsg.lParam) >> 16);

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
EGEApplication::_push_mouse_msg(unsigned message, ::WPARAM wparam,
	::LPARAM lparam)
{
	msgmouse_queue.push(EGEMSG{hwnd, message, wparam, lparam, ::GetTickCount(),
		unsigned(mouse_state_m << 2 | mouse_state_r << 1 | mouse_state_l << 0),
		unsigned()});
}

int
EGEApplication::_show_mouse(bool bShow)
{
	std::swap(bShow, mouse_show);
	return bShow;
}

void
EGEApplication::_uninit()
{
	YTraceDe(Informative, "Ready to call destroy method.");
	if(::IsWindow(hwnd))
		::ShowWindow(hwnd, SW_HIDE);
	YSLib::PostQuitMessage(0);
	if(ui_thread.joinable())
		ui_thread.detach();
	YTraceDe(Debug, "UI thread detached.");
	if(ys_thrd.joinable())
		ys_thrd.join();
	YTraceDe(Debug, "YSLib main thread finished.");
	if(use_force_exit)
		::ExitProcess(0);
	yassume(!_is_run());
	YTraceDe(Informative, "Destroy call finished.");
}

void
EGEApplication::_update()
{
	if(_is_run())
	{
		if(IsWindowVisible(hwnd) && window_dc)
			get_pages().update();
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
				rect.bottom + _dh - rect.top, SWP_NOACTIVATE | SWP_NOMOVE
				| SWP_NOOWNERZORDER | SWP_NOZORDER);
		}
	}
}

void
EGEApplication::_update_if_necessary()
{
	if(update_mark_count <= 0)
		_update();
}

bool
EGEApplication::_waitdealmessage()
{
	if(update_mark_count < UPDATE_MAX_CALL)
	{
		egectrl_root->draw({});
		_update();
		_process_queues();
		egectrl_root->update();
	}
	ege_sleep(1);
	return _is_run();
}

void
EGEApplication::_window_handle_wm_user_1(::LPARAM l, ::WPARAM w)
{
	auto& msg(Deref(reinterpret_cast<msg_createwindow*>(l)));

	if(w != 0)
	{
		msg.hwnd = ::CreateWindowExW(msg.exstyle, msg.classname, {}, msg.style,
			0, 0, 0, 0, getHWnd(), ::HMENU(msg.id), getHInstance(), {});
		if(msg.hEvent)
			::SetEvent(msg.hEvent);
	}
	else
	{
		if(msg.hwnd)
			::DestroyWindow(msg.hwnd);
		if(msg.hEvent)
			::SetEvent(msg.hEvent);
	}
}


_pages::_pages()
	: gstate(FetchEGEApplication()), active_dc(gstate._get_window_dc())
{
	check_page(0);
	active_dc = img_page[0]->getdc();
	imgtarget = img_page[active_page].get();
	update_mark_count = 0;
}

void
_pages::check_page(int page) const
{
	if(!img_page[page])
	{
		const int dc_w(gstate._get_dc_w());
		const int dc_h(gstate._get_dc_h());

		img_page[page].reset(new IMAGE(active_dc, dc_w, dc_h));
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
	imgtarget = pbuf ? pbuf : img_page[active_page].get();
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


EGEApplication&
FetchEGEApplication(int gdriver_n, int* gmode)
{
	static EGEApplication app(gdriver_n, gmode);

	return app;
}

_pages&
get_pages()
{
	static std::unique_ptr<_pages> p(new _pages());

	return *p;
}

} // namespace ege;

