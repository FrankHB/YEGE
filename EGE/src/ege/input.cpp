#include "global.h"
#include "ege/base.h"
#include "ege/input.h"

namespace ege
{

int
kbmsg()
{
	return get_global_state()._get_input(get_input_op::kbmsg);
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
	get_global_state()._flush_key_mouse(true);
}

int
getch()
{
	return get_global_state()._get_input(get_input_op::getch);
}

int
kbhit()
{
	return get_global_state()._get_input(get_input_op::kbhit);
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
	get_global_state()._flush_key_mouse({});
}

bool
showmouse(bool bShow)
{
	return get_global_state()._show_mouse(bShow);
}


int
mousepos(int* x, int* y)
{
	assert(x),
	assert(y);

	*x = get_global_state().mouse_last_x;
	*y = get_global_state().mouse_last_y;
	return 0;
}

} // namespace ege;

