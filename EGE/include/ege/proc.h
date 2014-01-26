#ifndef Inc_ege_proc_h_
#define Inc_ege_proc_h_

#include "ege/def.h"

namespace ege
{

typedef void CALLBACK_PROC();

EGEAPI int
SetCloseHandler(CALLBACK_PROC* func);

}

#endif

