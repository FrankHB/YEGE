#pragma once

#include "e_windows.h"
#include "e_img.h"

namespace ege
{

// ������غ���
void EGEAPI outtext(LPCSTR  textstring, PIMAGE pimg = NULL);                   // �ڵ�ǰλ���������
void EGEAPI outtext(LPCWSTR textstring, PIMAGE pimg = NULL);                   // �ڵ�ǰλ���������
void EGEAPI outtext(CHAR  c, PIMAGE pimg = NULL);                              // �ڵ�ǰλ������ַ�
void EGEAPI outtext(WCHAR c, PIMAGE pimg = NULL);                              // �ڵ�ǰλ������ַ�
void EGEAPI outtextxy(int x, int y, LPCSTR  textstring, PIMAGE pimg = NULL);   // ��ָ��λ���������
void EGEAPI outtextxy(int x, int y, LPCWSTR textstring, PIMAGE pimg = NULL);   // ��ָ��λ���������
void EGEAPI outtextxy(int x, int y, CHAR c, PIMAGE pimg = NULL);               // ��ָ��λ������ַ�
void EGEAPI outtextxy(int x, int y, WCHAR c, PIMAGE pimg = NULL);              // ��ָ��λ������ַ�
void EGEAPI outtextrect(int x, int y, int w, int h, LPCSTR  textstring, PIMAGE pimg = NULL); // ��ָ�����η�Χ�������
void EGEAPI outtextrect(int x, int y, int w, int h, LPCWSTR textstring, PIMAGE pimg = NULL); // ��ָ�����η�Χ�������
void EGEAPI xyprintf(int x, int y, LPCSTR  fmt, ...); // ��ָ��λ�������ʽ���ַ�����ָ����ͼĿ�����settarget
void EGEAPI xyprintf(int x, int y, LPCWSTR fmt, ...); // ��ָ��λ�������ʽ���ַ�����ָ����ͼĿ�����settarget
void EGEAPI rectprintf(int x, int y, int w, int h, LPCSTR  fmt, ...); // ��ָ�����������ʽ���ַ�����ָ����ͼĿ�����settarget
void EGEAPI rectprintf(int x, int y, int w, int h, LPCWSTR fmt, ...); // ��ָ�����������ʽ���ַ�����ָ����ͼĿ�����settarget
int  EGEAPI textwidth(LPCSTR  textstring, PIMAGE pimg = NULL);                 // ��ȡ�ַ���ռ�õ����ؿ�
int  EGEAPI textwidth(LPCWSTR textstring, PIMAGE pimg = NULL);                 // ��ȡ�ַ���ռ�õ����ؿ�
int  EGEAPI textwidth(CHAR  c, PIMAGE pimg = NULL);
int  EGEAPI textwidth(WCHAR c, PIMAGE pimg = NULL);
int  EGEAPI textheight(LPCSTR  textstring, PIMAGE pimg = NULL);                // ��ȡ�ַ���ռ�õ����ظ�
int  EGEAPI textheight(LPCWSTR textstring, PIMAGE pimg = NULL);                // ��ȡ�ַ���ռ�õ����ظ�
int  EGEAPI textheight(CHAR  c, PIMAGE pimg = NULL);
int  EGEAPI textheight(WCHAR c, PIMAGE pimg = NULL);
void EGEAPI settextjustify(int horiz, int vert, PIMAGE pimg = NULL);

// ���õ�ǰ������ʽ(�������)
//      nHeight: �ַ���ƽ���߶ȣ�
//      nWidth: �ַ���ƽ�����(0 ��ʾ����Ӧ)��
//      lpszFace: �������ƣ�
//      nEscapement: �ַ�������д�Ƕ�(��λ 0.1 ��)��
//      nOrientation: ÿ���ַ�����д�Ƕ�(��λ 0.1 ��)��
//      nWeight: �ַ��ıʻ���ϸ(0 ��ʾĬ�ϴ�ϸ)��
//      bItalic: �Ƿ�б�壻
//      bUnderline: �Ƿ��»��ߣ�
//      bStrikeOut: �Ƿ�ɾ���ߣ�
//      fbCharSet: ָ���ַ�����
//      fbOutPrecision: ָ�����ֵ�������ȣ�
//      fbClipPrecision: ָ�����ֵļ������ȣ�
//      fbQuality: ָ�����ֵ����������
//      fbPitchAndFamily: ָ���Գ��淽ʽ�������������ϵ�С�
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
void EGEAPI setfont(const LOGFONTA *font, PIMAGE pimg = NULL); // ���õ�ǰ������ʽ
void EGEAPI setfont(const LOGFONTW *font, PIMAGE pimg = NULL); // ���õ�ǰ������ʽ
void EGEAPI getfont(LOGFONTA *font, PIMAGE pimg = NULL);       // ��ȡ��ǰ������ʽ
void EGEAPI getfont(LOGFONTW *font, PIMAGE pimg = NULL);       // ��ȡ��ǰ������ʽ

//�߼����뺯��
// title �Ի�����⣬ text �Ի�����ʾ���֣� buf�����������ݵ��ַ���ָ�룬 lenָ��buf����󳤶ȣ�Ҳͬʱ�������������ݳ���
int EGEAPI inputbox_getline(LPCSTR  title, LPCSTR  text, LPSTR  buf, int len);  //�����Ի������û����룬��ǰ����������ͣ�����ط�0��ʾ������Ч��0Ϊ��Ч
int EGEAPI inputbox_getline(LPCWSTR title, LPCWSTR text, LPWSTR buf, int len);  //�����Ի������û����룬��ǰ����������ͣ�����ط�0��ʾ������Ч��0Ϊ��Ч

}

