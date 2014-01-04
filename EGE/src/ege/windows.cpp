﻿#include "ege/windows.h"
#include "global.h"

namespace ege
{

void
setinitmode(int mode, int x, int y)
{
	_set_initmode(mode, x, y);
}

void
setcaption(const char* caption)
{
	::SetWindowTextA(getHWnd(), caption);
}
void
setcaption(const wchar_t* caption)
{
	::SetWindowTextW(getHWnd(), caption);
}

int
attachHWND(::HWND hWnd)
{
	get_global_state()._g_attach_hwnd = hWnd;
	return 0;
}

::HWND
getHWnd()
{
	return get_global_state().hwnd;
}

::HINSTANCE
getHInstance()
{
	return get_global_state().instance;
}

} // namespace ege;

