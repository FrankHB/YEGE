#ifndef INC_ege_h_
#define INC_ege_h_

#include "ege/def.h"
#include "ege/mac.h"
#include "ege/base.h"
#include "ege/colorbase.h"
#include "ege/color.h"
#include "ege/proc.h"
#include "ege/v3d.h"
#include "ege/env.h"
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

#if 0
#include "ege/button.h"
#include "ege/fps.h"
#include "ege/label.h"
#include "ege/sys_edit.h"
#endif

namespace ege
{

EGEAPI void
setactivepage(int);

EGEAPI void
setvisualpage(int);

}

#endif

