#pragma once

#include <F:/Programing/YSLib/YBase/include/ydef.h>

#define _Cdecl
#define EGEAPI

#ifdef _GRAPH_LIB_BUILD_
#	include <cstdio>
#	define EGE_DEPRECATE(text)
#else
#	define EGE_DEPRECATE(text) __attribute__((deprecated))
#endif


