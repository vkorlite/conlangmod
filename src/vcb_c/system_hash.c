#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

int(*system_hash)(char*,int) = &hash_sdbm;
