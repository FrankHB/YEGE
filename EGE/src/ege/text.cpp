#include "ege/text.h"
#include "global.h"
#include "image.h"
#include "ege/time.h"
#include "ege/gdi.h"
#include "ege/sys_edit.h"
#include <cstdio>
#include <cstdarg>
#include <cstring> // for std::strlen;

namespace ege
{

namespace
{

::LRESULT CALLBACK
sys_edit_proc(::HWND hWnd, ::UINT message, ::WPARAM wParam, ::LPARAM lParam)
{
	auto& app(FetchEGEApplication());
	//int wmId, wmEvent;
	const auto pg_w(reinterpret_cast<EGEApplication*>(
		::GetWindowLongPtrW(hWnd, GWLP_USERDATA)));

	if(!pg_w)
		return ::DefWindowProcW(hWnd, message, wParam, lParam);
	switch(message)
	{
	case WM_PAINT:
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_ERASEBKGND:
	case WM_SETCURSOR:
		break;
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
	case WM_USER + 1:
		EGEApplication::_window_handle_wm_user_1(lParam, wParam);
		break;
	case WM_USER + 2:
		::SetFocus(::HWND(lParam));
		break;
	case WM_CTLCOLOREDIT:
		return (reinterpret_cast<egeControlBase*>(::GetWindowLongPtrW(
			::HWND(lParam), GWLP_USERDATA)))
			->onMessage(message, wParam, lParam);
	default:
		break;
	}
	return (reinterpret_cast<egeControlBase*>(pg_w))
		->onMessage(message, wParam, lParam);
}

unsigned int
private_gettextmode(IMAGE* img)
{
	unsigned fMode(TA_NOUPDATECP); //TA_UPDATECP;

	if(img->m_texttype.horiz == RIGHT_TEXT)
		fMode |= TA_RIGHT;
	else if(img->m_texttype.horiz == CENTER_TEXT)
		fMode |= TA_CENTER;
	else
		fMode |= TA_LEFT;
	fMode |= img->m_texttype.vert == BOTTOM_TEXT ? TA_BOTTOM : TA_TOP;
	return fMode;
}

void
private_textout(IMAGE* img, const char* textstring, int x, int y, int horiz,
	int vert)
{
	if(horiz >= 0 && vert >= 0)
	{
		unsigned fMode = TA_NOUPDATECP; //TA_UPDATECP;

		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
		if(img->m_texttype.horiz == RIGHT_TEXT)
			fMode |= TA_RIGHT;
		else if(img->m_texttype.horiz == CENTER_TEXT)
			fMode |= TA_CENTER;
		else
			fMode |= TA_LEFT;
		if(img->m_texttype.vert == BOTTOM_TEXT)
			fMode |= TA_BOTTOM;
		else
			fMode |= TA_TOP;
		::SetTextAlign(img->getdc(), fMode);
	}
	else
		::SetTextAlign(img->getdc(), private_gettextmode(img));
	if(textstring)
	{
		if(img->m_texttype.vert == CENTER_TEXT)
			y -= textheight(textstring, img) / 2;
		TextOutA(img->getdc(), x, y, textstring, std::strlen(textstring));
	}
}

void
private_textout(IMAGE* img, const wchar_t* textstring, int x, int y, int horiz,
	int vert)
{
	if(horiz >= 0 && vert >= 0)
	{
		unsigned fMode = TA_NOUPDATECP; //TA_UPDATECP;

		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
		if(img->m_texttype.horiz == RIGHT_TEXT)
			fMode |= TA_RIGHT;
		else if(img->m_texttype.horiz == CENTER_TEXT)
			fMode |= TA_CENTER;
		else
			fMode |= TA_LEFT;
		fMode |= img->m_texttype.vert == BOTTOM_TEXT ?  TA_BOTTOM : TA_TOP;
		::SetTextAlign(img->getdc(), fMode);
	}
	else
		::SetTextAlign(img->getdc(), private_gettextmode(img));
	if(textstring)
	{
		if(img->m_texttype.vert == CENTER_TEXT)
		{
			y -= textheight(textstring, img) / 2;
		}
		TextOutW(img->getdc(), x, y, textstring, ::lstrlenW(textstring));
	}
}

} // unnamed namespace;

::WNDPROC
sys_edit::GetSysEditWndProc()
{
	return sys_edit_proc;
}

void
outtext(const char* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		::POINT pt;
		::GetCurrentPositionEx(img->getdc(), &pt);

		private_textout(img, textstring, pt.x, pt.y, -1, -1);
	}
}

void
outtext(const wchar_t* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		::POINT pt;
		::GetCurrentPositionEx(img->getdc(), &pt);

		private_textout(img, textstring, pt.x, pt.y, -1, -1);
	}
}

void
outtext(char c, IMAGE* pimg)
{
	char str[10]{c};

	outtext(str, pimg);
}

void
outtext(wchar_t c, IMAGE* pimg)
{
	wchar_t str[10]{c};

	outtext(str, pimg);
}

void
outtextxy(int x, int y, const char* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		private_textout(img, textstring, x, y, -1, -1);
}

void
outtextxy(int x, int y, const wchar_t* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
		private_textout(img, textstring, x, y, -1, -1);
}

void
outtextxy(int x, int y, char c, IMAGE* pimg)
{
	char str[10]{c};

	outtextxy(x, y, str, pimg);
}

void
outtextxy(int x, int y, wchar_t c, IMAGE* pimg)
{
	wchar_t str[10]{c};

	outtextxy(x, y, str, pimg);
}

void
outtextrect(int x, int y, int w, int h, const char*  textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		::RECT rect{x, y, x + w, y + h};

		::DrawTextA(img->getdc(), textstring, -1, &rect,
			private_gettextmode(img) | DT_NOPREFIX | DT_WORDBREAK
			| DT_EDITCONTROL | DT_EXPANDTABS);
	}

}

void
outtextrect(int x, int y, int w, int h, const wchar_t* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE(pimg))
	{
		::RECT rect{x, y, x + w, y + h};

		::DrawTextW(img->getdc(), textstring, -1, &rect,
			private_gettextmode(img) | DT_NOPREFIX | DT_WORDBREAK
			| DT_EDITCONTROL | DT_EXPANDTABS);
	}

}

void
xyprintf(int x, int y, const char*  fmt, ...)
{
	const std::unique_ptr<char[]> buf(new char[8 << 12]);
	const auto buff(&buf[0]);

	FetchEGEApplication();
	va_list v;
	va_start(v, fmt);
	std::vsprintf(buff, fmt, v);
	outtextxy(x, y, buff);
	va_end(v);
}

void
xyprintf(int x, int y, const wchar_t* fmt, ...)
{
	const std::unique_ptr<wchar_t[]> buf(new wchar_t[8 << 11]);
	const auto buff(&buf[0]);

	FetchEGEApplication();
	va_list v;
	va_start(v, fmt);
	std::vswprintf(buff, fmt, v);
	outtextxy(x, y, buff);
	va_end(v);
}

void
rectprintf(int x, int y, int w, int h, const char*  fmt, ...)
{
	const std::unique_ptr<char[]> buf(new char[8 << 12]);
	const auto buff(&buf[0]);

	FetchEGEApplication();

	va_list v;

	va_start(v, fmt);
	vsprintf(buff, fmt, v);
	outtextrect(x, y, w, h, buff);
	va_end(v);
}

void
rectprintf(int x, int y, int w, int h, const wchar_t* fmt, ...)
{
	const std::unique_ptr<wchar_t[]> buf(new wchar_t[8 << 11]);
	const auto buff(&buf[0]);

	FetchEGEApplication();

	va_list v;

	va_start(v, fmt);
	std::vswprintf(buff, fmt, v);
	outtextrect(x, y, w, h, buff);
	va_end(v);
}

int
textwidth(const char* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		SIZE sz;
		::GetTextExtentPoint32A(img->getdc(), textstring, std::strlen(textstring),
			&sz);
		return sz.cx;
	}
	return 0;
}

int
textwidth(const wchar_t* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		SIZE sz;
		::GetTextExtentPoint32W(img->getdc(), textstring,
			::lstrlenW(textstring), &sz);
		return sz.cx;
	}
	return 0;
}

int
textwidth(char c, IMAGE* pimg)
{
	char str[2]{c};

	return textwidth(str, pimg);
}

int
textwidth(wchar_t c, IMAGE* pimg)
{
	wchar_t str[2]{c};

	return textwidth(str, pimg);
}

int
textheight(const char* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		SIZE sz;

		::GetTextExtentPoint32A(img->getdc(), textstring,
			std::strlen(textstring), &sz);
		return sz.cy;
	}
	return 0;
}

int
textheight(const wchar_t* textstring, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		SIZE sz;

		::GetTextExtentPoint32W(img->getdc(), textstring,
			::lstrlenW(textstring), &sz);
		return sz.cy;
	}
	return 0;
}

int
textheight(char c, IMAGE* pimg)
{
	char str[2]{c};

	return textheight(str, pimg);
}

int
textheight(wchar_t c, IMAGE* pimg)
{
	wchar_t str[2]{c};

	return textheight(str, pimg);
}

void
settextjustify(int horiz, int vert, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
	}
}


void
setfont(
	int nHeight,
	int nWidth,
	const char* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	byte fbCharSet,
	byte fbOutPrecision,
	byte fbClipPrecision,
	byte fbQuality,
	byte fbPitchAndFamily,
	IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::LOGFONTA lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, fbCharSet,
			fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->getdc(), hfont));
	}
}

void
setfont(
	int nHeight,
	int nWidth,
	const wchar_t* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	byte fbCharSet,
	byte fbOutPrecision,
	byte fbClipPrecision,
	byte fbQuality,
	byte fbPitchAndFamily,
	IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::LOGFONTW lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, fbCharSet,
			fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->getdc(), hfont));
	}
}

void
setfont(
	int nHeight,
	int nWidth,
	const char* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::LOGFONTA lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->getdc(), hfont));
	}
}

void
setfont(
	int nHeight,
	int nWidth,
	const wchar_t* lpszFace,
	int nEscapement,
	int nOrientation,
	int nWeight,
	int bItalic,
	int bUnderline,
	int bStrikeOut,
	IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::LOGFONTW lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->getdc(), hfont));
	}
}

void
setfont(int nHeight, int nWidth, const char* lpszFace, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::LOGFONTA lf{nHeight, nWidth, 0, 0, FW_DONTCARE, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->getdc(), hfont));
	}
}

void
setfont(int nHeight, int nWidth, const wchar_t* lpszFace, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
	{
		::LOGFONTW lf{nHeight, nWidth, 0, 0, FW_DONTCARE, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->getdc(), hfont));
	}
}

void
setfont(const ::LOGFONTA* font, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		::DeleteObject(::SelectObject(img->getdc(),
			::CreateFontIndirectA(font)));
}

void
setfont(const ::LOGFONTW* font, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		::DeleteObject(::SelectObject(img->getdc(),
			::CreateFontIndirectW(font)));
}

void
getfont(::LOGFONTA* font, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		::GetObjectA(::HFONT(::GetCurrentObject(img->getdc(), OBJ_FONT)),
			sizeof(::LOGFONTA), font);
}

void
getfont(::LOGFONTW* font, IMAGE* pimg)
{
	if(const auto img = CONVERT_IMAGE_CONST(pimg))
		::GetObjectW(::HFONT(::GetCurrentObject(img->getdc(), OBJ_FONT)),
			sizeof(::LOGFONTA), font);
}


namespace
{

void
draw_frame(IMAGE* img, int l, int t, int r, int b, color_t lc, color_t dc)
{
	setcolor(lc, img);
	line(l, b, l, t, img);
	lineto(r, t, img);
	setcolor(dc, img);
	lineto(r, b, img);
	lineto(l, b, img);
}

}

int
inputbox_getline(const char* title, const char* text, char* buf, int len)
{
	yconstraint(buf);
#if YEGE_Use_YSLib
	using namespace platform_ex;
	unique_ptr<wchar_t[]> wbuf(new wchar_t[len * 2]);
#endif

	const auto _buf(make_unique<wchar_t[]>(len));
	wchar_t _title[256], _text[256];

	::MultiByteToWideChar(CP_ACP, 0, title, -1, _title, 256);
	::MultiByteToWideChar(CP_ACP, 0,  text, -1,  _text, 256);
	buf[0] = 0;

#if YEGE_Use_YSLib
	if(const int ret = inputbox_getline(MBCSToWCS(title).c_str(),
		MBCSToWCS(text).c_str(), wbuf.get(), len))
	{
		// XXX: Remove redundant copy.
		std::strcpy(buf, WCSToMBCS(wbuf.get()).c_str());
		return ret;
	}
	return 0;
#else
	const int ret(inputbox_getline(_title, _text, _buf.get(), len));

	if(ret)
		::WideCharToMultiByte(CP_ACP, 0, _buf.get(), -1, buf, len, {}, {});
	return ret;
#endif
}

int
inputbox_getline(const wchar_t* title, const wchar_t* text, wchar_t* buf,
	int len)
{
	int w = 400, h = 300, x = (getwidth() - w) / 2, y = (getheight() - h) / 2;
	int ret = 0;

	IMAGE bg;
	bg.getimage(get_pages().imgtarget, 0, 0, getwidth(), getheight());

	IMAGE window(w, h);
	buf[0] = 0;

	sys_edit edit(true);

	edit.create(true);
	edit.move(x + 30 + 1, y + 192 + 1);
	edit.size(w - (30 + 1) * 2, h - 40 - 192 - 2);
	edit.setmaxlen(len);
	edit.visable(true);
	edit.setfocus();
	for(int bInit = 0; is_run(); delay_fps(30))
	{
		if(bInit)
		{
			bool exit = {};
			do
			{
				key_msg msg = getkey();
				if(msg.key == key_enter && msg.msg == key_msg_up)
				{
					exit = true;
					break;
				}
			}while(kbmsg());
			if(exit)
				break;
		}
		putimage(0, 0, &bg);
		if(bInit == 0)
		{
			bInit = 1;
			setbkcolor(EGERGB(0x80, 0xA0, 0x80), &window);
			draw_frame(&window, 0, 0, w - 1, h - 1,
				EGERGB(0xA0, 0xC0, 0xA0), EGERGB(0x50, 0x70, 0x50));
			setfillcolor(EGERGB(0, 0, 0xA0), &window);
			for(int dy = 1; dy < 24; dy++)
			{
				setcolor(hsl2rgb(240.0f, 1.0f,
					0.5f + float(dy / 24.0 * 0.3)), &window);
				line(1, dy, w - 1, dy, &window);
			}
			setcolor(0xFFFFFF, &window);
			setbkmode(TRANSPARENT, &window);
			setfont(18, 0, L"Tahoma", &window);
			outtextxy(3, 3, title, &window);
			setcolor(0x0, &window);

			::RECT rect{30, 32, w - 30, 128 - 3};

			::DrawTextW(window.getdc(), text, -1, &rect, DT_NOPREFIX | DT_LEFT
				| DT_TOP | TA_NOUPDATECP | DT_WORDBREAK | DT_EDITCONTROL
				| DT_EXPANDTABS);
		}
		putimage(x, y, &window);
	}
	edit.gettext(len, buf);
	len = ::lstrlenW(buf);
	while(len > 0 && (buf[len - 1] == '\r' || buf[len - 1] == '\n'))
		buf[--len] = 0;
	ret = len;
	putimage(0, 0, &bg);
	FetchEGEApplication()._getflush();
	return ret;
}

} // namespace ege;
