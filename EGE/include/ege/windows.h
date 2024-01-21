#ifndef Inc_ege_windows_h_
#define Inc_ege_windows_h_

#include "ege/colorbase.h" // for Size;
#include "ege/base.h" // for BitmapPtr;
#include "ege/env.h"
#include <windef.h> // for ::HBITMAP, ::HANDLE, ::HWND, ::HINSTANCE;
// NOTE: Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97362.
#undef __deref
#include <wingdi.h> // for PS_*;
#include <winuser.h> // for WM_MOUSEWHEEL;
#if YEGE_Use_YSLib
#	if YCL_Win32
#		include <YCLib/YModules.h>
#		include YFM_Win32_YCLib_NLS // for platform_ex::MBCSToWCS,
#	else
#		error "Win32 required for YSLib."
#	endif
#else
#	include <string> // for std::wstring;
#	include <winnls.h> // for CP_ACP;
#endif

#ifndef WM_MOUSEWHEEL
#	define WM_MOUSEWHEEL                   0x020A
#endif

namespace ege
{

#if YEGE_Use_YSLib
using platform_ex::ScreenBuffer;
#else
class EGEAPI ScreenBuffer
{
private:
	Size size;

protected:
	BitmapPtr pBuffer;
	::HBITMAP hBitmap;

public:
	ScreenBuffer(const Size&);
	ScreenBuffer(ScreenBuffer&&) ynothrow;
	~ScreenBuffer();

	ScreenBuffer&
	operator=(ScreenBuffer&&);

	DefGetter(const ynothrow, BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const ynothrow, ::HBITMAP, NativeHandle, hBitmap)
	DefGetter(const ynothrow, const Size&, Size, size)

	void
	Resize(const Size&);

	void
	UpdateFrom(ConstBitmapPtr) ynothrow;

	void
	swap(ScreenBuffer&) ynothrow;
};

inline DefSwap(ynothrow, ScreenBuffer)

#endif

enum line_styles /* Line styles for get/setlinestyle */
{
	SOLID_LINE      = PS_SOLID,
	CENTER_LINE     = PS_DASH,
	DOTTED_LINE     = PS_DOT,
	DASHED_LINE     = PS_DASHDOT,
	NULL_LINE       = PS_NULL,
	USERBIT_LINE    = PS_USERSTYLE,   /* User defined line style */
};

struct msg_createwindow
{
	::HANDLE hEvent;
	::HWND hwnd;
	const wchar_t* classname;
	unsigned long style;
	unsigned long exstyle;
	unsigned id;
	void* param;
};

EGEAPI void
setinitmode(int mode = INIT_DEFAULT, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT); //设置初始化模式，mode=0为普通，1为无边框窗口，xy是初始窗口坐标

EGEAPI void
setcaption(const char*  caption);
EGEAPI void
setcaption(const wchar_t* caption);

EGEAPI ::HWND
getHWnd();         // 获取绘图窗口句柄

EGEAPI ::HINSTANCE
getHInstance();


#if YEGE_Use_YSLib
using platform_ex::MBCSToWCS;
#else
YB_ATTR_nodiscard EGEAPI YB_NONNULL(1) std::wstring
MBCSToWCS(const char*, unsigned = CP_ACP);
#endif

}

#endif

