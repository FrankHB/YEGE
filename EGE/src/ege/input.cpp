#include "global.h"
#include "ege/base.h"
#include "ege/input.h"
#include "head.h"

namespace ege
{

int
kbmsg()
{
	return get_global_state()._kbmsg();
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
	return get_global_state()._getch();
}

int
kbhit()
{
	return get_global_state()._kbhit();
}


bool
mousemsg()
{
	return get_global_state()._mousemsg();
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
	yunseq(Deref(x) = get_global_state().mouse_last_x,
		Deref(y) = get_global_state().mouse_last_y);
	return 0;
}

} // namespace ege;

