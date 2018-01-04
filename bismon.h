// file bismon.h
#ifndef BISMON_INCLUDED
#define BISMON_INCLUDED

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stddef.h>
#include <stdint.h>

#if __cplusplus
using std::atomic_bool;
using std::atomic_load;
using std::atomic_store;
#else
// notice that <stdatomic.h> is not C++11 compatible
#include <stdatomic.h>
/// Glibc dont have yet threads.h
#define thread_local _Thread_local
#endif


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <setjmp.h>
#include <math.h>
#include <dlfcn.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <execinfo.h>


#include <gtk/gtk.h>

#include "cmacros_BM.h"
#include "types_BM.h"
#include "globals_BM.h"
#include "fundecl_BM.h"
#include "inline_BM.h"

#endif /*BISMON_INCLUDED*/
