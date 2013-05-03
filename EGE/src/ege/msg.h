#ifndef Inc_ege_msg_h_
#define Inc_ege_msg_h_

#include "ege/def.h"

namespace ege
{

struct EGEMSG
{
	::HWND        hwnd;
	::UINT        message;
	::WPARAM      wParam;
	::LPARAM      lParam;
	::DWORD       time;
	::UINT        mousekey;
	::UINT        flag;
};

} // namespace ege

#endif

