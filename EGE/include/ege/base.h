#ifndef Inc_ege_base_h_
#define Inc_ege_base_h_

#include <cmath>
#include <type_traits>
#include <memory>
#include "ege/def.h"
#include "ege/colorbase.h"

namespace ege
{

const double PI = 3.14159265358979323;

using std::size_t;
using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::get_deleter;
using std::make_shared;
using std::owner_less;
using std::shared_ptr;
using std::static_pointer_cast;
using std::unique_ptr;
using std::weak_ptr;

template<typename _type, typename... _tParams>
inline typename
	std::enable_if<!std::is_array<_type>::value, unique_ptr<_type>>::type
make_unique(_tParams&&... args)
{
	return unique_ptr<_type>(new _type(yforward(args)...));
}
template<typename _type, typename... _tParams>
inline typename std::enable_if<std::is_array<_type>::value
	&& std::extent<_type>::value == 0, unique_ptr<_type>>::type
make_unique(size_t size)
{
	return std::unique_ptr<_type>(new
		typename std::remove_extent<_type>::type[size]());
}
template<typename _type,  typename... _tParams>
typename std::enable_if<std::extent<_type>::value != 0>::type
make_unique(_tParams&&...) = delete;


template<typename _type>
yconstfn auto
Deref(_type&& p) -> decltype(*p)
{
	return *std::forward<_type&&>(p);
}


enum graphics_drivers /* define graphics drivers */
{
	DETECT, /* requests autodetection */
	CGA, MCGA, EGA, EGA64, EGAMONO, IBM8514, /* 1 - 6 */
	HERCMONO, ATT400, VGA, PC3270, /* 7 - 10 */
	TRUECOLOR, TRUECOLORSIZE,
	CURRENT_DRIVER = -1
};

enum graphics_errors        /* graphresult error return codes */
{
	grOk                =   0,
	grNoInitGraph       =  -1,
	grNotDetected       =  -2,
	grFileNotFound      =  -3,
	grInvalidDriver     =  -4,
	grNoLoadMem         =  -5,
	grNoScanMem         =  -6,
	grNoFloodMem        =  -7,
	grFontNotFound      =  -8,
	grNoFontMem         =  -9,
	grInvalidMode       = -10,
	grError             = -11,   /* generic error */
	grIOerror           = -12,
	grInvalidFont       = -13,
	grInvalidFontNum    = -14,
	grInvalidVersion    = -18,
	grException         = 0x10,  /* ege error */
	grParamError        = 0x11,
	grInvalidRegion     = 0x12,
	grOutOfMemory       = 0x13,
	grNullPointer       = 0x14,
	grAllocError        = 0x15,
	grInvalidMemory     = 0xCDCDCDCD,
};

enum message_event
{
	MSG_EVENT_UP            = 0x00,
	MSG_EVENT_DOWN          = 0x01,
	MSG_EVENT_CLICK         = 0x01,
	MSG_EVENT_DBCLICK       = 0x02,
	MSG_EVENT_MOVE          = 0x04,
	MSG_EVENT_WHEEL         = 0x10,
};

enum message_mouse
{
	MSG_MOUSE_LEFT      = 0x01,
	MSG_MOUSE_RIGHT     = 0x02,
	MSG_MOUSE_MID       = 0x04,
};

// 颜色
enum COLORS
{
	BLACK           = 0,
	BLUE            = EGERGB(0, 0, 0xA8),
	GREEN           = EGERGB(0, 0xA8, 0),
	CYAN            = EGERGB(0, 0xA8, 0xA8),
	RED             = EGERGB(0xA8, 0, 0),
	MAGENTA         = EGERGB(0xA8, 0, 0xA8),
	BROWN           = EGERGB(0xA8, 0xA8, 0),
	LIGHTGRAY       = EGERGB(0xA8, 0xA8, 0xA8),
	DARKGRAY        = EGERGB(0x54, 0x54, 0x54),
	LIGHTBLUE       = EGERGB(0x54, 0x54, 0xFC),
	LIGHTGREEN      = EGERGB(0x54, 0xFC, 0x54),
	LIGHTCYAN       = EGERGB(0x54, 0xFC, 0xFC),
	LIGHTRED        = EGERGB(0xFC, 0x54, 0x54),
	LIGHTMAGENTA    = EGERGB(0xFC, 0x54, 0xFC),
	YELLOW          = EGERGB(0xFC, 0xFC, 0x54),
	WHITE           = EGERGB(0xFC, 0xFC, 0xFC),
};

// 填充模式
enum fill_patterns  /* Fill patterns for get/setfillstyle */
{
	EMPTY_FILL,     /* fills area in background color */
	SOLID_FILL,     /* fills area in solid fill color */
	LINE_FILL,      /* --- fill */
	LTSLASH_FILL,   /* /// fill */
	SLASH_FILL,     /* /// fill with thick lines */
	BKSLASH_FILL,   /* \\\ fill with thick lines */
	LTBKSLASH_FILL, /* \\\ fill */
	HATCH_FILL,     /* light hatch fill */
	XHATCH_FILL,    /* heavy cross hatch fill */
	INTERLEAVE_FILL,/* interleaving line fill */
	WIDE_DOT_FILL,  /* Widely spaced dot fill */
	CLOSE_DOT_FILL, /* Closely spaced dot fill */
	USER_FILL       /* user defined fill */
};

enum text_just /* Horizontal and vertical justification for settextjustify */
{
	LEFT_TEXT   = 0,
	CENTER_TEXT = 1,
	RIGHT_TEXT  = 2,

	BOTTOM_TEXT = 0,
	/* CENTER_TEXT = 1,  already defined above */
	TOP_TEXT    = 2
};

enum key_msg_flag
{
	KEYMSG_CHAR_FLAG    = 2,
	KEYMSG_DOWN_FLAG    = 1,
	KEYMSG_UP_FLAG      = 1,

	KEYMSG_CHAR         = 0x40000,
	KEYMSG_DOWN         = 0x10000,
	KEYMSG_UP           = 0x20000,
	KEYMSG_FIRSTDOWN    = 0x80000,
};

enum music_state_flag
{
	MUSIC_MODE_NOT_OPEN = 0x0,
	MUSIC_MODE_NOT_READY = 0x20C,
	MUSIC_MODE_PAUSE = 0x211,
	MUSIC_MODE_PLAY = 0x20E,
	MUSIC_MODE_STOP = 0x20D,
	MUSIC_MODE_OPEN = 0x212,
	MUSIC_MODE_SEEK = 0x210,
};


enum pattern_type_e
{
	pattern_none            = 0,
	pattern_lineargradient  = 1,
	pattern_pathgradient    = 2,
	pattern_texture         = 3
};


struct textsettingstype
{
	int font;
	int direction;
	int charsize;
	int horiz;
	int vert;
};


struct linestyletype
{
	int linestyle = 0;
	unsigned short upattern = 0;
	int thickness = 0;

	linestyletype() = default;
	linestyletype(int l, unsigned short u, int t)
		: linestyle(l), upattern(u), thickness(t)
	{}
};


struct ege_point
{
	float x;
	float y;
};


struct ege_rect
{
	float x;
	float y;
	float w;
	float h;
};


class IMAGE;

}

#endif

