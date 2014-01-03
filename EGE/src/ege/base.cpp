#include "ege/base.h"
#include "global.h"

namespace ege
{

void
setrendermode(rendermode_e mode)
{
	if(mode == RENDER_MANUAL)
		get_global_state()._render_manual();
	else
		get_global_state()._render_normal();
}

}

