#pragma once

#include "e_base.h"

namespace ege
{

//ʱ�亯�������º��������ڶ��߳���ʹ�ã�ֻ�ܸ���ͼ���̵߳��ã�
void EGEAPI ege_sleep(long ms);     // �����ӳ�ms����
void EGEAPI delay(long ms);         // �����ӳ�ms����
void EGEAPI delay_ms(long ms);      // ƽ���ӳ�ms����
void EGEAPI delay_fps(int fps);    // ƽ���ӳ�1000/fps���룬�����ȶ�֡�ʿ���
void EGEAPI delay_fps(long fps);    // ƽ���ӳ�1000/fps���룬�����ȶ�֡�ʿ���
void EGEAPI delay_fps(double fps);  // ƽ���ӳ�1000/fps���룬�����ȶ�֡�ʿ���
void EGEAPI delay_jfps(int fps);   // ƽ���ӳ�1000/fps���룬�����ȶ��߼�֡�ʿ��ƣ���ͼ����֡
void EGEAPI delay_jfps(long fps);   // ƽ���ӳ�1000/fps���룬�����ȶ��߼�֡�ʿ��ƣ���ͼ����֡
void EGEAPI delay_jfps(double fps); // ƽ���ӳ�1000/fps���룬�����ȶ��߼�֡�ʿ��ƣ���ͼ����֡
// ���º������Զ��߳���ʹ�ã���ͼ��(worker)�̵߳�sleepʹ�����
void EGEAPI api_sleep(long dwMilliseconds);
double EGEAPI fclock(); // ��ȡ����Ϊ��λ�ĸ���ʱ�䣬ֻ���ڼ�ʱ�ã�����0.01�룬

}

