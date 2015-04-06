#ifndef Inc_ege_head_h_
#define Inc_ege_head_h_

#include "ege/base.h"

#ifndef DEFAULT_CHARSET
#define DEFAULT_CHARSET ANSI_CHARSET
#endif

namespace ege
{

#if YEGE_Use_YSLib
using namespace YSLib::Drawing;
using namespace YSLib::UI;
#endif

} // namespace ege;

#endif

