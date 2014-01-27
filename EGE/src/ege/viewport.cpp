#include "ege/viewport.h"
#include "image.h"
#include "global.h"

namespace ege
{

void
getviewport(int* pleft, int* ptop, int* pright, int* pbottom, int* pclip,
	IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE_CONST(pimg);
	if(pleft)
		*pleft = img->m_vpt.left;
	if(ptop)
		*ptop = img->m_vpt.top;
	if(pright)
		*pright = img->m_vpt.right;
	if(pbottom)
		*pbottom = img->m_vpt.bottom;
	if(pclip)
		*pclip = img->m_vpt.clipflag;
}

void
setviewport(int left, int top, int right, int bottom, int clip, IMAGE* pimg)
{
	//_graph_setting * pg = &get_global_state();

	const auto img = CONVERT_IMAGE(pimg);

	::SetViewportOrgEx(img->m_hDC, 0, 0, {});

	img->m_vpt.left     = left;
	img->m_vpt.top      = top;
	img->m_vpt.right    = right;
	img->m_vpt.bottom   = bottom;
	img->m_vpt.clipflag = clip;

	if(img->m_vpt.left < 0)
		img->m_vpt.left = 0;
	if(img->m_vpt.top < 0)
		img->m_vpt.top = 0;
	if(img->m_vpt.right > img->m_width)
		img->m_vpt.right = img->m_width;
	if(img->m_vpt.bottom > img->m_height)
		img->m_vpt.bottom = img->m_height;

	::HRGN rgn = {};

	if(img->m_vpt.clipflag)
		rgn = ::CreateRectRgn(img->m_vpt.left, img->m_vpt.top, img->m_vpt.right,
			img->m_vpt.bottom);
	else
		rgn = ::CreateRectRgn(0, 0, img->m_width,img->m_height);
	::SelectClipRgn(img->m_hDC, rgn);
	::DeleteObject(rgn);

	//OffsetViewportOrgEx(img->m_hDC, img->m_vpt.left, img->m_vpt.top, {});
	::SetViewportOrgEx(img->m_hDC, img->m_vpt.left, img->m_vpt.top, {});
}

void
clearviewport(IMAGE* pimg)
{
	const auto img = CONVERT_IMAGE(pimg);

	if(img && img->m_hDC)
	{
		::RECT rect{0, 0, img->m_vpt.right - img->m_vpt.left,
			img->m_vpt.bottom - img->m_vpt.top};
		::HBRUSH hbr_c = (::HBRUSH)::GetCurrentObject(img->m_hDC, OBJ_BRUSH);
		::LOGBRUSH logBrush;

		::GetObject(hbr_c, sizeof(logBrush), &logBrush);
		::HBRUSH hbr = ::CreateSolidBrush(logBrush.lbColor);
		::FillRect(img->m_hDC, &rect, hbr);
		::DeleteObject(hbr);
	}
}

void
window_getviewport(viewporttype* viewport)
{
	auto pg = &get_global_state();
	viewport->left   = pg->base_x;
	viewport->top    = pg->base_y;
	viewport->right  = pg->base_w + pg->base_x;
	viewport->bottom = pg->base_h + pg->base_y;
}

void
window_getviewport(int* left, int* top, int* right, int* bottom)
{
	auto pg = &get_global_state();
	if(left)   *left   = pg->base_x;
	if(top)    *top    = pg->base_y;
	if(right)  *right  = pg->base_w + pg->base_x;
	if(bottom) *bottom = pg->base_h + pg->base_y;
}

void
window_setviewport(int left, int top, int right, int bottom)
{
	auto pg = &get_global_state();
	int same_xy = 0, same_wh = 0;
	if(pg->base_x == left && pg->base_y == top)
		same_xy = 1;
	if(pg->base_w == bottom - top && pg->base_h == right - left)
		same_wh = 1;
	pg->base_x = left;
	pg->base_y = top;
	pg->base_w = right - left;
	pg->base_h = bottom - top;
	if(same_xy == 0 || same_wh == 0)
		--update_mark_count;
	/*修正窗口大小*/
	if(same_wh == 0)
	{
		::RECT rect, crect;
		int dw, dh;
		::GetClientRect(pg->hwnd, &crect);
		::GetWindowRect(pg->hwnd, &rect);
		dw = pg->base_w - crect.right;
		dh = pg->base_h - crect.bottom;
		{
			::HWND hwnd = GetParent(pg->hwnd);
			if(hwnd)
			{
				::POINT pt{0, 0};
				::ClientToScreen(hwnd, &pt);
				rect.left   -= pt.x;
				rect.top    -= pt.y;
				rect.right  -= pt.x;
				rect.bottom -= pt.y;
			}

			::MoveWindow(pg->hwnd, rect.left, rect.top,
				rect.right + dw - rect.left, rect.bottom + dh - rect.top, TRUE);
		}
	}
}

} // namespace ege;

