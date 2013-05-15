#include "ege/fpsbase.h"
#include "global.h"

namespace ege
{

float
getfps()
{
	return graph_setting._get_FPS(0);
}

} // namespace ege;

