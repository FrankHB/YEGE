#pragma once

#if 1
#include "e_def.h"
#include "e_color.h"
#include "e_proc.h"
#include "e_v3d.h"
#include "e_env.h"
#include "e_gdi.h"
#include "e_time.h"
#include "e_text.h"
#include "e_pushtarget.h"
#include "e_ctl.h"
#include "e_input.h"
#include "e_music.h"
#include "e_compress.h"
#include "e_random.h"
#include "e_fps.h"
#endif

namespace ege
{

EGEAPI void
setactivepage(int);
EGEAPI void
setvisualpage(int);

}

#ifndef _GRAPH_LIB_BUILD_
#define Sleep(ms) delay_ms(ms)
#endif

