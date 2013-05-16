#ifndef Inc_ege_proc_h_
#define Inc_ege_proc_h_

#include "ege/def.h"

namespace ege
{

typedef void (CALLBACK_PROC)();
typedef int (__stdcall MSG_KEY_PROC)(void*, unsigned, int);
typedef int (__stdcall MSG_MOUSE_PROC)(void*, unsigned, int, int, int);
typedef CALLBACK_PROC* LPCALLBACK_PROC;
typedef MSG_KEY_PROC* LPMSG_KEY_PROC;
typedef MSG_MOUSE_PROC* LPMSG_MOUSE_PROC;

EGEAPI int
SetCloseHandler(LPCALLBACK_PROC func);

}

#endif

