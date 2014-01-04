#include "global.h"
#include "ege/base.h"
#include "ege/input.h"


namespace ege
{

int
kbmsg()
{
	return get_global_state().exit_window ? int(grNoInitGraph)
		: int(get_global_state()._peekallkey(1));
}

key_msg
getkey()
{
	return get_global_state()._getkey();
}

int
keystate(int key)
{
	return get_global_state()._keystate(key);
}

void
flushkey()
{
	get_global_state()._flushkey();
}

int
getch()
{
	return get_global_state()._getch_ex(0);
}

int
kbhit()
{
	return get_global_state()._kbhit_ex(0);
}


int
mousemsg()
{
	if(get_global_state().exit_window)
		return 0;

	EGEMSG msg(get_global_state()._peekmouse());

	return bool(msg.hwnd);
}

mouse_msg
getmouse()
{
	return get_global_state()._getmouse();
}

void
flushmouse()
{
	get_global_state()._flushmouse();
}

bool
showmouse(bool bShow)
{
	return get_global_state()._show_mouse(bShow);
}


int
mousepos(int* x, int* y)
{
	*x = get_global_state().mouse_last_x;
	*y = get_global_state().mouse_last_y;
	return 0;
}

} // namespace ege;

