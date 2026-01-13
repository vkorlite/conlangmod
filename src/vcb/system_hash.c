#ifndef __VCB_INCL
#include "vcb/vcb.h"
#endif

int(*system_hash)(char*,int) = &hash_sdbm;
st_HashTable *sys_hash_init(){return hash_init(system_hash);}
