#pragma once

#include "e_windows.h"
#include "e_img.h"

namespace ege
{

// 文字相关函数
void EGEAPI outtext(LPCSTR  textstring, PIMAGE pimg = NULL);                   // 在当前位置输出文字
void EGEAPI outtext(LPCWSTR textstring, PIMAGE pimg = NULL);                   // 在当前位置输出文字
void EGEAPI outtext(CHAR  c, PIMAGE pimg = NULL);                              // 在当前位置输出字符
void EGEAPI outtext(WCHAR c, PIMAGE pimg = NULL);                              // 在当前位置输出字符
void EGEAPI outtextxy(int x, int y, LPCSTR  textstring, PIMAGE pimg = NULL);   // 在指定位置输出文字
void EGEAPI outtextxy(int x, int y, LPCWSTR textstring, PIMAGE pimg = NULL);   // 在指定位置输出文字
void EGEAPI outtextxy(int x, int y, CHAR c, PIMAGE pimg = NULL);               // 在指定位置输出字符
void EGEAPI outtextxy(int x, int y, WCHAR c, PIMAGE pimg = NULL);              // 在指定位置输出字符
void EGEAPI outtextrect(int x, int y, int w, int h, LPCSTR  textstring, PIMAGE pimg = NULL); // 在指定矩形范围输出文字
void EGEAPI outtextrect(int x, int y, int w, int h, LPCWSTR textstring, PIMAGE pimg = NULL); // 在指定矩形范围输出文字
void EGEAPI xyprintf(int x, int y, LPCSTR  fmt, ...); // 在指定位置输出格式化字符串，指定绘图目标调用settarget
void EGEAPI xyprintf(int x, int y, LPCWSTR fmt, ...); // 在指定位置输出格式化字符串，指定绘图目标调用settarget
void EGEAPI rectprintf(int x, int y, int w, int h, LPCSTR  fmt, ...); // 在指定矩形输出格式化字符串，指定绘图目标调用settarget
void EGEAPI rectprintf(int x, int y, int w, int h, LPCWSTR fmt, ...); // 在指定矩形输出格式化字符串，指定绘图目标调用settarget
int  EGEAPI textwidth(LPCSTR  textstring, PIMAGE pimg = NULL);                 // 获取字符串占用的像素宽
int  EGEAPI textwidth(LPCWSTR textstring, PIMAGE pimg = NULL);                 // 获取字符串占用的像素宽
int  EGEAPI textwidth(CHAR  c, PIMAGE pimg = NULL);
int  EGEAPI textwidth(WCHAR c, PIMAGE pimg = NULL);
int  EGEAPI textheight(LPCSTR  textstring, PIMAGE pimg = NULL);                // 获取字符串占用的像素高
int  EGEAPI textheight(LPCWSTR textstring, PIMAGE pimg = NULL);                // 获取字符串占用的像素高
int  EGEAPI textheight(CHAR  c, PIMAGE pimg = NULL);
int  EGEAPI textheight(WCHAR c, PIMAGE pimg = NULL);
void EGEAPI settextjustify(int horiz, int vert, PIMAGE pimg = NULL);

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
void EGEAPI setfont(int nHeight, int nWidth, LPCSTR lpszFace,  PIMAGE pimg = NULL);
void EGEAPI setfont(int nHeight, int nWidth, LPCWSTR lpszFace, PIMAGE pimg = NULL);
void EGEAPI setfont(int nHeight, int nWidth, LPCSTR lpszFace,  int nEscapement, int nOrientation,
					int nWeight, int bItalic, int bUnderline, int bStrikeOut, PIMAGE pimg = NULL);
void EGEAPI setfont(int nHeight, int nWidth, LPCWSTR lpszFace, int nEscapement, int nOrientation,
					int nWeight, int bItalic, int bUnderline, int bStrikeOut, PIMAGE pimg = NULL);
void EGEAPI setfont(int nHeight, int nWidth, LPCSTR lpszFace,  int nEscapement, int nOrientation,
					int nWeight, int bItalic, int bUnderline, int bStrikeOut, BYTE fbCharSet,
					BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily, PIMAGE pimg = NULL);
void EGEAPI setfont(int nHeight, int nWidth, LPCWSTR lpszFace, int nEscapement, int nOrientation,
					int nWeight, int bItalic, int bUnderline, int bStrikeOut, BYTE fbCharSet,
					BYTE fbOutPrecision, BYTE fbClipPrecision, BYTE fbQuality, BYTE fbPitchAndFamily, PIMAGE pimg = NULL);
void EGEAPI setfont(const LOGFONTA* font, PIMAGE pimg = NULL); // 设置当前字体样式
void EGEAPI setfont(const LOGFONTW* font, PIMAGE pimg = NULL); // 设置当前字体样式
void EGEAPI getfont(LOGFONTA* font, PIMAGE pimg = NULL);       // 获取当前字体样式
void EGEAPI getfont(LOGFONTW* font, PIMAGE pimg = NULL);       // 获取当前字体样式

//高级输入函数
// title 对话框标题，text 对话框提示文字，buf接收输入数据的字符串指针，len指出buf的最大长度，也同时会限制输入内容长度
int EGEAPI inputbox_getline(LPCSTR  title, LPCSTR  text, LPSTR  buf, int len);  //弹出对话框，让用户输入，当前程序运行暂停，返回非0表示输入有效，0为无效
int EGEAPI inputbox_getline(LPCWSTR title, LPCWSTR text, LPWSTR buf, int len);  //弹出对话框，让用户输入，当前程序运行暂停，返回非0表示输入有效，0为无效

}

