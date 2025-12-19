#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

st_List *list_init(st_Arena *arena){
    st_List *list;
    char ex = 1;
    if(arena == NULL){
        arena = arena_init(sizeof(st_List)+1);
        ex = 0;
    }
    list = arena_alloc(arena, sizeof(st_List));
    list->arena = arena;
    list->ex = ex;
    list->value = 0;
    list->next = 0;
    return list;
}

void list_add(st_List *list, void *value){
    if(!list->value){
        list->value = value;
        return;
    }
    if(!list->ex && sizeof(st_List) >= list->arena->size){
        st_Arena *newarena = arena_init((list->arena->size+list->arena->curr-list->arena->ptr) << 1 );
        st_List *newlist = arena_alloc(newarena, sizeof(st_List));
        newlist->arena = newarena;
        st_List *currnew = newlist;
        st_List *curr;
        for(curr = list; curr; curr = curr->next){
            currnew->value = curr->value;
            currnew->next = arena_alloc(newarena, sizeof(st_List));
            currnew = currnew->next;
        }
        currnew->value = value;
        currnew->next = 0;
        list_free(list);
        list = newlist;
    } else{
        st_List *curr;
        for(curr = list; curr->next; curr = curr->next);
        curr->next = arena_alloc(list->arena, sizeof(st_List));
        curr = curr->next;
        curr->value = value;
        curr->next = 0;
    }
}

void *list_get(st_List *list, int index){
    st_List *curr = list;
    for(int i = 0; i < index && curr->next; i++){
        curr = curr->next;
    }
    return curr->value;
}

void list_copy(st_List *receiver, st_List *giver){
    st_List *curr_r = receiver;
    st_List *curr_g;
    for(curr_g = giver; curr_g->next; curr_g = curr_g->next){
        curr_r->value = curr_g->value;
        if(curr_r->next)
            curr_r = curr_r->next;
        else{
            curr_r = arena_alloc(receiver->arena, sizeof(st_List));
            curr_r = curr_r->next;
        }
    }
    curr_r->value = curr_g->value;
}

void list_pop(st_List *list){
    st_List *curr;
    for(curr = list; curr->next->next; curr = curr->next);
    curr->next = 0;
}

void list_rem(st_List *list, int (*sign_function)(void *)){
    if(sign_function(list->value))
        list = list->next;
    for(st_List *curr = list; curr->next; curr = curr->next)
        if(sign_function(curr->next->value)){
            if(curr->next->next)
                curr->next = curr->next->next;
            else
                curr->next = 0;
        }
}

size_t list_size(st_List *list){
    size_t out = 0;
    for(st_List *curr = list; curr->next; curr=curr->next){
        out++;
    }
    out++;
    return out;
}

void list_free(st_List *list){
    if(!list->ex)
        arena_free(list->arena);
}
