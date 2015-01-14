#include "global.h"
#include "ege/base.h"
#include "ege/input.h"
#include "head.h"

namespace ege
{

int
kbmsg()
{
	return FetchEGEApplication()._kbmsg();
}

key_msg
getkey()
{
	return FetchEGEApplication()._getkey();
}

int
keystate(int key)
{
	return FetchEGEApplication()._keystate(key);
}

void
flushkey()
{
	FetchEGEApplication()._flushkey();
}

int
getch()
{
	return FetchEGEApplication()._getch();
}

int
kbhit()
{
	return FetchEGEApplication()._kbhit();
}


bool
mousemsg()
{
	return FetchEGEApplication()._mousemsg();
}

mouse_msg
getmouse()
{
	return FetchEGEApplication()._getmouse();
}

void
flushmouse()
{
	FetchEGEApplication()._flushmouse();
}

bool
showmouse(bool bShow)
{
	return FetchEGEApplication()._show_mouse(bShow);
}


int
mousepos(int* x, int* y)
{
	yunseq(Deref(x) = FetchEGEApplication().mouse_last_x,
		Deref(y) = FetchEGEApplication().mouse_last_y);
	return 0;
}

} // namespace ege;

