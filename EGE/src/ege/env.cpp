#include "ege/env.h"
#include "ege/base.h"
#include "ege/windows.h"
#include "global.h"


namespace ege
{

void
initgraph(int* gdriver, int* gmode, char*)
{
	get_global_state()._init_graph_x(gdriver, gmode);
}
void
initgraph(int width, int height, int flag)
{
	int g(TRUECOLORSIZE), m((width) | (height << 16));

	if(!_g_initcall)
		setinitmode(flag);
	initgraph(&g, &m, {});
}

void
closegraph()
{
	::ShowWindow(get_global_state().hwnd, SW_HIDE);
}

bool
is_run()
{
	return get_global_state()._is_run();
}

} // namespace ege;

