#pragma once

#include "e_def.h"

namespace ege
{

// ��ͼ������غ���

void EGEAPI initgraph(int* gdriver, int* gmode, char* path);   // ���� Borland C++ 3.1 �����أ�ֻʹ�� 640x480x24bit
void EGEAPI closegraph();                                      // �ر�ͼ�λ���
bool EGEAPI is_run();   // �ж�UI�Ƿ��˳�

} // namespace ege

