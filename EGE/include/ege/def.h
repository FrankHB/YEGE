#ifndef Inc_ege_def_h_
#define Inc_ege_def_h_

#ifndef YEGE_Use_YSLib
#	define YEGE_Use_YSLib false
#endif

#if YEGE_Use_YSLib
#	include <ystdex/cstddef.h> // for ystdex::byte, ystdex::octet, CHAR_BIT;

namespace ege
{

using ystdex::byte;
using ystdex::octet;

} // namespace ege;

#else
#	include <climits> // for CHAR_BIT;
#	include <cassert>
#	include <cwchar>
#	include <utility>

#define YPP_Empty
#define YPP_Comma ,
#define YPP_Concat(_x, _y) _x ## _y

#	if __GNUC__
#		define YB_EXPECT(_expr, _constant) (__builtin_expect(_expr, _constant))
#		define YB_LIKELY(_expr) (__builtin_expect(bool(_expr), 1))
#		define YB_UNLIKELY(_expr) (__builtin_expect(bool(_expr), 0))
#	else
#		define YB_EXPECT(_expr, _constant) (_expr)
#		define YB_LIKELY(_expr) (_expr)
#		define YB_UNLIKELY(_expr) (_expr)
#	endif

#	define yconstfn constexpr
#	define yconstexpr constexpr
#	define ynothrow noexcept

#	define yconstraint assert
#	define yassume assert

#	define yforward(_expr) std::forward<decltype(_expr)>(_expr)

#	define yunused(...) static_cast<void>(__VA_ARGS__)

namespace ege
{

using byte = unsigned char;

#if CHAR_BIT == 8
using octet = byte;
#	else
using octet = void;
#endif

template<typename _type, typename... _tParams>
yconstfn auto
unsequenced(_type&& arg, _tParams&&...) -> decltype(yforward(arg))
{
	return yforward(arg);
}

} // namespace ege;

#	define yunseq ege::unsequenced
#endif

#ifdef YEGE_DLL
#	define EGEAPI __declspec(dllimport)
#elif defined(YEGE_BUILD_DLL)
#	define EGEAPI __declspec(dllexport)
#else
#	define EGEAPI
#endif

#endif

