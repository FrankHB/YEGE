#ifndef Inc_ege_time_h_
#define Inc_ege_time_h_

#include "ege/def.h"

namespace ege
{

//时间函数（以下函数不能在多线程下使用，只能给绘图主线程调用）
EGEAPI void
delay_ms(long ms);      // 平均延迟ms毫秒

EGEAPI void
delay_fps(double fps);  // 平均延迟1000/fps毫秒，用于稳定帧率控制

EGEAPI void
delay_jfps(double fps); // 平均延迟1000/fps毫秒，用于稳定逻辑帧率控制，绘图带跳帧

// 以下函数可以多线程下使用，非图形(worker)线程的sleep使用这个
EGEAPI void
ege_sleep(long ms);     // 至少延迟ms毫秒

EGEAPI double
fclock(); // 获取以秒为单位的浮点时间，只用于计时用，精度0.01秒，

}

#endif

