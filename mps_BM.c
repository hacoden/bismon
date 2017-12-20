// file mps_BM.c
#include "bismon.h"

/// GTK headers are defining TRUE & FALSE which is also inside some
/// enum in mps code
#undef TRUE
#undef FALSE

#include "mps/code/mps.c"
#include "mps/code/mpscamc.h"

void
mpsinit_BM (void)
{
#warning should code mpsinit_BM
}                               /* end mpsinit_BM */
