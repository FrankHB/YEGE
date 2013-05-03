#ifndef Inc_ege_e_input_h_
#define Inc_ege_e_input_h_

#include <wingdi.h>
#include "e_base.h"
#include "e_mac.h"
#include "e_img.h"

namespace ege
{

//键盘处理函数
EGEAPI int kbmsg();
key_msg EGEAPI getkey();
EGEAPI int keystate(int key);       // 获得键码为key的键（见key_code_e）是否按下，如果key使用鼠标按键的键码，则获得的是鼠标键状态
EGEAPI void flushkey();              // 清空键盘消息缓冲区

EGEAPI int getch();
EGEAPI int kbhit();

//鼠标处理函数
EGEAPI int mousemsg();                  // 检查是否存在鼠标消息
mouse_msg EGEAPI getmouse();                  // 获取一个鼠标消息。如果没有，就等待

EGEAPI void flushmouse();                // 清空鼠标消息缓冲区
EGEAPI int showmouse(int bShow);        // 设置是否显示鼠标
EGEAPI int mousepos(int* x, int* y); // 获取当前鼠标位置

}

#endif

