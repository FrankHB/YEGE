#ifndef Inc_ege_env_h_
#define Inc_ege_env_h_

#include "ege/def.h"

namespace ege
{

enum initmode_flag
{
	INIT_NOBORDER       = 0x1,
	INIT_CHILD          = 0x2,
	INIT_TOPMOST        = 0x4,
	INIT_RENDERMANUAL   = 0x8,
	INIT_NOFORCEEXIT    = 0x10,
	INIT_WITHLOGO       = 0x100,
	INIT_DEFAULT    = 0x0,
	INIT_ANIMATION  = INIT_DEFAULT | INIT_RENDERMANUAL | INIT_NOFORCEEXIT,
};


// 绘图环境相关函数

EGEAPI void
initgraph(int Width, int Height, int Flag = INIT_DEFAULT);    // 初始化图形环境
EGEAPI void
initgraph(int* gdriver, int* gmode, char* path); // 兼容 Borland C++ 3.1 的重载，只使用 640x480x24bit

EGEAPI void
closegraph(); // 关闭图形环境

EGEAPI bool
is_run();   // 判断UI是否退出

} // namespace ege;

#endif

