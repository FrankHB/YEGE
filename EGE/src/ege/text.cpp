#include "ege/text.h"
#include "global.h"
#include "image.h"
#include "ege/time.h"
#include "ege/gdi.h"
#include "ege/sys_edit.h"
#include <cstdio>
#include <cstdarg>

namespace ege
{

/* private function */
static
unsigned int
private_gettextmode(IMAGE* img)
{
	::UINT fMode = TA_NOUPDATECP; //TA_UPDATECP;
	if(img->m_texttype.horiz == RIGHT_TEXT)
	{
		fMode |= TA_RIGHT;
	}
	else if(img->m_texttype.horiz == CENTER_TEXT)
	{
		fMode |= TA_CENTER;
	}
	else
	{
		fMode |= TA_LEFT;
	}
	if(img->m_texttype.vert == BOTTOM_TEXT)
	{
		fMode |= TA_BOTTOM;
	}
	else
	{
		fMode |= TA_TOP;
	}
	return fMode;
}

/* private function */
static
void
private_textout(IMAGE* img, const char* textstring, int x, int y, int horiz, int vert)
{
	if(horiz >= 0 && vert >= 0)
	{
		::UINT fMode = TA_NOUPDATECP; //TA_UPDATECP;
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
		::SetTextAlign(img->m_hDC, fMode);
	}
	else
	{
		::SetTextAlign(img->m_hDC, private_gettextmode(img));
	}
	if(textstring)
	{
		if(img->m_texttype.vert == CENTER_TEXT)
			y -= textheight(textstring, img) / 2;
		TextOutA(img->m_hDC, x, y, textstring, (int)strlen(textstring));
	}
}

/* private function */
static
void
private_textout(IMAGE* img, const wchar_t* textstring, int x, int y, int horiz, int vert)
{
	if(horiz >= 0 && vert >= 0)
	{
		::UINT fMode = TA_NOUPDATECP; //TA_UPDATECP;
		img->m_texttype.horiz = horiz;
		img->m_texttype.vert = vert;
		if(img->m_texttype.horiz == RIGHT_TEXT)
		{
			fMode |= TA_RIGHT;
		}
		else if(img->m_texttype.horiz == CENTER_TEXT)
		{
			fMode |= TA_CENTER;
		}
		else
		{
			fMode |= TA_LEFT;
		}
		if(img->m_texttype.vert == BOTTOM_TEXT)
		{
			fMode |= TA_BOTTOM;
		}
		else
		{
			fMode |= TA_TOP;
		}
		::SetTextAlign(img->m_hDC, fMode);
	}
	else
	{
		::SetTextAlign(img->m_hDC, private_gettextmode(img));
	}
	if(textstring)
	{
		if(img->m_texttype.vert == CENTER_TEXT)
		{
			y -= textheight(textstring, img) / 2;
		}
		TextOutW(img->m_hDC, x, y, textstring, (int)::lstrlenW(textstring));
	}
}

void
outtext(const char* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		::POINT pt;
		::GetCurrentPositionEx(img->m_hDC, &pt);
		private_textout(img, textstring, pt.x, pt.y, -1, -1);
	}
}

void
outtext(const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		::POINT pt;
		::GetCurrentPositionEx(img->m_hDC, &pt);
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
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		private_textout(img, textstring, x, y, -1, -1);
	}
}

void
outtextxy(int x, int y, const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		private_textout(img, textstring, x, y, -1, -1);
	}
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
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		unsigned int fmode = private_gettextmode(img);
		::RECT rect{x, y, x + w, y + h};
		::DrawTextA(img->m_hDC, textstring, -1, &rect, fmode | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_EXPANDTABS);
	}

}

void
outtextrect(int x, int y, int w, int h, const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img)
	{
		unsigned int fmode = private_gettextmode(img);
		::RECT rect{x, y, x + w, y + h};
		::DrawTextW(img->m_hDC, textstring, -1, &rect, fmode | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL | DT_EXPANDTABS);
	}

}

void
xyprintf(int x, int y, const char*  fmt, ...)
{
	const std::unique_ptr<char[]> buf(new char[8 << 12]);
	const auto buff(&buf[0]);

	get_global_state();
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

	get_global_state();
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

	get_global_state();
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

	get_global_state();
	va_list v;
	va_start(v, fmt);
	std::vswprintf(buff, fmt, v);
	outtextrect(x, y, w, h, buff);
	va_end(v);
}

int
textwidth(const char* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32A(img->m_hDC, textstring, (int)strlen(textstring), &sz);
		return sz.cx;
	}
	return 0;
}

int
textwidth(const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32W(img->m_hDC, textstring, (int)::lstrlenW(textstring), &sz);
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
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32A(img->m_hDC, textstring, (int)strlen(textstring), &sz);
		return sz.cy;
	}
	return 0;
}

int
textheight(const wchar_t* textstring, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		SIZE sz;
		GetTextExtentPoint32W(img->m_hDC, textstring, (int)::lstrlenW(textstring), &sz);
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
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
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
	BYTE fbCharSet,
	BYTE fbOutPrecision,
	BYTE fbClipPrecision,
	BYTE fbQuality,
	BYTE fbPitchAndFamily,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTA lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, fbCharSet,
			fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
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
	BYTE fbCharSet,
	BYTE fbOutPrecision,
	BYTE fbClipPrecision,
	BYTE fbQuality,
	BYTE fbPitchAndFamily,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTW lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, fbCharSet,
			fbOutPrecision, fbClipPrecision, fbQuality, fbPitchAndFamily, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
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
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTA lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
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
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTW lf{nHeight, nWidth, nEscapement, nOrientation, nWeight,
			bItalic != 0, bUnderline != 0, bStrikeOut != 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
}

void
setfont(int nHeight, int nWidth, const char* lpszFace, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTA lf{nHeight, nWidth, 0, 0, FW_DONTCARE, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, 0};

		::lstrcpyA(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectA(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
}

void
setfont(int nHeight, int nWidth, const wchar_t* lpszFace, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(img)
	{
		::LOGFONTW lf{nHeight, nWidth, 0, 0, FW_DONTCARE, 0, 0, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, 0};

		::lstrcpyW(lf.lfFaceName, lpszFace);
		::HFONT hfont = ::CreateFontIndirectW(&lf);
		::DeleteObject(::SelectObject(img->m_hDC, hfont));
	}
}

void
setfont(const ::LOGFONTA* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::DeleteObject(::SelectObject(img->m_hDC, ::CreateFontIndirectA(font)));
}

void
setfont(const ::LOGFONTW* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::DeleteObject(::SelectObject(img->m_hDC, ::CreateFontIndirectW(font)));
}

void
getfont(::LOGFONTA* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::GetObjectA((::HFONT)::GetCurrentObject(img->m_hDC, OBJ_FONT),
			sizeof(::LOGFONTA), font);
}

void
getfont(::LOGFONTW* font, IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);

	if(img)
		::GetObjectW((::HFONT)::GetCurrentObject(img->m_hDC, OBJ_FONT),
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
	const auto _buf(static_cast<wchar_t*>(operator new(len * 2)));
	wchar_t _title[256], _text[256];

	::MultiByteToWideChar(CP_ACP, 0, title, -1, _title, 256);
	::MultiByteToWideChar(CP_ACP, 0,  text, -1,  _text, 256);
	buf[0] = 0;

	int ret = inputbox_getline(_title, _text, _buf, len);

	if(ret)
		::WideCharToMultiByte(CP_ACP, 0, _buf, -1, buf, len, {}, {});
	operator delete(_buf);
	return ret;
}

int
inputbox_getline(const wchar_t* title, const wchar_t* text, wchar_t* buf, int len)
{
	auto pg = &get_global_state();
	IMAGE bg;
	IMAGE window;
	int w = 400, h = 300, x = (getwidth() - w) / 2, y = (getheight() - h) / 2;
	int ret = 0;

	bg.getimage(0, 0, getwidth(), getheight());
	window.createimage(w, h);
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
			} while(kbmsg());
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
				setcolor(HSLtoRGB(240.0f, 1.0f,
					0.5f + float(dy / 24.0 * 0.3)), &window);
				line(1, dy, w - 1, dy, &window);
			}
			setcolor(0xFFFFFF, &window);
			setbkmode(TRANSPARENT, &window);
			setfont(18, 0, L"Tahoma", &window);
			outtextxy(3, 3, title, &window);
			setcolor(0x0, &window);

			::RECT rect{30, 32, w - 30, 128 - 3};

			::DrawTextW(window.m_hDC, text, -1, &rect, DT_NOPREFIX | DT_LEFT
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
	pg->_getflush();
	return ret;
}

} // namespace ege;
