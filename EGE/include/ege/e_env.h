#pragma once

#include "e_def.h"

namespace ege
{

// 绘图环境相关函数

void EGEAPI initgraph(int* gdriver, int* gmode, char* path);   // 兼容 Borland C++ 3.1 的重载，只使用 640x480x24bit
void EGEAPI closegraph();                                      // 关闭图形环境
bool EGEAPI is_run();   // 判断UI是否退出

} // namespace ege

