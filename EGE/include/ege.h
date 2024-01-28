#ifndef INC_ege_h_
#define INC_ege_h_

#include "ege/base.h"
#include "ege/color.h"
#include "ege/v3d.h"
#include "ege/env.h"
#include "ege/viewport.h"
#include "ege/gapi.h"
#include "ege/gapi_aa.h"
#include "ege/img.h"
#include "ege/gdi.h"
#include "ege/time.h"
#include "ege/text.h"
#include "ege/pushtarget.h"
#include "ege/ctl.h"
#include "ege/input.h"
#include "ege/music.h"
#include "ege/compress.h"
#include "ege/windows.h"
#include "ege/random.h"
#include "ege/fpsbase.h"

#if false
#include "ege/button.h"
#include "ege/fps.h"
#include "ege/label.h"
#include "ege/sys_edit.h"
#endif

#define EGE_VERSION "24.01"
#define EGE_VERSION_W L"24.01"
#define EGE_VERSION_MAJOR 24
#define EGE_VERSION_MINOR 1
#define EGE_VERSION_PATCH 0
#define EGE_MAKE_VERSION_NUMBER(major, minor, patch) \
	((major) * 10000L + (minor) * 100L + (patch))
#define EGE_VERSION_NUMBER EGE_MAKE_VERSION_NUMBER(EGE_VERSION_MAJOR, \
	EGE_VERSION_MINOR, EGE_VERSION_PATCH)

namespace ege
{

EGEAPI void
setactivepage(int);

EGEAPI void
setvisualpage(int);

}

#endif

