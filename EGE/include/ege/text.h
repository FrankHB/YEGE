#ifndef Inc_ege_text_h_
#define Inc_ege_text_h_

#include "ege/def.h"
#include <windef.h> // for ::POINT used by <wingdi.h>
// NOTE: Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=97362.
#undef __deref
#include <wingdi.h> // for ::LOGFONTA, ::LOGFONTW;

namespace ege
{

class IMAGE;

// 文字相关函数
EGEAPI void
outtext(const char* textstring, IMAGE* pimg = {});                   // 在当前位置输出文字
EGEAPI void
outtext(const wchar_t* textstring, IMAGE* pimg = {});                   // 在当前位置输出文字
EGEAPI void
outtext(char c, IMAGE* pimg = {});                              // 在当前位置输出字符
EGEAPI void
outtext(wchar_t c, IMAGE* pimg = {});                              // 在当前位置输出字符

EGEAPI void
outtextxy(int x, int y, const char*  textstring, IMAGE* pimg = {});   // 在指定位置输出文字
EGEAPI void
outtextxy(int x, int y, const wchar_t* textstring, IMAGE* pimg = {});   // 在指定位置输出文字
EGEAPI void
outtextxy(int x, int y, char c, IMAGE* pimg = {});               // 在指定位置输出字符
EGEAPI void
outtextxy(int x, int y, wchar_t c, IMAGE* pimg = {});              // 在指定位置输出字符

EGEAPI void
outtextrect(int x, int y, int w, int h, const char*  textstring, IMAGE* pimg = {}); // 在指定矩形范围输出文字
EGEAPI void
outtextrect(int x, int y, int w, int h, const wchar_t* textstring, IMAGE* pimg = {}); // 在指定矩形范围输出文字

EGEAPI void
xyprintf(int x, int y, const char*  fmt, ...); // 在指定位置输出格式化字符串，指定绘图目标调用settarget
EGEAPI void
xyprintf(int x, int y, const wchar_t* fmt, ...); // 在指定位置输出格式化字符串，指定绘图目标调用settarget

EGEAPI void rectprintf(int x, int y, int w, int h, const char*  fmt, ...); // 在指定矩形输出格式化字符串，指定绘图目标调用settarget
EGEAPI void rectprintf(int x, int y, int w, int h, const wchar_t* fmt, ...); // 在指定矩形输出格式化字符串，指定绘图目标调用settarget

EGEAPI int
textwidth(const char*  textstring, IMAGE* pimg = {}); // 获取字符串占用的像素宽
EGEAPI int
textwidth(const wchar_t* textstring, IMAGE* pimg = {}); // 获取字符串占用的像素宽
EGEAPI int
textwidth(char c, IMAGE* pimg = {});
EGEAPI int
textwidth(wchar_t c, IMAGE* pimg = {});

EGEAPI int
textheight(const char*  textstring, IMAGE* pimg = {}); // 获取字符串占用的像素高
EGEAPI int
textheight(const wchar_t* textstring, IMAGE* pimg = {}); // 获取字符串占用的像素高
EGEAPI int
textheight(char  c, IMAGE* pimg = {});
EGEAPI int
textheight(wchar_t c, IMAGE* pimg = {});

EGEAPI void
settextjustify(int horiz, int vert, IMAGE* pimg = {});


// 设置当前字体样式(详见帮助)
//      nHeight: 字符的平均高度；
//      nWidth: 字符的平均宽度(0 表示自适应)；
//      lpszFace: 字体名称；
//      nEscapement: 字符串的书写角度(单位 0.1 度)；
//      nOrientation: 每个字符的书写角度(单位 0.1 度)；
//      nWeight: 字符的笔画粗细(0 表示默认粗细)；
//      bItalic: 是否斜体；
//      bUnderline: 是否下划线；
//      bStrikeOut: 是否删除线；
//      fbCharSet: 指定字符集；
//      fbOutPrecision: 指定文字的输出精度；
//      fbClipPrecision: 指定文字的剪辑精度；
//      fbQuality: 指定文字的输出质量；
//      fbPitchAndFamily: 指定以常规方式描述字体的字体系列。
EGEAPI void
setfont(int nHeight, int nWidth, const char* lpszFace,  IMAGE* pimg = {});
EGEAPI void
setfont(int nHeight, int nWidth, const wchar_t* lpszFace, IMAGE* pimg = {});
EGEAPI void
setfont(int nHeight, int nWidth, const char* lpszFace,  int nEscapement, int nOrientation,
	int nWeight, int bItalic, int bUnderline, int bStrikeOut, IMAGE* pimg = {});
EGEAPI void
setfont(int nHeight, int nWidth, const wchar_t* lpszFace, int nEscapement, int nOrientation,
	int nWeight, int bItalic, int bUnderline, int bStrikeOut, IMAGE* pimg = {});
EGEAPI void setfont(int nHeight, int nWidth, const char* lpszFace,  int nEscapement, int nOrientation,
	int nWeight, int bItalic, int bUnderline, int bStrikeOut, ::BYTE fbCharSet,
	::BYTE fbOutPrecision, ::BYTE fbClipPrecision, ::BYTE fbQuality, ::BYTE fbPitchAndFamily, IMAGE* pimg = {});
EGEAPI void setfont(int nHeight, int nWidth, const wchar_t* lpszFace, int nEscapement, int nOrientation,
	int nWeight, int bItalic, int bUnderline, int bStrikeOut, ::BYTE fbCharSet,
	::BYTE fbOutPrecision, ::BYTE fbClipPrecision, ::BYTE fbQuality, ::BYTE fbPitchAndFamily, IMAGE* pimg = {});

EGEAPI void
setfont(const ::LOGFONTA* font, IMAGE* pimg = {}); // 设置当前字体样式
EGEAPI void
setfont(const ::LOGFONTW* font, IMAGE* pimg = {}); // 设置当前字体样式

EGEAPI void
getfont(::LOGFONTA* font, const IMAGE* pimg = {});       // 获取当前字体样式
EGEAPI void
getfont(::LOGFONTW* font, const IMAGE* pimg = {});       // 获取当前字体样式


//高级输入函数
// title 对话框标题，text 对话框提示文字，buf接收输入数据的字符串指针，len指出buf的最大长度，也同时会限制输入内容长度
EGEAPI int
inputbox_getline(const char* title, const char*  text, char* buf, int len);  //弹出对话框，让用户输入，当前程序运行暂停，返回非0表示输入有效，0为无效
EGEAPI int
inputbox_getline(const wchar_t* title, const wchar_t* text, wchar_t* buf, int len);  //弹出对话框，让用户输入，当前程序运行暂停，返回非0表示输入有效，0为无效

}

#endif

