#include "ege/windows.h"
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

::HWND
getHWnd()
{
	return get_global_state()._get_hwnd();
}

::HINSTANCE
getHInstance()
{
	return _graph_setting::get_instance();
}

} // namespace ege;

