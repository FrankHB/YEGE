#ifndef Inc_ege_msg_h_
#define Inc_ege_msg_h_

#include <windef.h>

namespace ege
{

struct EGEMSG
{
	::HWND hwnd = {};
	::UINT message = 0;
	::WPARAM wParam = 0;
	::LPARAM lParam = 0;
	::DWORD time = 0;
	::UINT mousekey = 0;
	::UINT flag = 0;

	EGEMSG() = default;
	EGEMSG(::HWND h, ::UINT m, ::WPARAM w, ::LPARAM l, ::DWORD t, ::UINT k,
		::UINT f)
		: hwnd(h), message(m), wParam(w), lParam(l), time(t), mousekey(k),
		flag(f)
	{}
};

} // namespace ege;

#endif

