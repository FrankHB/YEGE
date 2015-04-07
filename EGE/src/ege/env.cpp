#include "ege/env.h"
#include "ege/base.h"
#include "ege/windows.h"
#include "global.h"
#include "head.h"

namespace ege
{

void
initgraph(size_t width, size_t height, int flag)
{
	if(!_g_initcall)
		setinitmode(flag);
	FetchEGEApplication(SDst(width), SDst(height))._init_graph_x();
}

void
closegraph()
{
	FetchEGEApplication()._uninit();
}

bool
is_run()
{
	return FetchEGEApplication()._is_run();
}

} // namespace ege;

