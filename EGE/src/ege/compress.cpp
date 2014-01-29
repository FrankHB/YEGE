#include "ege/compress.h"
#include "../zlib/zlib.h"

namespace ege
{

int
ege_compress(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen)
{
	if(sourceLen == 0)
		return -1;
	int ret = compress(static_cast<Bytef*>(dest) + sizeof(unsigned long),
		destLen, static_cast<const Bytef*>(source), sourceLen);
	static_cast<unsigned long*>(dest)[0] = sourceLen;
	*destLen += 4;
	return ret;
}

int
ege_compress2(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen, int level)
{
	if(sourceLen == 0)
		return -1;
	int ret = compress2(static_cast<Bytef*>(dest) + sizeof(unsigned long),
		destLen, static_cast<const Bytef*>(source), sourceLen, level);
	*static_cast<unsigned long*>(dest) = sourceLen;
	*destLen += sizeof(unsigned long);
	return ret;
}

unsigned long
ege_uncompress_size(const void* source, unsigned long sourceLen)
{
	return sourceLen > sizeof(unsigned long)
		? static_cast<const unsigned long*>(source)[0] : 0;
}

int
ege_uncompress(void* dest, unsigned long* destLen, const void* source,
	unsigned long sourceLen)
{
	*destLen = ege_uncompress_size(source, sourceLen);
	return *destLen > 0 ? uncompress(static_cast<Bytef*>(dest), destLen,
		static_cast<const Bytef*>(source) + sizeof(unsigned long),
		sourceLen - sizeof(unsigned long)) : -1;
}

} // namespace ege;

