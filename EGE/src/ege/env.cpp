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
initgraph(int Width, int Height, int Flag)
{
	int g = TRUECOLORSIZE, m = (Width) | (Height << 16);

	if(graph_setting._g_initcall == 0)
		setinitmode(Flag);
	initgraph(&g, &m, (char*)"");
	//using flag;
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

