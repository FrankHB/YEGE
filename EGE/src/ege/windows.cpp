#include "ege/windows.h"
#include "global.h"

namespace ege
{

void
setinitmode(int mode, int x, int y)
{
	graph_setting._set_initmode(mode, x, y);
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
	graph_setting._g_attach_hwnd = hWnd;
	return 0;
}

::HWND
getHWnd()
{
	return graph_setting.hwnd;
}

::HINSTANCE
getHInstance()
{
	return graph_setting.instance;
}

} // namespace ege;

