#include "ege/env.h"
#include "ege/base.h"
#include "ege/windows.h"
#include "global.h"


namespace ege
{

void
initgraph(int* gdriver, int* gmode, char*)
{
	graph_setting._init_graph_x(gdriver, gmode);
}
void
initgraph(int width, int height, int flag)
{
	int g(TRUECOLORSIZE), m((width) | (height << 16));

	if(graph_setting._g_initcall == 0)
			setinitmode(flag);
	initgraph(&g, &m, nullptr);
}

void
closegraph()
{
	::ShowWindow(graph_setting.hwnd, SW_HIDE);
}

bool
is_run()
{
	return graph_setting._is_run();
}

} // namespace ege;

