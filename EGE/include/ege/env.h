#ifndef Inc_ege_env_h_
#define Inc_ege_env_h_

#include "ege/def.h"

namespace ege
{

// 绘图环境相关函数

EGEAPI void
initgraph(int* gdriver, int* gmode, char* path);   // 兼容 Borland C++ 3.1 的重载，只使用 640x480x24bit

EGEAPI void
closegraph();                                      // 关闭图形环境

EGEAPI bool
is_run();   // 判断UI是否退出

} // namespace ege

#endif

