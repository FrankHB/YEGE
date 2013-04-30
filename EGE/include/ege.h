#ifndef INC_ege_h_
#define INC_ege_h_

#include "ege/e_def.h"
#include "ege/e_color.h"
#include "ege/e_proc.h"
#include "ege/e_v3d.h"
#include "ege/e_env.h"
#include "ege/e_gdi.h"
#include "ege/e_time.h"
#include "ege/e_text.h"
#include "ege/e_pushtarget.h"
#include "ege/e_ctl.h"
#include "ege/e_input.h"
#include "ege/e_music.h"
#include "ege/e_compress.h"
#include "ege/e_random.h"
#include "ege/e_fps.h"

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

#endif

