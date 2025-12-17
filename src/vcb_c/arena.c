#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

st_Arena *arena_init(size_t size){
    st_Arena *out = malloc(sizeof(st_Arena));
    out->ptr = malloc(size);
    out->curr = out->ptr;
    out->size = size;
    return out;
}

st_Arena *arena_recit(st_Arena *arena, size_t size){
    st_Arena *out;
    if(arena == NULL){
        out = malloc(sizeof(st_Arena));
        out->ptr = malloc(size);
    }
    else{
        out = arena_alloc(arena, sizeof(st_Arena));
        out->ptr = arena_alloc(arena, size);
    }
    out->curr = out->ptr;
    out->size = size;
    return out;
}

void *arena_alloc(st_Arena *arena, size_t size){
    if(size > arena->size){
        fprintf(stderr, "vcb/arena.c, arena_alloc(): NOT ENOUGH MEMORY FOR %d BYTES, %d BYTES LEFT\n", (int) size, (int) arena->size);
        return NULL;
    }
    void *out = arena->curr;
    arena->curr+=size;
    arena->size-=size;
    return out;
}

void arena_free(st_Arena *arena){
    free(arena->ptr);
    free(arena);
}


