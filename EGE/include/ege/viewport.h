#ifndef Inc_ege_viewport_h_
#define Inc_ege_viewport_h_

#include "ege/def.h"

namespace ege
{

class IMAGE;

struct viewporttype
{
	int left;
	int top;
	int right;
	int bottom;
	int clipflag;
};


EGEAPI void
getviewport(int* pleft, int* ptop, int* pright, int* pbottom,
	int* pclip = {}, IMAGE* pimg = {}); //获取视图信息

EGEAPI void
setviewport(int left, int top, int right, int bottom, int clip = 1,
	IMAGE* pimg = {});           //设置视图

EGEAPI void
clearviewport(IMAGE* pimg = {}); //清空视图


EGEAPI void
window_getviewport(viewporttype* viewport);

EGEAPI void
window_getviewport(int* left, int* top, int* right, int* bottom);

EGEAPI void
window_setviewport(int  left, int  top, int  right, int  bottom);

}

#endif

