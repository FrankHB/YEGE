#include "global.h"
#include "ege/base.h"
#include "ege/input.h"

namespace ege
{

int
kbmsg()
{
	return FetchEGEApplication()._get_input(get_input_op::kbmsg);
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
	FetchEGEApplication()._flush_key_mouse(true);
}

int
getch()
{
	return FetchEGEApplication()._get_input(get_input_op::getch);
}

int
kbhit()
{
	return FetchEGEApplication()._get_input(get_input_op::kbhit);
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
	FetchEGEApplication()._flush_key_mouse({});
}

bool
showmouse(bool bShow)
{
	return FetchEGEApplication()._show_mouse(bShow);
}


int
mousepos(int* x, int* y)
{
	assert(x),
	assert(y);

	*x = FetchEGEApplication().mouse_last_x;
	*y = FetchEGEApplication().mouse_last_y;
	return 0;
}

} // namespace ege;

