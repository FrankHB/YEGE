#include "ege/head.h"
#include <cmath>
#include <cstdarg>
#include "zlib.h"
#include "ege/sys_edit.h"
#include "ege/common.h"

namespace ege
{

void
setactivepage(int page)
{
	struct _graph_setting* pg = &graph_setting;
	pg->active_page = page;
	if(pg->img_page[page] == NULL)
	{
		pg->img_page[page] = new IMAGE;
		pg->img_page[page]->createimage(pg->dc_w, pg->dc_h);
		pg->dc = pg->img_page[page]->m_hDC;
	}
}

void
setvisualpage(int page)
{
	struct _graph_setting* pg = &graph_setting;
	pg->visual_page = page;
	if(pg->img_page[page] == NULL)
	{
		pg->img_page[page] = new IMAGE;
		pg->img_page[page]->createimage(pg->dc_w, pg->dc_h);
	}
	pg->update_mark_count = 0;
}

#if 0
void
swappage()
{
	struct _graph_setting* pg = &graph_setting;
	setvisualpage(pg->active_page);
	setactivepage(1 - pg->active_page);
}
#endif

} // namespace ege
