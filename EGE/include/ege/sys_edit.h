#ifndef Inc_ege_sys_edit_h_
#define Inc_ege_sys_edit_h_

#include "ege/e_ctl.h"

namespace ege
{

class sys_edit : public egeControlBase
{
public:
	CTL_PREINIT(sys_edit, egeControlBase)
	{
		(void)inheritlevel;
		// do sth. before sub objects' construct function call
	} CTL_PREINITEND;
	sys_edit(CTL_DEFPARAM) : CTL_INITBASE(egeControlBase)
	{
		CTL_INIT; // must be the first linef
		directdraw(true);
		m_hwnd = nullptr;
	}
	~sys_edit()
	{
		destory();
	}
	int create(bool multiline = false, int = 2)
	{
		if(m_hwnd)
		{
			destory();
		}
		msg_createwindow msg{nullptr, nullptr, nullptr, 0, 0, 0, nullptr};
		msg.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
		msg.classname = L"EDIT";
		msg.id = egeControlBase::allocId();
		msg.style = WS_CHILD | WS_BORDER | ES_LEFT | ES_WANTRETURN;
		msg.style |= multiline ? ES_MULTILINE | WS_VSCROLL : ES_AUTOHSCROLL;
		msg.exstyle = WS_EX_CLIENTEDGE;// | WS_EX_STATICEDGE;
		msg.param = this;

		::PostMessageW(getHWnd(), WM_USER + 1, 1, (::LPARAM)&msg);
		::WaitForSingleObject(msg.hEvent, INFINITE);

		m_hwnd = msg.hwnd;
		m_hFont     = nullptr;
		m_hBrush    = nullptr;
		m_color     = 0x0;
		m_bgcolor   = 0xFFFFFF;

		::SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, (::LONG_PTR)this);
		m_callback = ::GetWindowLongPtrW(m_hwnd, GWLP_WNDPROC);
		::SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, (::LONG_PTR)getProcfunc());
		{
			char fontname[]{'\xcb', '\xce', '\xcc', '\xe5', 0, 0};
			setfont(12, 6, fontname);
		}
		visable(false);

		::CloseHandle(msg.hEvent);

		return 0;
	}
	int destory()
	{
		if(m_hwnd)
		{
			msg_createwindow msg{nullptr, nullptr, nullptr, 0, 0, 0, nullptr};
			msg.hwnd = m_hwnd;
			msg.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
			::SendMessage(m_hwnd, WM_SETFONT, 0, 0);
			::DeleteObject(m_hFont);
			::PostMessageW(getHWnd(), WM_USER + 1, 0, (::LPARAM)&msg);
			::WaitForSingleObject(msg.hEvent, INFINITE);
			::CloseHandle(msg.hEvent);
			if(m_hBrush) ::DeleteObject(m_hBrush);
			m_hwnd = nullptr;
			return 1;
		}
		return 0;
	}
	LRESULT onMessage(::UINT message, ::WPARAM wParam, ::LPARAM lParam)
	{
		if(message == WM_CTLCOLOREDIT)
		{
			::HDC dc = (::HDC)wParam;
			::HBRUSH br = ::CreateSolidBrush(RGBTOBGR(m_bgcolor));

			::SetBkColor(dc, RGBTOBGR(m_bgcolor));
			::SetTextColor(dc, RGBTOBGR(m_color));
			::DeleteObject(m_hBrush);
			m_hBrush = br;
			return ::LRESULT(br);
			//} else if(message == WM_SETFOCUS)
			//{
			//    int a = 0;
			//    int b = 1;
			//    return 0;
		}
		else
		{
			return ((LRESULT(CALLBACK*)(::HWND, ::UINT, ::WPARAM, ::LPARAM))m_callback)(m_hwnd, message, wParam, lParam);
		}
		//return 0;
	}
	void visable(bool bvisable)
	{
		egeControlBase::visable(bvisable);
		::ShowWindow(m_hwnd, (int)bvisable);
	}
	void setfont(int h, int w, const char* fontface)
	{
		{
			::LOGFONTA lf{h, w, 0, 0, FW_DONTCARE, 0, 0, 0, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH, 0};
			::lstrcpyA(lf.lfFaceName, fontface);

			::HFONT hFont = ::CreateFontIndirectA(&lf);

			if(hFont)
			{
				::SendMessageA(m_hwnd, WM_SETFONT, (::WPARAM)hFont, 0);
				::DeleteObject(m_hFont);
				m_hFont = hFont;
			}
		}
	}
	void setfont(int h, int w, const wchar_t* fontface)
	{
		{
			::LOGFONTW lf{h, w, 0, 0, FW_DONTCARE, 0, 0, 0, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH, 0};

			::lstrcpyW(lf.lfFaceName, fontface);

			::HFONT hFont = ::CreateFontIndirectW(&lf);

			if(hFont)
			{
				::SendMessageW(m_hwnd, WM_SETFONT, (::WPARAM)hFont, 0);
				::DeleteObject(m_hFont);
				m_hFont = hFont;
			}
		}
	}
	void move(int x, int y)
	{
		egeControlBase::move(x, y);
		::MoveWindow(m_hwnd, m_x, m_y, m_w, m_h, TRUE);
	}
	void size(int w, int h)
	{
		egeControlBase::size(w, h);
		::MoveWindow(m_hwnd, m_x, m_y, m_w, m_h, TRUE);
	}
	void settext(const char* text)
	{
		::SendMessageA(m_hwnd, WM_SETTEXT, 0, (::LPARAM)text);
	}
	void settext(const wchar_t* text)
	{
		::SendMessageW(m_hwnd, WM_SETTEXT, 0, (::LPARAM)text);
	}
	void gettext(int maxlen, char* text)
	{
		::SendMessageA(m_hwnd, WM_GETTEXT, (::WPARAM)maxlen, (::LPARAM)text);
	}
	void gettext(int maxlen, wchar_t* text)
	{
		::SendMessageW(m_hwnd, WM_GETTEXT, (::WPARAM)maxlen, (::LPARAM)text);
	}
	void setmaxlen(int maxlen)
	{
		::SendMessageW(m_hwnd, EM_LIMITTEXT, (::WPARAM)maxlen, 0);
	}
	void setcolor(color_t color)
	{
		m_color = color;
		::InvalidateRect(m_hwnd, nullptr, TRUE);
	}
	void setbgcolor(color_t bgcolor)
	{
		m_bgcolor = bgcolor;
		//::RedrawWindow(m_hwnd, nullptr, nullptr, RDW_INVALIDATE);
		::InvalidateRect(m_hwnd, nullptr, TRUE);
	}
	void setreadonly(bool readonly)
	{
		::SendMessageW(m_hwnd, EM_SETREADONLY, (::WPARAM)readonly, 0);
		::InvalidateRect(m_hwnd, nullptr, TRUE);
	}
	void setfocus()
	{
		::PostMessageW(getHWnd(), WM_USER + 2, 0, (::LPARAM)m_hwnd);
	}
protected:
	::HWND        m_hwnd;
	::HFONT       m_hFont;
	::HBRUSH      m_hBrush;
	color_t     m_color;
	color_t     m_bgcolor;
	::LONG_PTR    m_callback;
};

} // namespace ege

#endif

