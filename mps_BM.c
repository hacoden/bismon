// file mps_BM.c
#include "bismon.h"

/// GTK headers are defining TRUE & FALSE which is also inside some
/// enum in mps code
#undef TRUE
#undef FALSE

#include "mps/code/mpscamc.h"
#include "mps/code/mpsavm.h"

#include "mps/code/mps.c"

int mpsarenasize_BM;            /* in megabytes */
pthread_t mainthreadid_BM;

mps_arena_t mpsarena_BM;
mps_fmt_t mpsfmt_BM;
atomic_bool want_garbage_collection_BM;


static mps_res_t
mpsval_scan_BM (mps_ss_t ss, mps_addr_t base, mps_addr_t limit)
{
  MPS_SCAN_BEGIN (ss)
  {
    while (base < limit)
      {
        extendedval_tyBM curval = (extendedval_tyBM) base;
        int curty = valtype_BM (curval);
        switch (curty)
          {
#warning mpsval_scan_BM switch incomplete
          default:
            FATAL_BM ("mpsval_scan_BM: invalid curty#%d for curval@%p", curty,
                      curval);
          }
      };
  }
  MPS_SCAN_END (ss);
  return MPS_RES_OK;
}                               /* end mpsval_scan_BM */

static mps_addr_t
mpsval_skip_BM (mps_addr_t base)
{
  extendedval_tyBM curval = (extendedval_tyBM) base;
  int curty = valtype_BM (curval);
  switch (curty)
    {
#warning mpsval_skip_BM switch incomplete
    default:
      FATAL_BM ("mpsval_skip_BM: invalid curty#%d for curval@%p",
                curty, curval);
    }
}                               /* end mpsval_skip_BM */

static void
mpsval_fwd_BM (mps_addr_t old, mps_addr_t new)
{
  extendedval_tyBM oldval = (extendedval_tyBM) old;
  int oldty = valtype_BM (oldval);
  switch (oldty)
    {
#warning mpsval_fwd_BM switch incomplete
    default:
      FATAL_BM ("mpsval_fwd_BM: invalid oldty#%d for oldval@%p",
                oldty, oldval);
    }
}                               /* end mpsval_fwd_BM */

static mps_addr_t
mpsval_isfwd_BM (mps_addr_t addr)
{
  extendedval_tyBM curval = (extendedval_tyBM) addr;
  int curty = valtype_BM (curval);
  switch (curty)
    {
#warning mpsval_isfwd_BM switch incomplete
    default:
      FATAL_BM ("mpsval_isfwd_BM: invalid curty#%d for curval@%p",
                curty, curval);
    }
}                               /* end mpsval_isfwd_BM */

static void
mpsval_pad_BM (mps_addr_t addr, size_t size)
{
#warning mpsval_pad_BM incomplete
}                               /* end mpsval_pad_BM */


mpsinit_BM (const char *argsmps)
{
  mainthreadid_BM = pthread_self ();
  if (argsmps)
    {
      const char *p = argsmps;
      while (p && *p)
        {
          int w = 0;
          while (*p == ',' || isspace (*p))
            p++;
          if (sscanf (p, "arenasize=%d%n", &mpsarenasize_BM, &w) > 0)
            p += w;
          else
            FATAL_BM ("bad %s for MPS in %s", p, argsmps);
        };
    }
  if (mpsarenasize_BM <= 16)
    mpsarenasize_BM = 16;
  if (mpsarenasize_BM > 1024 * 1024)
    mpsarenasize_BM = 1024 * 1024;
  mps_res_t res = 0;
  MPS_ARGS_BEGIN (args)
  {
    MPS_ARGS_ADD (args, MPS_KEY_ARENA_SIZE, mpsarenasize_BM * 1024 * 1024);
    res = mps_arena_create_k (&mpsarena_BM, mps_arena_class_vm (), args);
  }
  MPS_ARGS_END (args);
  if (res != MPS_RES_OK)
    FATAL_BM ("Couldn't create arena of %d megabytes", mpsarenasize_BM);
  MPS_ARGS_BEGIN (args)
  {
    MPS_ARGS_ADD (args, MPS_KEY_FMT_ALIGN, VALALIGN_BM);
    MPS_ARGS_ADD (args, MPS_KEY_FMT_SCAN, mpsval_scan_BM);
    MPS_ARGS_ADD (args, MPS_KEY_FMT_SKIP, mpsval_skip_BM);
    MPS_ARGS_ADD (args, MPS_KEY_FMT_FWD, mpsval_fwd_BM);
    MPS_ARGS_ADD (args, MPS_KEY_FMT_ISFWD, mpsval_isfwd_BM);
    MPS_ARGS_ADD (args, MPS_KEY_FMT_PAD, mpsval_pad_BM);
    res = mps_fmt_create_k (&mpsfmt_BM, mpsarena_BM, args);
  }
  MPS_ARGS_END (args);
  if (res != MPS_RES_OK)
    FATAL_BM ("Could not create MPS value format");
#warning should code mpsinit_BM
}                               /* end mpsinit_BM */

void
gcobjmark_BM (struct garbcoll_stBM *gc, objectval_tyBM * obj)
{
  FATAL_BM ("obsolete gcobjmark_BM obj@%p", obj);
#warning obsolete gcobjmark_BM
}                               /* end gcobjmark_BM */

void
gcvaluemark_BM (struct garbcoll_stBM *gc, value_tyBM val, int depth)
{
  FATAL_BM ("obsolete gcvaluemark_BM val@%p", val);
#warning obsolete gcvaluemark_BM
}

void
gcextendedmark_BM (struct garbcoll_stBM *gc, extendedval_tyBM xval, int depth)
{
  FATAL_BM ("obsolete gcextendedmark_BM xval@%p", xval);
#warning obsolete gcextendedmark_BM
}


void
deleteobjectpayload_BM (objectval_tyBM * obj, extendedval_tyBM payl)
{
  FATAL_BM ("obsolete deleteobjectpayload_BM obj@%p payl@%p", obj, payl);
#warning obsolete deleteobjectpayload_BM
}

void *
allocgcty_BM (unsigned type, size_t sz)
{
  FATAL_BM ("obsolete allocgcty_BM type%u sz %zu", type, sz);
#warning obsolete allocgcty_BM
}

void
fullgarbagecollection_BM (struct stackframe_stBM *stkfram)
{
  FATAL_BM ("obsolete fullgarbagecollection_BM stkfram@%p", stkfram);
#warning obsolete fullgarbagecollection_BM
}
