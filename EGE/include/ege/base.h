#ifndef Inc_ege_base_h_
#define Inc_ege_base_h_

#include <cmath>
#include "ege/def.h"
#include "ege/colorbase.h"

namespace ege
{

const double PI = 3.14159265358979323;

enum graphics_drivers /* define graphics drivers */
{
	DETECT, /* requests autodetection */
	CGA, MCGA, EGA, EGA64, EGAMONO, IBM8514, /* 1 - 6 */
	HERCMONO, ATT400, VGA, PC3270, /* 7 - 10 */
	TRUECOLOR, TRUECOLORSIZE,
	CURRENT_DRIVER = -1
};

enum graphics_modes   /* graphics modes for each driver */
{
	CGAC0       = 0,  /* 320x200 palette 0; 1 page  */
	CGAC1       = 1,  /* 320x200 palette 1; 1 page  */
	CGAC2       = 2,  /* 320x200 palette 2: 1 page  */
	CGAC3       = 3,  /* 320x200 palette 3; 1 page  */
	CGAHI       = 4,  /* 640x200 1 page             */
	MCGAC0      = 0,  /* 320x200 palette 0; 1 page  */
	MCGAC1      = 1,  /* 320x200 palette 1; 1 page  */
	MCGAC2      = 2,  /* 320x200 palette 2; 1 page  */
	MCGAC3      = 3,  /* 320x200 palette 3; 1 page  */
	MCGAMED     = 4,  /* 640x200 1 page             */
	MCGAHI      = 5,  /* 640x480 1 page             */
	EGALO       = 0,  /* 640x200 16 color 4 pages   */
	EGAHI       = 1,  /* 640x350 16 color 2 pages   */
	EGA64LO     = 0,  /* 640x200 16 color 1 page    */
	EGA64HI     = 1,  /* 640x350 4 color  1 page    */
	EGAMONOHI   = 0,  /* 640x350 64K on card, 1 page - 256K on card, 4 pages */
	HERCMONOHI  = 0,  /* 720x348 2 pages            */
	ATT400C0    = 0,  /* 320x200 palette 0; 1 page  */
	ATT400C1    = 1,  /* 320x200 palette 1; 1 page  */
	ATT400C2    = 2,  /* 320x200 palette 2; 1 page  */
	ATT400C3    = 3,  /* 320x200 palette 3; 1 page  */
	ATT400MED   = 4,  /* 640x200 1 page             */
	ATT400HI    = 5,  /* 640x400 1 page             */
	VGALO       = 0,  /* 640x200 16 color 4 pages   */
	VGAMED      = 1,  /* 640x350 16 color 2 pages   */
	VGAHI       = 2,  /* 640x480 16 color 1 page    */
	PC3270HI    = 0,  /* 720x350 1 page             */
	IBM8514LO   = 0,  /* 640x480 256 colors         */
	IBM8514HI   = 1   /*1024x768 256 colors         */
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


enum rendermode_e
{
	RENDER_AUTO,
	RENDER_MANUAL,
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


EGEAPI void setrendermode(rendermode_e mode);

}

#endif

