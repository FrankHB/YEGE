#ifndef Inc_ege_proc_h_
#define Inc_ege_proc_h_

#include "ege/def.h"

namespace ege
{

typedef void CALLBACK_PROC();
typedef int __stdcall MSG_KEY_PROC(void*, unsigned, int);
typedef int __stdcall MSG_MOUSE_PROC(void*, unsigned, int, int, int);

EGEAPI int
SetCloseHandler(CALLBACK_PROC* func);

}

#endif

