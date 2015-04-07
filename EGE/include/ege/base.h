#ifndef Inc_ege_base_h_
#define Inc_ege_base_h_

#include "ege/colorbase.h"
#if !YEGE_Use_YSLib
#	include <cstddef>
#	include <cstdint>
#	include <type_traits>
#	include <memory>
#	include <limits>
#	include <tuple>
#endif

namespace ege
{

#if YEGE_Use_YSLib
using YSLib::size_t;
using YSLib::bad_weak_ptr;
using YSLib::const_pointer_cast;
using YSLib::dynamic_pointer_cast;
using YSLib::enable_shared_from_this;
using YSLib::get_deleter;
using YSLib::make_shared;
using YSLib::owner_less;
using YSLib::shared_ptr;
using YSLib::static_pointer_cast;
using YSLib::unique_ptr;
using YSLib::weak_ptr;

using YSLib::make_unique;

using platform::Nonnull;
using platform::Deref;

using YSLib::SPos;
using YSLib::SDst;

using YSLib::min;
using YSLib::max;

using YSLib::FetchZero;
using YSLib::HalfDifference;
using YSLib::IsInInterval;
using YSLib::IsInOpenInterval;

using YSLib::Drawing::GBinaryGroup;
using YSLib::Drawing::Point;
using YSLib::Drawing::Vec;
using YSLib::Drawing::Size;
using YSLib::Drawing::Rect;
#else

#define PDefH(_t, _n, ...) \
	_t \
	_n(__VA_ARGS__)

#define PDefHOp(_t, _op, ...) \
	PDefH(_t, operator _op, __VA_ARGS__)

#define ImplExpr(...) \
	{ \
		(__VA_ARGS__), void(); \
	}
#define ImplRet(...) \
	{ \
		return __VA_ARGS__; \
	}
#define ImplThrow(...) \
	{ \
		throw __VA_ARGS__; \
	}
#define ImplUnseq(...) \
	{ \
		yunused(yunseq(__VA_ARGS__)); \
	}

#define TryExpr(...) \
	try \
	ImplExpr(__VA_ARGS__)
#define TryRet(...) \
	try \
	ImplRet(__VA_ARGS__)

#define CatchExpr(_ex, ...) \
	catch(_ex) \
	ImplExpr(__VA_ARGS__)
#define CatchIgnore(_ex) \
	catch(_ex) \
	{}
#define CatchRet(_ex, ...) \
	catch(_ex) \
	ImplRet(__VA_ARGS__)
#define CatchThrow(_ex, ...) \
	catch(_ex) \
	ImplThrow(__VA_ARGS__)

#define DefDeCtor(_t) \
	_t() = default;
#define DefDelCtor(_t) \
	_t() = delete;

#define DefDeCopyCtor(_t) \
	_t(const _t&) = default;
#define DefDelCopyCtor(_t) \
	_t(const _t&) = delete;

#define DefDeMoveCtor(_t) \
	_t(_t&&) = default;
#define DefDelMoveCtor(_t) \
	_t(_t&&) = delete;

#define DefDeCopyMoveCtor(_t) \
	DefDeCopyCtor(_t) \
	DefDeMoveCtor(_t)

#define DefDeDtor(_t) \
	~_t() = default;
#define DefDelDtor(_t) \
	~_t() = delete;

#define ImplDeDtor(_t) \
	_t::DefDeDtor(_t)

#define DefDeCopyAssignment(_t) \
	_t& operator=(const _t&) = default;
#define DefDelCopyAssignment(_t) \
	_t& operator=(const _t&) = delete;

#define DefDeMoveAssignment(_t) \
	_t& operator=(_t&&) = default;
#define DefDelMoveAssignment(_t) \
	_t& operator=(_t&&) = delete;

#define DefDeCopyMoveAssignment(_t) \
	DefDeCopyAssignment(_t) \
	DefDeMoveAssignment(_t)

#define DefDeCopyMoveCtorAssignment(_t) \
	DefDeCopyMoveCtor(_t) \
	DefDeCopyMoveAssignment(_t)

#define DefCvt(_q, _t, ...) \
	operator _t() _q \
	ImplRet(__VA_ARGS__)

#define DefNeg \
	PDefHOp(bool, !, ) const ynothrow \
		ImplRet(!bool(*this))

#define DefBoolNeg(_spec, ...) \
	DefNeg \
	_spec DefCvt(const ynothrow, bool, __VA_ARGS__)

#define DefPred(_q, _n, ...) \
	bool \
	YPP_Concat(Is, _n)() _q \
		ImplRet(__VA_ARGS__)

#define DefGetter(_q, _t, _n, ...) \
	_t \
	YPP_Concat(Get, _n)() _q \
		ImplRet(__VA_ARGS__)

#define DefSetter(_t, _n, _m) \
	void \
	YPP_Concat(Set, _n)(_t _tempArgName) \
		ImplExpr((_m) = _tempArgName)

#define DefSwap(_q, _t) \
	PDefH(void, swap, _t& _x, _t& _y) _q \
		ImplExpr(_x.swap(_y))

using std::size_t;
using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::get_deleter;
using std::make_shared;
using std::owner_less;
using std::shared_ptr;
using std::static_pointer_cast;
using std::unique_ptr;
using std::weak_ptr;

template<typename _type, typename... _tParams>
inline typename
	std::enable_if<!std::is_array<_type>::value, unique_ptr<_type>>::type
make_unique(_tParams&&... args)
{
	return unique_ptr<_type>(new _type(yforward(args)...));
}
template<typename _type, typename... _tParams>
inline typename std::enable_if<std::is_array<_type>::value
	&& std::extent<_type>::value == 0, unique_ptr<_type>>::type
make_unique(size_t size)
{
	return std::unique_ptr<_type>(new
		typename std::remove_extent<_type>::type[size]());
}
template<typename _type,  typename... _tParams>
typename std::enable_if<std::extent<_type>::value != 0>::type
make_unique(_tParams&&...) = delete;


template<typename _type>
inline _type&&
Nonnull(_type&& p) ynothrow
{
	yconstraint(p);
	return yforward(p);
}

template<typename _type>
yconstfn auto
Deref(_type&& p) -> decltype(*p)
{
	return *ege::Nonnull(yforward(p));
}

using SPos = long;
using SDst = unsigned long;

#define YAssert(_expr, _msg) yassume(_expr)
#define YTraceDe(_lv, ...)

using std::min;
using std::max;

template<typename _type>
yconstfn _type
FetchZero() ynothrow
{
	return _type(0);
}

template<typename _type>
yconstfn _type
HalfDifference(_type x, _type y)
{
	return (x - y) / 2;
}

template<typename _type>
inline bool
IsInInterval(_type i, _type b) ynothrow
{
	YAssert(FetchZero<_type>() < b,
		"Zero element as lower bound is not less than upper bound.");
	return !(i < FetchZero<_type>()) && i < b;
}
template<typename _type>
inline bool
IsInInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a) && i < b;
}

template<typename _type>
inline bool
IsInOpenInterval(_type i, _type b) ynothrow
{
	YAssert(FetchZero<_type>() < b,
		"Zero element as lower bound is not less than upper bound.");
	return FetchZero<_type>() < i && i < b;
}
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b,
		"Lower bound is not less than upper bound.");
	return a < i && i < b;
}

class Size;
class Rect;

template<typename _type>
class GBinaryGroup
{
public:
	static const GBinaryGroup Invalid;

	_type X = 0, Y = 0;

	yconstfn DefDeCtor(GBinaryGroup)
	yconstfn DefDeCopyCtor(GBinaryGroup)
	explicit yconstfn
	GBinaryGroup(const Size&) ynothrow;
	explicit yconstfn
	GBinaryGroup(const Rect&) ynothrow;
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	GBinaryGroup(_tScalar1 x, _tScalar2 y) ynothrow
		: X(_type(x)), Y(_type(y))
	{}
	template<typename _tPair>
	yconstfn
	GBinaryGroup(const _tPair& pr) ynothrow
		: X(std::get<0>(pr)), Y(std::get<1>(pr))
	{}

	DefDeCopyAssignment(GBinaryGroup)

	yconstfn PDefHOp(GBinaryGroup, -, ) const ynothrow
		ImplRet(GBinaryGroup(-X, -Y))

	PDefHOp(GBinaryGroup&, +=, const GBinaryGroup& val) ynothrow
		ImplRet(yunseq(X += val.X, Y += val.Y), *this)
	PDefHOp(GBinaryGroup&, -=, const GBinaryGroup& val) ynothrow
		ImplRet(yunseq(X -= val.X, Y -= val.Y), *this)

	yconstfn DefGetter(const ynothrow, _type, X, X)
	yconstfn DefGetter(const ynothrow, _type, Y, Y)

	DefSetter(_type, X, X)
	DefSetter(_type, Y, Y)

	yconstfn DefPred(const ynothrow, Zero, X == 0 && Y == 0)

	PDefH(_type&, GetRef, bool b = true) ynothrow
		ImplRet(b ? X : Y)
	PDefH(const _type&, GetRef, bool b = true) const ynothrow
		ImplRet(b ? X : Y)
};

template<typename _type>
const GBinaryGroup<_type> GBinaryGroup<_type>::Invalid{
	std::numeric_limits<_type>::lowest(), std::numeric_limits<_type>::lowest()};

template<typename _type>
yconstfn bool
operator==(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return x.X == y.X && x.Y == y.Y;
}

template<typename _type>
yconstfn bool
operator!=(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return !(x == y);
}

template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return GBinaryGroup<_type>(x.X + y.X, x.Y + y.Y);
}

template<typename _type>
yconstfn GBinaryGroup<_type>
operator-(const GBinaryGroup<_type>& x, const GBinaryGroup<_type>& y) ynothrow
{
	return GBinaryGroup<_type>(x.X - y.X, x.Y - y.Y);
}

template<typename _type, typename _tScalar>
yconstfn GBinaryGroup<_type>
operator*(const GBinaryGroup<_type>& val, _tScalar l) ynothrow
{
	return GBinaryGroup<_type>(val.X * l, val.Y * l);
}

template<class _tBinary>
yconstfn _tBinary
Transpose(const _tBinary& obj) ynothrow
{
	return _tBinary(obj.Y, obj.X);
}

template<typename _type>
yconstfn GBinaryGroup<_type>
TransposeCCW(const GBinaryGroup<_type>& val) ynothrow
{
	return GBinaryGroup<_type>(val.Y, -val.X);
}

template<typename _type>
yconstfn GBinaryGroup<_type>
TransposeCW(const GBinaryGroup<_type>& val) ynothrow
{
	return GBinaryGroup<_type>(-val.Y, val.X);
}

template<size_t _vIdx, typename _type>
yconstfn _type&
get(GBinaryGroup<_type>& val)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? val.X : val.Y;
}
template<size_t _vIdx, typename _type>
yconstfn const _type&
get(const GBinaryGroup<_type>& val)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? val.X : val.Y;
}


using Point = GBinaryGroup<SPos>;

using Vec = GBinaryGroup<SPos>;


class EGEAPI Size
{
public:
	static const Size Invalid;

	SDst Width, Height; //!< 宽和高。

	yconstfn
	Size() ynothrow
		: Width(0), Height(0)
	{}
	yconstfn
	Size(const Size& s) ynothrow
		: Width(s.Width), Height(s.Height)
	{}
	explicit yconstfn
	Size(const Rect&) ynothrow;
	template<typename _type>
	explicit yconstfn
	Size(const GBinaryGroup<_type>& val) ynothrow
		: Width(static_cast<SDst>(val.X)), Height(static_cast<SDst>(val.Y))
	{}
	template<typename _tScalar1, typename _tScalar2>
	yconstfn
	Size(_tScalar1 w, _tScalar2 h) ynothrow
		: Width(static_cast<SDst>(w)), Height(static_cast<SDst>(h))
	{}

	DefDeCopyAssignment(Size)

	yconstfn DefBoolNeg(yconstfn explicit, Width != 0 || Height != 0)

	PDefHOp(Size&, &=, const Size& s) ynothrow
		ImplRet(yunseq(Width = min(Width, s.Width),
			Height = min(Height, s.Height)), *this)

	PDefHOp(Size&, |=, const Size& s) ynothrow
		ImplRet(yunseq(Width = max(Width, s.Width),
			Height = max(Height, s.Height)), *this)

	yconstfn DefCvt(const ynothrow, Vec, Vec(Width, Height))

	yconstfn DefPred(const ynothrow, LineSegment,
		!((Width == 0) ^ (Height == 0)))
	yconstfn DefPred(const ynothrow, UnstrictlyEmpty, Width == 0 || Height == 0)

	PDefH(SDst&, GetRef, bool b = true) ynothrow
		ImplRet(b ? Width : Height)
	PDefH(const SDst&, GetRef, bool b = true) const ynothrow
		ImplRet(b ? Width : Height)
};

yconstfn PDefHOp(bool, ==, const Size& x, const Size& y) ynothrow
	ImplRet(x.Width == y.Width && x.Height == y.Height)

yconstfn PDefHOp(bool, !=, const Size& x, const Size& y) ynothrow
	ImplRet(!(x == y))

inline PDefHOp(Size, &, const Size& x, const Size& y) ynothrow
	ImplRet({min(x.Width, y.Width), min(x.Height, y.Height)})

inline PDefHOp(Size, |, const Size& x, const Size& y) ynothrow
	ImplRet({max(x.Width, y.Width), max(x.Height, y.Height)})

yconstfn PDefH(auto, GetAreaOf, const Size& s) ynothrow
	-> decltype(s.Width * s.Height)
	ImplRet(s.Width * s.Height)

yconstfn PDefH(Point, LocateCenter, const Size& x, const Size& y)
	ImplRet({HalfDifference(SPos(x.Width), SPos(y.Width)),
		HalfDifference(SPos(x.Height), SPos(y.Height))})

yconstfn PDefH(Point, RotateCenter, const Size& s)
	ImplRet({HalfDifference(SPos(s.Width), SPos(s.Height)),
		HalfDifference(SPos(s.Height), SPos(s.Width))})

template<size_t _vIdx>
yconstfn SDst&
get(Size& s)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? s.Width : s.Height;
}
template<size_t _vIdx>
yconstfn const SDst&
get(const Size& s)
{
	static_assert(_vIdx < 2, "Invalid index found.");

	return _vIdx == 0 ? s.Width : s.Height;
}


template<typename _type>
yconstfn GBinaryGroup<_type>
operator+(GBinaryGroup<_type> val, const Size& s) ynothrow
{
	return {val.X + _type(s.Width), val.Y + _type(s.Height)};
}

template<typename _type>
yconstfn GBinaryGroup<_type>
operator-(GBinaryGroup<_type> val, const Size& s) ynothrow
{
	return {val.X - _type(s.Width), val.Y - _type(s.Height)};
}


yconstfn PDefH(Size, Transpose, const Size& s) ynothrow
	ImplRet({s.Height, s.Width})


template<typename _tScalar = float>
inline _tScalar
ScaleMin(const Size& x, const Size& y, _tScalar threshold = 1.F)
{
	return std::min({threshold,
		_tScalar(_tScalar(x.Width) / _tScalar(y.Width)),
		_tScalar(_tScalar(x.Height) / _tScalar(y.Height))});
}


class EGEAPI Rect : private Point, private Size
{
public:
	static const Rect Invalid;

	using Point::X;
	using Point::Y;
	using Size::Width;
	using Size::Height;

	DefDeCtor(Rect)
	explicit yconstfn
	Rect(const Point& pt) ynothrow
		: Point(pt), Size()
	{}
	yconstfn
	Rect(const Size& s) ynothrow
		: Point(), Size(s)
	{}
	yconstfn
	Rect(const Point& pt, const Size& s) ynothrow
		: Point(pt), Size(s)
	{}
	yconstfn
	Rect(const Point& pt, SDst w, SDst h) ynothrow
		: Point(pt.X, pt.Y), Size(w, h)
	{}
	yconstfn
	Rect(SPos x, SPos y, const Size& s) ynothrow
		: Point(x, y), Size(s.Width, s.Height)
	{}
	yconstfn
	Rect(SPos x, SPos y, SDst w, SDst h) ynothrow
		: Point(x, y), Size(w, h)
	{}
	yconstfn DefDeCopyCtor(Rect)

	DefDeCopyAssignment(Rect)
	Rect&
	operator=(const Point& pt) ynothrow
	{
		yunseq(X = pt.X, Y = pt.Y);
		return *this;
	}
	Rect&
	operator=(const Size& s) ynothrow
	{
		yunseq(Width = s.Width, Height = s.Height);
		return *this;
	}

	Rect&
	operator&=(const Rect&) ynothrow;

	Rect&
	operator|=(const Rect&) ynothrow;

	using Size::operator!;

	using Size::operator bool;

	bool
	Contains(SPos px, SPos py) const ynothrow;
	PDefH(bool, Contains, const Point& pt) const ynothrow
		ImplRet(Contains(pt.X, pt.Y))
	bool
	Contains(const Rect&) const ynothrow;
	bool
	ContainsStrict(SPos px, SPos py) const ynothrow;
	PDefH(bool, ContainsStrict, const Point& pt) const ynothrow
		ImplRet(ContainsStrict(pt.X, pt.Y))
	bool
	ContainsStrict(const Rect&) const ynothrow;
	using Size::IsLineSegment;
	using Size::IsUnstrictlyEmpty;

	yconstfn DefGetter(const ynothrow, SPos, Bottom, Y + SPos(Height))
	yconstfn DefGetter(const ynothrow, const Point&, Point,
		static_cast<const Point&>(*this))
	DefGetter(ynothrow, Point&, PointRef, static_cast<Point&>(*this))
	yconstfn DefGetter(const ynothrow, SPos, Right, X + SPos(Width))
	yconstfn DefGetter(const ynothrow, const Size&, Size,
		static_cast<const Size&>(*this))
	DefGetter(ynothrow, Size&, SizeRef, static_cast<Size&>(*this))
	using Point::GetX;
	using Point::GetY;

	using Point::SetX;
	using Point::SetY;
};

yconstfn PDefHOp(bool, ==, const Rect& x, const Rect& y) ynothrow
	ImplRet(x.GetPoint() == y.GetPoint() && x.GetSize() == y.GetSize())

yconstfn PDefHOp(bool, !=, const Rect& x, const Rect& y) ynothrow
	ImplRet(!(x == y))

yconstfn PDefHOp(Rect, +, const Rect& r, const Vec& v) ynothrow
	ImplRet({r.GetPoint() + v, r.GetSize()})

yconstfn PDefHOp(Rect, -, const Rect& r, const Vec& v) ynothrow
	ImplRet({r.GetPoint() - v, r.GetSize()})

inline PDefHOp(Rect, &, const Rect& x, const Rect& y) ynothrow
	ImplRet(Rect(x) &= y)

inline PDefHOp(Rect, |, const Rect& x, const Rect& y) ynothrow
	ImplRet(Rect(x) |= y)

inline PDefH(void, Diminish, Rect& r, SDst off1 = 1, SDst off2 = 2)
	ImplExpr(YAssert(r.Width > off2 && r.Height > off2,
		"Boundary is too small."), yunseq(r.X += SPos(off1), r.Y += SPos(off1),
		r.Width -= off2, r.Height -= off2))

inline PDefH(Rect, MakeRect, const Point& pt1, const Point& pt2) ynothrow
	ImplRet(Rect(std::min(pt1.X, pt2.X), std::min(pt1.Y, pt2.Y),
		SDst(pt1.X < pt2.X ? pt2.X - pt1.X : pt1.X - pt2.X),
		SDst(pt1.Y < pt2.Y ? pt2.Y - pt1.Y : pt1.Y - pt2.Y)))

inline PDefH(Rect, TransposeCCW, const Rect& r) ynothrow
	ImplRet(MakeRect(TransposeCCW(Point(r.GetRight(), r.Y)),
		TransposeCCW(Point(r.X, r.GetBottom()))))

inline PDefH(Rect, TransposeCW, const Rect& r) ynothrow
	ImplRet(MakeRect(TransposeCW(Point(r.GetRight(), r.Y)),
		TransposeCW(Point(r.X, r.GetBottom()))))

template<size_t _vIdx>
yconstfn typename std::conditional<_vIdx < 2, SPos, SDst>::type&
get(Rect& r)
{
	return std::get<_vIdx>(std::tuple<SPos&, SPos&, SDst&, SDst&>(r.X, r.Y,
		r.Width, r.Height));
}
template<size_t _vIdx>
yconstfn const typename std::conditional<_vIdx < 2, SPos, SDst>::type&
get(const Rect& r)
{
	return std::get<_vIdx>(std::tuple<const SPos&, const SPos&, const SDst&,
		const SDst&>(r.X, r.Y, r.Width, r.Height));
}


template<typename _type>
yconstfn
GBinaryGroup<_type>::GBinaryGroup(const Rect& r) ynothrow
	: X(r.X), Y(r.Y)
{}

yconstfn
Size::Size(const Rect& r) ynothrow
	: Width(r.Width), Height(r.Height)
{}
#endif

const double PI = 3.14159265358979323;

enum graphics_errors        /* graphresult error return codes */
{
	grOk                =   0,
	grNoInitGraph       =  -1,
	grNotDetected       =  -2,
	grFileNotFound      =  -3,
	grInvalidDriver     =  -4,
	grNoLoadMem         =  -5,
	grNoScanMem         =  -6,
	grNoFloodMem        =  -7,
	grFontNotFound      =  -8,
	grNoFontMem         =  -9,
	grInvalidMode       = -10,
	grError             = -11,   /* generic error */
	grIOerror           = -12,
	grInvalidFont       = -13,
	grInvalidFontNum    = -14,
	grInvalidVersion    = -18,
	grException         = 0x10,  /* ege error */
	grParamError        = 0x11,
	grInvalidRegion     = 0x12,
	grOutOfMemory       = 0x13,
	grNullPointer       = 0x14,
	grAllocError        = 0x15,
	grInvalidMemory     = 0xCDCDCDCD,
};

// 颜色
enum COLORS
{
	BLACK           = 0,
	BLUE            = EGERGB(0, 0, 0xA8),
	GREEN           = EGERGB(0, 0xA8, 0),
	CYAN            = EGERGB(0, 0xA8, 0xA8),
	RED             = EGERGB(0xA8, 0, 0),
	MAGENTA         = EGERGB(0xA8, 0, 0xA8),
	BROWN           = EGERGB(0xA8, 0xA8, 0),
	LIGHTGRAY       = EGERGB(0xA8, 0xA8, 0xA8),
	DARKGRAY        = EGERGB(0x54, 0x54, 0x54),
	LIGHTBLUE       = EGERGB(0x54, 0x54, 0xFC),
	LIGHTGREEN      = EGERGB(0x54, 0xFC, 0x54),
	LIGHTCYAN       = EGERGB(0x54, 0xFC, 0xFC),
	LIGHTRED        = EGERGB(0xFC, 0x54, 0x54),
	LIGHTMAGENTA    = EGERGB(0xFC, 0x54, 0xFC),
	YELLOW          = EGERGB(0xFC, 0xFC, 0x54),
	WHITE           = EGERGB(0xFC, 0xFC, 0xFC),
};

// 填充模式
enum fill_patterns  /* Fill patterns for get/setfillstyle */
{
	EMPTY_FILL,     /* fills area in background color */
	SOLID_FILL,     /* fills area in solid fill color */
	LINE_FILL,      /* --- fill */
	LTSLASH_FILL,   /* /// fill */
	SLASH_FILL,     /* /// fill with thick lines */
	BKSLASH_FILL,   /* \\\ fill with thick lines */
	LTBKSLASH_FILL, /* \\\ fill */
	HATCH_FILL,     /* light hatch fill */
	XHATCH_FILL,    /* heavy cross hatch fill */
	INTERLEAVE_FILL,/* interleaving line fill */
	WIDE_DOT_FILL,  /* Widely spaced dot fill */
	CLOSE_DOT_FILL, /* Closely spaced dot fill */
	USER_FILL       /* user defined fill */
};

enum text_just /* Horizontal and vertical justification for settextjustify */
{
	LEFT_TEXT   = 0,
	CENTER_TEXT = 1,
	RIGHT_TEXT  = 2,

	BOTTOM_TEXT = 0,
	/* CENTER_TEXT = 1,  already defined above */
	TOP_TEXT    = 2
};

enum key_msg_flag
{
	KEYMSG_CHAR_FLAG    = 2,
	KEYMSG_DOWN_FLAG    = 1,
	KEYMSG_UP_FLAG      = 1,

	KEYMSG_CHAR         = 0x40000,
	KEYMSG_DOWN         = 0x10000,
	KEYMSG_UP           = 0x20000,
	KEYMSG_FIRSTDOWN    = 0x80000,
};

enum music_state_flag
{
	MUSIC_MODE_NOT_OPEN = 0x0,
	MUSIC_MODE_NOT_READY = 0x20C,
	MUSIC_MODE_PAUSE = 0x211,
	MUSIC_MODE_PLAY = 0x20E,
	MUSIC_MODE_STOP = 0x20D,
	MUSIC_MODE_OPEN = 0x212,
	MUSIC_MODE_SEEK = 0x210,
};

enum pattern_type_e
{
	pattern_none            = 0,
	pattern_lineargradient  = 1,
	pattern_pathgradient    = 2,
	pattern_texture         = 3
};


struct textsettingstype
{
	int font;
	int direction;
	int charsize;
	int horiz;
	int vert;
};


struct linestyletype
{
	int linestyle = 0;
	unsigned short upattern = 0;
	int thickness = 0;

	linestyletype() = default;
	linestyletype(int l, unsigned short u, int t)
		: linestyle(l), upattern(u), thickness(t)
	{}
};


struct ege_point
{
	float x;
	float y;
};


struct ege_rect
{
	float x;
	float y;
	float w;
	float h;
};


class IMAGE;

}

#endif

