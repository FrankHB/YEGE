#pragma once

#include "e_base.h"

namespace ege
{

//时间函数（以下函数不能在多线程下使用，只能给绘图主线程调用）
void EGEAPI ege_sleep(long ms);     // 至少延迟ms毫秒
void EGEAPI delay(long ms);         // 至少延迟ms毫秒
void EGEAPI delay_ms(long ms);      // 平均延迟ms毫秒
void EGEAPI delay_fps(int fps);    // 平均延迟1000/fps毫秒，用于稳定帧率控制
void EGEAPI delay_fps(long fps);    // 平均延迟1000/fps毫秒，用于稳定帧率控制
void EGEAPI delay_fps(double fps);  // 平均延迟1000/fps毫秒，用于稳定帧率控制
void EGEAPI delay_jfps(int fps);   // 平均延迟1000/fps毫秒，用于稳定逻辑帧率控制，绘图带跳帧
void EGEAPI delay_jfps(long fps);   // 平均延迟1000/fps毫秒，用于稳定逻辑帧率控制，绘图带跳帧
void EGEAPI delay_jfps(double fps); // 平均延迟1000/fps毫秒，用于稳定逻辑帧率控制，绘图带跳帧
// 以下函数可以多线程下使用，非图形(worker)线程的sleep使用这个
void EGEAPI api_sleep(long dwMilliseconds);
double EGEAPI fclock(); // 获取以秒为单位的浮点时间，只用于计时用，精度0.01秒，

}

