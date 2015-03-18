#include "ege/env.h"
#include "ege/base.h"
#include "ege/windows.h"
#include "global.h"


namespace ege
{

void
initgraph(int* gdriver, int* gmode, char*)
{
	assert(gdriver);

	FetchEGEApplication(*gdriver, gmode)._init_graph_x();
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
	::ShowWindow(FetchEGEApplication()._get_hwnd(), SW_HIDE);
}

bool
is_run()
{
	return FetchEGEApplication()._is_run();
}

} // namespace ege;

