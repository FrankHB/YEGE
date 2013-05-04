#include "ege/base.h"
#include "global.h"

namespace ege
{

void
setrendermode(rendermode_e mode)
{
	if(mode == RENDER_MANUAL)
		graph_setting._render_manual();
	else
		graph_setting._render_normal();
}

}

