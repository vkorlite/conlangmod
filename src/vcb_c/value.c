#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif 

char *value_get(st_Value *value){
    return (value->col)? (char*) cond(value->value->cond->str, (st_Object **) value->value->cond->input, value->value->cond->size): value->value->str;
}
