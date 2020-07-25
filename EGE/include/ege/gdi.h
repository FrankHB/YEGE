#ifndef Inc_ege_gdi_h_
#define Inc_ege_gdi_h_

#include "ege/base.h"
#include <WinDef.h>
#include <Wingdi.h>

namespace ege
{

// 从图片文件获取图像(bmp/jpg/gif/emf/wmf)
EGEAPI int
getimage(IMAGE*, const char*, int = 0, int = 0);
// 从图片文件获取图像(bmp/jpg/gif/emf/wmf)
EGEAPI int
getimage(IMAGE*, const wchar_t*, int = 0, int = 0);
// 从资源文件获取图像(bmp/jpg/gif/emf/wmf)
EGEAPI int
getimage(IMAGE*, const char*, const char*, int = 0, int = 0);
// 从资源文件获取图像(bmp/jpg/gif/emf/wmf)
EGEAPI int
getimage(IMAGE*, const wchar_t*, const wchar_t*, int = 0, int = 0);

// 绘制图像到屏幕
EGEAPI void
putimage(int, int, const IMAGE*, unsigned long = SRCCOPY);
// 绘制图像到屏幕(指定宽高)
EGEAPI void
putimage(int, int, int, int, const IMAGE*, int, int, unsigned long = SRCCOPY);
// 绘制图像到屏幕(指定源宽高和目标宽高进行拉伸)
EGEAPI void
putimage(int, int, int, int, const IMAGE*, int, int, int, int,
	unsigned long = SRCCOPY);
// 绘制图像到另一图像中
EGEAPI void
putimage(IMAGE*, int, int, const IMAGE*, unsigned long = SRCCOPY);
// 绘制图像到另一图像中(指定宽高)
EGEAPI void
putimage(IMAGE*, int, int, int, int, const IMAGE*, int, int,
	unsigned long = SRCCOPY);
// 绘制图像到另一图像中(指定源宽高和目标宽高进行拉伸)
EGEAPI void
putimage(IMAGE*, int, int, int, int, const IMAGE*, int, int, int, int,
	unsigned long = SRCCOPY);

EGEAPI int
saveimage(const IMAGE*, const char*);
EGEAPI int
saveimage(const IMAGE*, const wchar_t*);

EGEAPI int
savepng(const IMAGE*, const char*, int = 0);
EGEAPI int
savepng(const IMAGE*, const wchar_t*, int = 0);

EGEAPI int
getimage_pngfile(IMAGE*, const char*);
EGEAPI int
getimage_pngfile(IMAGE*, const wchar_t*);

}

#endif

