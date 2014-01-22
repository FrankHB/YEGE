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
#include <mutex> // for std::once_flag, std::call_once;

#ifdef _WIN64
#define ARCH "x64"
#else
#define ARCH "x86"
#endif

#define TOSTRING_(x) #x
#define TOSTRING(x) TOSTRING_(x)
#define GCC_VER TEXT(TOSTRING(__GNUC__)) TEXT(".") TEXT(TOSTRING(__GNUC_MINOR__)) TEXT(".") TEXT(TOSTRING(__GNUC_PATCHLEVEL__))

#define EGE_TITLE TEXT("yEGE13.04 ") TEXT("GCC") GCC_VER TEXT(ARCH)

#define RENDER_TIMER_ID     916

namespace ege
{

int _g_initoption(INIT_DEFAULT);
bool _g_initcall;

int update_mark_count; //更新标记
bool timer_stop_mark;
bool skip_timer_mark;
egeControlBase* egectrl_root;
egeControlBase* egectrl_focus;


namespace
{

::DWORD g_windowstyle(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
	| WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_VISIBLE);
::DWORD g_windowexstyle(WS_EX_LEFT | WS_EX_LTRREADING);
int _g_windowpos_x(CW_USEDEFAULT), _g_windowpos_y(CW_USEDEFAULT);
bool init_finish;

CALLBACK ::BOOL
EnumResNameProc(::HMODULE hModule, ::LPCTSTR, ::LPTSTR lpszName,
	::LONG_PTR lParam)
{
	if(const auto hico = ::HICON(::LoadImage(hModule, lpszName, IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE)))
	{
		*((::HICON*)lParam) = hico;
		return {};
	}
	return true;
}

::DWORD WINAPI
messageloopthread(LPVOID lpParameter)
{
	auto& g(*reinterpret_cast<_graph_setting*>(lpParameter));
	const int nCmdShow(SW_SHOW);
	static ::WNDCLASSEX wcex;

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = getProcfunc();
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g.instance;
	wcex.hIcon = ::LoadIcon(nullptr, IDI_WINLOGO);
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (::HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = g.window_class_name;

	const auto load([&](::LPCTSTR rt){
		::HICON hico = nullptr;

		EnumResourceNames(g.instance, rt, EnumResNameProc,
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

	//执行应用程序初始化
	const auto dw(::GetSystemMetrics(SM_CXFRAME) * 2),
		dh(::GetSystemMetrics(SM_CYFRAME)
		+ ::GetSystemMetrics(SM_CYCAPTION) * 2);

	if(g._g_attach_hwnd)
	{
		::LONG_PTR style = ::GetWindowLongPtrW(g._g_attach_hwnd, GWL_STYLE);
		style |= WS_CHILDWINDOW | WS_CLIPCHILDREN;
		::SetWindowLongPtrW(g._g_attach_hwnd, GWL_STYLE, style);
	}
	g.hwnd = ::CreateWindowEx(g_windowexstyle, g.window_class_name,
		g.window_caption, g_windowstyle & ~WS_VISIBLE, _g_windowpos_x,
		_g_windowpos_y, g.dc_w + dw, g.dc_h + dh, g._g_attach_hwnd, nullptr,
		g.instance, nullptr);
	if(!g.hwnd)
		return 0xFFFFFFFF;
	if(g._g_attach_hwnd)
	{
		wchar_t name[64];

		std::swprintf(name, L"ege_%X",
			::DWORD(::DWORD_PTR(g._g_attach_hwnd)));
		if(::CreateEventW(nullptr, FALSE, TRUE, name))
			if(::GetLastError() == ERROR_ALREADY_EXISTS)
				::PostMessage(g.hwnd, WM_CLOSE, 0, 0);
	}
	::SetWindowLongPtrW(g.hwnd, GWLP_USERDATA, ::LONG_PTR(&g));
	g.exit_window = 0;
	::ShowWindow(g.hwnd, nCmdShow);
	if(g_windowexstyle & WS_EX_TOPMOST)
		::SetWindowPos(g.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE);

	//图形初始化
	auto hDC(::GetDC(g.hwnd));

	g.dc = hDC;
	g.window_dc = hDC;
	g.img_timer_update = newimage();
	setactivepage(0);
	settarget(nullptr);
	setvisualpage(0);
	window_setviewport(0, 0, g.dc_w, g.dc_h);
	//::ReleaseDC(hwnd, hDC);
	g.mouse_show = {};
	g.use_force_exit = !(_g_initoption & INIT_NOFORCEEXIT);
	g.close_manually = true;
	skip_timer_mark = {};
	::SetTimer(g.hwnd, RENDER_TIMER_ID, 50, nullptr);
	init_finish = true;

	MSG msg;

	while(!g.exit_window)
		if(::GetMessage(&msg, nullptr, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
			::Sleep(1);
	return 0;
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

_graph_setting::_graph_setting()
	: instance(::GetModuleHandle(nullptr))
{
	static std::once_flag init_flag;

	std::call_once(init_flag, []{
		static ::ULONG_PTR g_gdiplusToken;
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;

		Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
	});
}

bool
_graph_setting::_is_run() const
{
	return !(exit_window || exit_flag);
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
		dc_h = short(unsigned(gmode >> 16));
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

int
_graph_setting::_set_target(IMAGE* pbuf)
{
	imgtarget_set = pbuf;
	imgtarget = pbuf ? pbuf : img_page[active_page];
	return 0;
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
_graph_setting::_flushkey()
{
	EGEMSG msg;

	if(msgkey_queue.empty())
		_dealmessage(false);
	if(!msgkey_queue.empty())
		while(msgkey_queue.pop(msg))
			;
}

void
_graph_setting::_flushmouse()
{
	EGEMSG msg;

	if(msgmouse_queue.empty())
		_dealmessage(false);
	if(!msgmouse_queue.empty())
		while(msgmouse_queue.pop(msg))
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
					msg = msgkey_queue.last();
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

	if(msgkey_queue.empty())
		_dealmessage(false);
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

	if(exit_window)
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
	} while(!exit_window && !exit_flag && _waitdealmessage());
	return mmsg;
}

#if 0
EGEMSG
_graph_setting::_getmouse_p()
{
	auto msg = EGEMSG();

	msgmouse_queue.pop(msg);
	return msg;
}
#endif

void
_graph_setting::_init_graph_x(int* gdriver, int* gmode)
{
	static std::once_flag init_flag;

	_set_mode(*gdriver, *gmode);
	std::call_once(init_flag, [this]{
	//	::SECURITY_ATTRIBUTES sa{};
		::DWORD pid;

		init_finish = false;
		threadui_handle = ::CreateThread(nullptr, 0, messageloopthread,
			this, CREATE_SUSPENDED, &pid);
		::ResumeThread(threadui_handle);
		while(!init_finish)
			::Sleep(1);
		::UpdateWindow(hwnd);
		//初始化鼠标位置数据
		mouse_last_x = dc_w / 2;
		mouse_last_y = dc_h / 2;

		static egeControlBase _egeControlBase;

		if(_g_initoption & INIT_RENDERMANUAL)
			setrendermode(RENDER_MANUAL);
		mouse_show = true;
	});
	exit_flag = {};
	exit_window = 0;
	for(int page = 0; page < BITMAP_PAGE_SIZE; ++page)
		if(img_page[page])
			img_page[page]->createimage(dc_w, dc_h);
	::ShowWindow(hwnd, SW_SHOW);
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

		COMPOSITIONFORM cpf{0, ::POINT(), ::RECT()};

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
		::POINT pt;

		::GetCursorPos(&pt);
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
		_dealmessage(false);
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
	msgmouse_queue.push(EGEMSG{hwnd, message, wparam, lparam, ::GetTickCount(),
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

void
_graph_setting::_render_normal()
{
	delay_ms(0);
	::SetTimer(hwnd, RENDER_TIMER_ID, 0, nullptr);
	skip_timer_mark = false;
	lock_window = false;
}

void
_graph_setting::_render_manual()
{
	if(!lock_window)
	{
		::KillTimer(hwnd, RENDER_TIMER_ID);
		timer_stop_mark = true;
		::PostMessageW(hwnd, WM_TIMER, RENDER_TIMER_ID, 0);
		lock_window = true;
		while(timer_stop_mark)
			::Sleep(1);
	}
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

	msgkey_queue.process(bind(&_graph_setting::_process_ui_msg, this, _1));
	msgmouse_queue.process(bind(&_graph_setting::_process_ui_msg, this, _1));
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
_graph_setting::_window_create(msg_createwindow& msg)
{
	msg.hwnd = ::CreateWindowExW(msg.exstyle, msg.classname, nullptr,
		msg.style, 0, 0, 0, 0, getHWnd(), (HMENU)msg.id, getHInstance(),
		nullptr);
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


_graph_setting&
get_global_state()
{
	static std::unique_ptr<_graph_setting> p(new _graph_setting());

	return *p;
}

} // namespace ege;

