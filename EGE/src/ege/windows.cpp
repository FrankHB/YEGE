#include "ege/windows.h" // for ::MultiByteToWideChar;
#include "global.h"
#if YEGE_Use_YSLib
#	include "ege/base.h" // for ege::Nonnull;
#else
#	include <limits>
#	include <algorithm>
#endif

namespace ege
{

#if !YEGE_Use_YSLib

namespace
{

template<typename _tDst, typename _type>
inline _tDst
CheckUpperBound(_type val, const std::string& = "")
{
	using common_t = typename std::common_type<_tDst, _type>::type;

	if(YB_UNLIKELY(common_t(val) > common_t(std::numeric_limits<_tDst>::max())))
		throw std::runtime_error("value out of range");
	return _tDst(val);
}

template<typename _tDst, typename _type>
inline _tDst
CheckPositive(_type val, const std::string& name = "")
{
	if(!(0 < val))
		throw std::runtime_error("failed getting positive value");
	return CheckUpperBound<_tDst>(val, name);
}

using Pixel = color_int_t*;

template<class _tWString>
YB_ATTR_nodiscard YB_NONNULL(3) _tWString
MBCSToWCSImpl(const typename _tWString::allocator_type& a, int l,
	const char* str, unsigned cp)
{
	const int w_len(::MultiByteToWideChar(cp, 0, ege::Nonnull(str), l, {}, 0));

	if(w_len != 0)
	{
		_tWString res(CheckPositive<size_t>(w_len), wchar_t(), a);

		::MultiByteToWideChar(cp, 0, str, l, &res[0], w_len);
		if(l == -1 && !res.empty())
			res.pop_back();
		return res;
	}
	return _tWString(a);
}

} // unnamed namespace;

ScreenBuffer::ScreenBuffer(const Size& s)
	: size(s), hBitmap([this]{
		::BITMAPINFO bmi{{sizeof(::BITMAPINFOHEADER), CheckPositive<SPos>(
			size.Width, "width"), -CheckPositive<SPos>(size.Height,
			"height") - 1, 1, 32, BI_RGB, static_cast<unsigned long>(
			sizeof(Pixel) * size.Width * size.Height), 0, 0, 0, 0}, {}};
		void* p_buf{};
		const auto h(::CreateDIBSection({}, &bmi, DIB_RGB_COLORS,
			&reinterpret_cast<void*&>(p_buf), {}, 0));

		pBuffer = reinterpret_cast<BitmapPtr>(p_buf);
		return h;
	}())
{}
ScreenBuffer::ScreenBuffer(ScreenBuffer&& sbuf) ynothrow
	: size(sbuf.size), hBitmap(sbuf.hBitmap)
{
	sbuf.hBitmap = {};
}
ScreenBuffer::~ScreenBuffer()
{
	if(hBitmap)
		// XXX: Error ignored.
		::DeleteObject(hBitmap);
}

ScreenBuffer&
ScreenBuffer::operator=(ScreenBuffer&& sbuf)
{
	sbuf.swap(*this);
	return *this;
}

void
ScreenBuffer::Resize(const Size& s)
{
	if(s != size)
		*this = ScreenBuffer(s);
}

void
ScreenBuffer::UpdateFrom(ConstBitmapPtr p_buf) ynothrow
{
	std::copy_n(Nonnull(p_buf), size.Width * size.Height, GetBufferPtr());
}

void
ScreenBuffer::swap(ScreenBuffer& sbuf) ynothrow
{
	std::swap(size, sbuf.size),
	std::swap(pBuffer, sbuf.pBuffer),
	std::swap(hBitmap, sbuf.hBitmap);
}
#endif

void
setcaption(const char* caption)
{
	::SetWindowTextA(getHWnd(), caption);
}
void
setcaption(const wchar_t* caption)
{
	::SetWindowTextW(getHWnd(), caption);
}

::HWND
getHWnd()
{
	return FetchEGEApplication()._get_hwnd();
}

::HINSTANCE
getHInstance()
{
	return EGEApplication::GetInstance();
}


#if !YEGE_Use_YSLib
std::wstring
MBCSToWCS(const char* str, unsigned cp)
{
	return MBCSToWCSImpl<std::wstring>({}, -1, str, cp);
}
#endif

} // namespace ege;

