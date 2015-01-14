#include "ege/proc.h"
#include "ege/base.h"
#include "global.h"

namespace ege
{

int
SetCloseHandler(CALLBACK_PROC* func)
{
	FetchEGEApplication().callback_close = func;
	return grOk;
}

} // namespace ege;

