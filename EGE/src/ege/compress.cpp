﻿#include "ege/compress.h"
#include "../zlib/zlib.h"

namespace ege
{

int
ege_compress(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen)
{
	if(sourceLen == 0)
		return -1;
	int ret = compress((Bytef*)dest + sizeof(unsigned long),
		(uLongf*)destLen, (Bytef*)source, (uLong)sourceLen);
	((unsigned long*)dest)[0] = sourceLen;
	*destLen += 4;
	return ret;
}

int
ege_compress2(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen, int level)
{
	if(sourceLen == 0)
		return -1;
	int ret = compress2((Bytef*)dest + sizeof(unsigned long),
		(uLongf*)destLen, (Bytef*)source, (uLong)sourceLen, level);
	*(unsigned long*)dest = sourceLen;
	*destLen += sizeof(unsigned long);
	return ret;
}

unsigned long
ege_uncompress_size(const void* source, unsigned long sourceLen)
{
	return sourceLen > sizeof(unsigned long) ? ((unsigned long*)source)[0] : 0;
}

int
ege_uncompress(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen)
{
	*(uLongf*)destLen = ege_uncompress_size(source, sourceLen);
	return *(uLongf*)destLen > 0 ? uncompress((Bytef*)dest, (uLongf*)destLen,
		(Bytef*)source + sizeof(unsigned long),
		(uLong)sourceLen - sizeof(unsigned long)) : -1;
}

} // namespace ege;

