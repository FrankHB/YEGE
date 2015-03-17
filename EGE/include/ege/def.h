#ifndef Inc_ege_def_h_
#define Inc_ege_def_h_

//#include <ydef.h>
#include <cassert>
#include <cwchar>

#define yconstfn constexpr
#define yconstexpr constexpr
#define ynothrow noexcept

#define yconstraint assert
#define yassume assert

#define yunused(...) static_cast<void>(__VA_ARGS__)

#define EGEAPI

#endif

