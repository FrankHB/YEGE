#include "ege/proc.h"
#include "ege/base.h"
#include "global.h"

namespace ege
{

int
SetCloseHandler(LPCALLBACK_PROC func)
{
	get_global_state().callback_close = func;
	return grOk;
}

} // namespace ege;

