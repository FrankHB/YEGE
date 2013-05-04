/*********************************************************
* YEGE (Yet another Easy Graphics Engine)
* FileName      graphics.h
* GitHub        https://github.com/FrankHB/yege
* 原作：
* HomePage1     http://misakamm.github.com/xege
* HomePage2     http://misakamm.bitbucket.org/index.htm
* HomePage3     http://tcgraphics.sourceforge.net
* teiba1        http://tieba.baidu.com/f?kw=ege
* teiba2        http://tieba.baidu.com/f?kw=ege%C4%EF
* Blog:         http://blog.misakamm.org
* E-Mail:       mailto:misakamm[at gmail com]
* QQ:           moegirl[at misakamm org]
*
* FileName: graphics.h
* 在 Win32 下模拟 Borland BGI 绘图库，实现简单的绘图之余，扩展了较复杂的绘图能力
*
* 不支持包含 conio.h 头文件
* 使用本库需要 C++ 编译器，可支持的编译器：
* MinGW G++ 4.8.0 或以上
*********************************************************/

/****************************************************************************
** 注意事项：
* ★delay_ms(0)能自行判断有没有更新的必要，连续多次但不大量的调用并不会产生帧率的影响
* ★调用delay_ms, delay_fps, getch, GetMouseMsg 时，窗口内容可能会更新，这些函数相当于内置了 delay_ms(0)，
* ★如果你只需要更新窗口，而不想等待，可以用delay_ms(0)或者delay(0)
* ★如果你使用了锁定模式绘图，那可以使用WindowFlush代替delay(0)，但不能代替非 0 的 delay
* ★合理地使用delay函数，可以减少你的程序占用的CPU，否则一个delay都没有调用的话，程序将占满一个CPU的时间
* ★setfillstyle函数的自定义填充模式未实现，但一般应用足够
****************************************************************************/

#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include "ege.h"
using namespace ege;

#endif

