﻿#ifndef Inc_ege_compress_h_
#define Inc_ege_compress_h_

#include "ege/def.h"

namespace ege
{

/* 压缩函数 */
/* 压缩时dest缓冲区要保证最小大小为sourceLen * 1.001 + 16 */
/* 调用compress/compress2前，*destLen必须有值，表示dest缓冲区的最大大小，返回时这个值表示实际大小 */
/* compress2 的level 从0-9，0不压缩，9最大压缩，compress函数使用默认值6 */
EGEAPI int
ege_compress(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen);

EGEAPI int
ege_compress2(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen, int level);

EGEAPI int
ege_uncompress(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen);

/* 返回0表示错误，其它表示大小 */
EGEAPI unsigned long
ege_uncompress_size(const void* source, unsigned long sourceLen);

}

#endif

