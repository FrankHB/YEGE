﻿#ifndef Inc_ege_msg_h_
#define Inc_ege_msg_h_

#include <windef.h> // for ::HWND, ::WPARAM, ::LPARAM.
// NOTE: Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97362.
#undef __deref

namespace ege
{

struct EGEMSG
{
	::HWND hwnd = {};
	unsigned message = 0;
	::WPARAM wParam = 0;
	::LPARAM lParam = 0;
	unsigned long time = 0;
	unsigned mousekey = 0;
	unsigned flag = 0;

	EGEMSG() = default;
	EGEMSG(::HWND h, unsigned m, ::WPARAM w, ::LPARAM l, unsigned long t,
		unsigned k, unsigned f)
		: hwnd(h), message(m), wParam(w), lParam(l), time(t), mousekey(k),
		flag(f)
	{}
};

} // namespace ege;

#endif

