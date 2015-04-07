#include "ege/base.h"

namespace ege
{

#if !YEGE_Use_YSLib

const Size Size::Invalid(std::numeric_limits<SDst>::lowest(),
	std::numeric_limits<SDst>::lowest());

namespace
{

bool
RectContainsRaw(const Rect& r, SPos px, SPos py) ynothrow
{
	YAssert(r.Width > 0, "Invalid width found."),
	YAssert(r.Height > 0, "Invalid height found.");

	return IsInInterval<SPos>(px - r.X, SPos(r.Width))
		&& IsInInterval<SPos>(py - r.Y, SPos(r.Height));
}
inline bool
RectContainsRaw(const Rect& r, const Point& pt) ynothrow
{
	return RectContainsRaw(r, pt.X, pt.Y);
}

bool
RectContainsStrictRaw(const Rect& r, SPos px, SPos py) ynothrow
{
	YAssert(r.Width > 1, "Invalid width found."),
	YAssert(r.Height > 1, "Invalid height found.");
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return IsInOpenInterval<SPos>(px - r.X, SPos(r.Width - 1))
		&& IsInOpenInterval<SPos>(py - r.Y, SPos(r.Height - 1));
}
inline bool
RectContainsStrictRaw(const Rect& r, const Point& pt) ynothrow
{
	return RectContainsStrictRaw(r, pt.X, pt.Y);
}

} // unnamed namespace;

const Rect Rect::Invalid(Size::Invalid);

bool
Rect::Contains(SPos px, SPos py) const ynothrow
{
	return !IsUnstrictlyEmpty() && RectContainsRaw(*this, px, py);
}
bool
Rect::Contains(const Rect& r) const ynothrow
{
	return !IsUnstrictlyEmpty() && RectContainsRaw(*this, r.GetPoint())
		&& RectContainsRaw(*this, r.GetPoint() + r.GetSize() - Vec(1, 1));
}

bool
Rect::ContainsStrict(SPos px, SPos py) const ynothrow
{
	return Width > 1 && Height > 1 && RectContainsStrictRaw(*this, px, py);
}
bool
Rect::ContainsStrict(const Rect& r) const ynothrow
{
	return Width > 1 && Height > 1 && !r.IsUnstrictlyEmpty()
		&& RectContainsStrictRaw(*this, r.GetPoint())
		&& RectContainsStrictRaw(*this, r.GetPoint() + r.GetSize() - Vec(1, 1));
}

Rect&
Rect::operator&=(const Rect& r) ynothrow
{
	const SPos x1(max(X, r.X)), x2(min(GetRight(), r.GetRight())),
		y1(max(Y, r.Y)), y2(min(GetBottom(), r.GetBottom()));

	return *this = x2 < x1 || y2 < y1 ? Rect()
		: Rect(x1, y1, SDst(x2 - x1), SDst(y2 - y1));
}

Rect&
Rect::operator|=(const Rect& r) ynothrow
{
	if(!*this)
		return *this = r;
	if(!r)
		return *this;

	const SPos mx(min(X, r.X)), my(min(Y, r.Y));

	return *this = Rect(mx, my, SDst(max(GetRight(), r.GetRight())
		- mx), SDst(max(GetBottom(), r.GetBottom()) - my));
}

#endif

} // namespace ege;

