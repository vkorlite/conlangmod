#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif 

st_Value *value_init(st_Arena *arena, char *str, char col, void *input, size_t size){
    if(arena == NULL)
        arena = arena_init(sizeof(st_Value) + sizeof(st_fValue) + sizeof(st_cValue) + 256 + 1);
    st_Value *out = arena_alloc(arena, sizeof(st_Value));
    out->col = col;
    char *vstr = arena_alloc(arena, 256);
    strcpy(vstr, str);
    out->value = arena_alloc(arena, sizeof(st_fValue));
    if(col){
        out->value->cond->str = vstr;
        out->value->cond->input = input;
        out->value->cond->size = size;
    } else
        out->value->str = vstr;
    return out;
}

char *value_get(st_Value *value){
    return (value->col)? (char*) cond(value->value->cond->str, (st_Object **) value->value->cond->input, value->value->cond->size): value->value->str;
}
