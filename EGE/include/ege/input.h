#ifndef Inc_ege_input_h_
#define Inc_ege_input_h_

#include "ege/def.h"
#include <windows.h>

namespace ege
{

enum key_msg_e
{
	key_msg_none = 0,
	key_msg_down = 1,
	key_msg_up = 2,
	key_msg_char = 4,
};

enum key_flag_e
{
	key_flag_shift = 0x100,
	key_flag_ctrl = 0x200
};

enum mouse_msg_e
{
	mouse_msg_none = 0,
	mouse_msg_down = 0x10,
	mouse_msg_up = 0x20,
	mouse_msg_move = 0x40,
	mouse_msg_wheel = 0x80
};

enum mouse_flag_e
{
	mouse_flag_left = 1,
	mouse_flag_right = 2,
	mouse_flag_mid = 4,
	mouse_flag_shift = 0x100,
	mouse_flag_ctrl = 0x200
};


struct key_msg
{
	int key;
	key_msg_e msg;
	unsigned int flags;
};

struct mouse_msg
{
	int x;
	int y;
	mouse_msg_e msg;
	unsigned int flags;
	int wheel;
	bool is_left()
	{
		return (flags & mouse_flag_left) != 0;
	}
	bool is_right()
	{
		return (flags & mouse_flag_right) != 0;
	}
	bool is_mid()
	{
		return (flags & mouse_flag_mid) != 0;
	}
	bool is_down()
	{
		return msg == mouse_msg_down;
	}
	bool is_up()
	{
		return msg == mouse_msg_up;
	}
	bool is_move()
	{
		return msg == mouse_msg_move;
	}
	bool is_wheel()
	{
		return msg == mouse_msg_wheel;
	}
};


enum key_code_e
{
	key_mouse_l     = 0x01,
	key_mouse_r     = 0x02,
	key_mouse_m     = 0x04,
	key_back        = 0x08,
	key_tab         = 0x09,
	key_enter       = 0x0d,
	key_shift       = 0x10,
	key_control     = 0x11,
	key_menu        = 0x12,
	key_pause       = 0x13,
	key_capslock    = 0x14,
	key_esc         = 0x1b,
	key_space       = 0x20,

	key_pageup      = 0x21,
	key_pagedown    = 0x22,
	key_home        = 0x23,
	key_end         = 0x24,

	key_left        = 0x25,
	key_up          = 0x26,
	key_right       = 0x27,
	key_down        = 0x28,

	key_print       = 0x2a,
	key_snapshot    = 0x2c,
	key_insert      = 0x2d,
	key_delete      = 0x2e,

	key_0           = 0x30,
	key_1           = 0x31,
	key_2           = 0x32,
	key_3           = 0x33,
	key_4           = 0x34,
	key_5           = 0x35,
	key_6           = 0x36,
	key_7           = 0x37,
	key_8           = 0x38,
	key_9           = 0x39,

	key_A           = 0x41,
	key_Z           = 0x5a,
	key_win_l       = 0x5b,
	key_win_r       = 0x5c,

	key_sleep       = 0x5f,

	key_num0        = 0x60,
	key_num1        = 0x61,
	key_num2        = 0x62,
	key_num3        = 0x63,
	key_num4        = 0x64,
	key_num5        = 0x65,
	key_num6        = 0x66,
	key_num7        = 0x67,
	key_num8        = 0x68,
	key_num9        = 0x69,

	key_f1          = 0x70,
	key_f2          = 0x71,
	key_f3          = 0x72,
	key_f4          = 0x73,
	key_f5          = 0x74,
	key_f6          = 0x75,
	key_f7          = 0x76,
	key_f8          = 0x77,
	key_f9          = 0x78,
	key_f10         = 0x79,
	key_f11         = 0x7a,
	key_f12         = 0x7b,

	key_numlock     = 0x90,
	key_scrolllock  = 0x91,

	key_shift_l     = 0xa0,
	key_shift_r     = 0xa1,
	key_control_l   = 0xa2,
	key_control_r   = 0xa3,
	key_menu_l      = 0xa4,
	key_menu_r      = 0xa5,

	key_semicolon   = 0xba,
	key_plus        = 0xbb,
	key_comma       = 0xbc,
	key_minus       = 0xbd,
	key_period      = 0xbe,
	key_slash       = 0xbf,
	key_tilde       = 0xc0,
	key_lbrace      = 0xdb,
	key_backslash   = 0xdc,
	key_rbrace      = 0xdd,
	key_quote       = 0xde,

	key_ime_process = 0xe5,
};


//键盘处理函数
EGEAPI int
kbmsg();

EGEAPI key_msg
getkey();

EGEAPI int
keystate(int key);       // 获得键码为key的键（见key_code_e）是否按下，如果key使用鼠标按键的键码，则获得的是鼠标键状态

EGEAPI void
flushkey();              // 清空键盘消息缓冲区

EGEAPI int
getch();

EGEAPI int
kbhit();


//鼠标处理函数
EGEAPI int
mousemsg();                  // 检查是否存在鼠标消息

EGEAPI mouse_msg
getmouse();                  // 获取一个鼠标消息。如果没有，就等待

EGEAPI void
flushmouse();                // 清空鼠标消息缓冲区

EGEAPI bool
showmouse(bool bShow);        // 设置是否显示鼠标

EGEAPI int
mousepos(int* x, int* y); // 获取当前鼠标位置

}

#endif

