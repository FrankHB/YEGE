#include "global.h"
#include "ege/base.h"
#include "ege/input.h"


namespace ege
{

int
kbmsg()
{
	return graph_setting.exit_window ? int(grNoInitGraph)
		: int(graph_setting._peekallkey(1));
}

int
kbhit()
{
	return graph_setting._kbhit_ex(0);
}

int
getch()
{
	return graph_setting._getch_ex(0);
}

key_msg
getkey()
{
	return graph_setting._getkey();
}

void
flushkey()
{
	graph_setting._flushkey();
}

int
keystate(int key)
{
	return graph_setting._keystate(key);
}

void
flushmouse()
{
	graph_setting._flushmouse();
}

int
mousemsg()
{
	if(graph_setting.exit_window)
		return 0;

	EGEMSG msg(graph_setting._peekmouse());

	return bool(msg.hwnd);
}

mouse_msg
getmouse()
{
	return graph_setting._getmouse();
}

} // namespace ege;

