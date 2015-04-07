#include "ege/viewport.h"
#include "image.h"
#include "global.h"

namespace ege
{

void
getviewport(int* pleft, int* ptop, int* pright, int* pbottom, int* pclip,
	IMAGE* pimg)
{
	auto& vpt(cimg_ref_c(pimg).m_vpt);

	if(pleft)
		*pleft = vpt.left;
	if(ptop)
		*ptop = vpt.top;
	if(pright)
		*pright = vpt.right;
	if(pbottom)
		*pbottom = vpt.bottom;
	if(pclip)
		*pclip = vpt.clipflag;
}

void
setviewport(int left, int top, int right, int bottom, int clip, IMAGE* pimg)
{
	cimg_ref(pimg).SetViewport(left, top, right, bottom, clip);
}

void
clearviewport(IMAGE* pimg)
{
	auto& img(cimg_ref(pimg));

	if(img.getdc())
	{
		::RECT rect{0, 0, img.m_vpt.right - img.m_vpt.left,
			img.m_vpt.bottom - img.m_vpt.top};
		::HBRUSH hbr_c = ::HBRUSH(::GetCurrentObject(img.getdc(),
			OBJ_BRUSH));
		::LOGBRUSH logBrush;

		::GetObjectW(hbr_c, sizeof(logBrush), &logBrush);

		::HBRUSH hbr(::CreateSolidBrush(logBrush.lbColor));

		::FillRect(img.getdc(), &rect, hbr);
		::DeleteObject(hbr);
	}
}

void
window_getviewport(viewporttype* viewport)
{
	auto& pages(get_pages());

	viewport->left = pages.base_x;
	viewport->top = pages.base_y;
	viewport->right = pages.base_w + pages.base_x;
	viewport->bottom = pages.base_h + pages.base_y;
}

void
window_getviewport(int* left, int* top, int* right, int* bottom)
{
	auto& pages(get_pages());

	if(left)
		*left = pages.base_x;
	if(top)
		*top = pages.base_y;
	if(right)
		*right = pages.base_w + pages.base_x;
	if(bottom)
		*bottom = pages.base_h + pages.base_y;
}

void
window_setviewport(int left, int top, int right, int bottom)
{
	auto& pages(get_pages());
	auto& gstate(FetchEGEApplication());
	const auto hwnd(gstate._get_hwnd());
	const bool same_xy(pages.base_x == left && pages.base_y == top),
		same_wh(pages.base_w == bottom - top && pages.base_h == right - left);

	pages.base_x = left;
	pages.base_y = top;
	pages.base_w = right - left;
	pages.base_h = bottom - top;
	if(!same_xy || !same_wh)
		--update_mark_count;
	/*修正窗口大小*/
	if(!same_wh)
	{
		::RECT rect, crect;
		int dw, dh;
		::GetClientRect(hwnd, &crect);
		::GetWindowRect(hwnd, &rect);
		dw = pages.base_w - crect.right;
		dh = pages.base_h - crect.bottom;
		{
			::HWND hparent = GetParent(hwnd);
			if(hparent)
			{
				::POINT pt{0, 0};
				::ClientToScreen(hparent, &pt);
				rect.left   -= pt.x;
				rect.top    -= pt.y;
				rect.right  -= pt.x;
				rect.bottom -= pt.y;
			}
			::MoveWindow(hwnd, rect.left, rect.top,
				rect.right + dw - rect.left, rect.bottom + dh - rect.top, true);
		}
	}
}

} // namespace ege;

