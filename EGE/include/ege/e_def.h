#pragma once

//#include <F:/Programing/YSLib/YBase/include/ydef.h>
#include <cwchar>
#define yconstfn constexpr

#define _Cdecl
#define EGEAPI

#ifdef _GRAPH_LIB_BUILD_
#	include <cstdio>
#	define EGE_DEPRECATE(text)
#else
#	define EGE_DEPRECATE(text) __attribute__((deprecated))
#endif


