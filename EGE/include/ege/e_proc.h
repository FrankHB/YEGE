#ifndef Inc_ege_e_proc_h_
#define Inc_ege_e_proc_h_

#include "e_def.h"

namespace ege
{

typedef void (CALLBACK_PROC)();
typedef int (__stdcall MSG_KEY_PROC)(void*, unsigned, int);
typedef int (__stdcall MSG_MOUSE_PROC)(void*, unsigned, int, int, int);
typedef CALLBACK_PROC*        LPCALLBACK_PROC;
typedef MSG_KEY_PROC*         LPMSG_KEY_PROC;
typedef MSG_MOUSE_PROC*       LPMSG_MOUSE_PROC;

/*
callback function define as:
int __stdcall on_msg_key(void* param, unsigned msg, int key);
msg: see 'enum message_event'
key: keycode
return zero means process this message, otherwise means pass it and then process with 'getkey' function
*/
//int message_addkeyhandler(void* param, LPMSG_KEY_PROC func);        //设置键盘回调函数
/*
callback function define as:
int __stdcall on_msg_mouse(void* param, unsigned msg, int key, int x, int y);
msg: see 'enum message_event'
key: see 'enum message_mouse', if msg==MSG_EVENT_WHELL, key is a int number that indicates the distance the wheel is rotated, expressed in multiples or divisions of WHEEL_DELTA, which is 120.
x,y: current mouse (x, y)
return zero means process this message, otherwise means pass it and then process with 'GetMouseMsg' function
*/
//int message_addmousehandler(void* param, LPMSG_MOUSE_PROC func);    //设置鼠标回调函数
int EGEAPI SetCloseHandler(LPCALLBACK_PROC func);

}

#endif

