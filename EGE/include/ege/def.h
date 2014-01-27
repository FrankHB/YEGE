#ifndef Inc_ege_def_h_
#define Inc_ege_def_h_

#include <ydef.h>

#ifdef YEGE_DLL
#	define EGEAPI __declspec(dllimport)
#elif defined(YEGE_BUILD_DLL)
#	define EGEAPI __declspec(dllexport)
#else
#	define EGEAPI
#endif

#endif

