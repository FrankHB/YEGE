#pragma once

#include <wingdi.h>
#include "e_base.h"
#include "e_mac.h"
#include "e_img.h"

namespace ege
{

//���̴�����
int     EGEAPI kbmsg();
key_msg EGEAPI getkey();
EGE_DEPRECATE(getchEx)
int     EGEAPI getchEx(int flag);
EGE_DEPRECATE(kbhitEx)
int     EGEAPI kbhitEx(int flag);
int     EGEAPI keystate(int key);       // ��ü���Ϊkey�ļ�����key_code_e���Ƿ��£����keyʹ����갴���ļ��룬���õ�������״̬
void    EGEAPI flushkey();              // ��ռ�����Ϣ������

//#ifndef _GRAPH_LIB_BUILD_
#if !defined(_INC_CONIO) && !defined(_CONIO_H_)
#define _INC_CONIO
#define _CONIO_H_
int EGEAPI getch();
int EGEAPI kbhit();
#else
#define getch getchEx
#define kbhit kbhitEx
#endif
//#endif

//��괦����
int         EGEAPI mousemsg();                  // ����Ƿ���������Ϣ
mouse_msg   EGEAPI getmouse();                  // ��ȡһ�������Ϣ�����û�У��͵ȴ�
#if 0
EGE_DEPRECATE(GetMouseMsg)
MOUSEMSG    EGEAPI GetMouseMsg();               // �����Ƽ�ʹ�õĺ�������ȡһ�������Ϣ�����û�У��͵ȴ�
#endif

void        EGEAPI flushmouse();                // ��������Ϣ������
int         EGEAPI showmouse(int bShow);        // �����Ƿ���ʾ���
int         EGEAPI mousepos(int *x, int *y); // ��ȡ��ǰ���λ��



}

