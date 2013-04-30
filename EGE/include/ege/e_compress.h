#pragma once

namespace ege
{

/* ѹ������ */
/* ѹ��ʱdest������Ҫ��֤��С��СΪsourceLen * 1.001 + 16 */
/* ����compress/compress2ǰ��*destLen������ֵ����ʾdest������������С������ʱ���ֵ��ʾʵ�ʴ�С */
/* compress2 ��level ��0-9��0��ѹ����9���ѹ����compress����ʹ��Ĭ��ֵ6 */
int             EGEAPI ege_compress(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen);
int             EGEAPI ege_compress2(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen, int level);
int             EGEAPI ege_uncompress(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen);
unsigned long   EGEAPI ege_uncompress_size(const void* source, unsigned long sourceLen); /* ����0��ʾ����������ʾ��С */

}

