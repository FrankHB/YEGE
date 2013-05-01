#include "ege/head.h"
#include <cmath>
#include <cstdarg>
#include "zlib.h"
#include "ege/sys_edit.h"
#include "ege/common.h"

namespace ege
{

void
_graph_setting::_setactivepage(int page)
{
	active_page = page;
	if(img_page[page] == nullptr)
	{
		img_page[page] = new IMAGE;
		img_page[page]->createimage(dc_w, dc_h);
		dc = img_page[page]->m_hDC;
	}
}

void
_graph_setting::_setvisualpage(int page)
{
	visual_page = page;
	if(!img_page[page])
	{
		img_page[page] = new IMAGE;
		img_page[page]->createimage(dc_w, dc_h);
	}
	update_mark_count = 0;
}

void
setactivepage(int page)
{
	graph_setting._setactivepage(page);
}

void
setvisualpage(int page)
{
	graph_setting._setvisualpage(page);
}

} // namespace ege
