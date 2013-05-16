#include "ege/proc.h"
#include "ege/base.h"
#include "global.h"

namespace ege
{

int
SetCloseHandler(LPCALLBACK_PROC func)
{
	graph_setting.callback_close = func;
	return grOk;
}

} // namespace ege;

