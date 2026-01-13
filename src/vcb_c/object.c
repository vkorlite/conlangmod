#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define OBJ_STRLEN 256

st_Object *object_init(char *value, char *lang, st_List *vars){
    st_Object *out = malloc(sizeof(st_Object));
    out->db = ((st_Var*)vars->value)->db;
    st_HashTable *vars_hashtable = sys_hash_init();
    st_List *curr;
    for(curr = vars; curr->next; curr = curr->next)
        hash_add(vars_hashtable, ((st_Var*)curr->value)->name, ((st_Var*)curr->value));
    out->vars = vars_hashtable;
    out->value = malloc(OBJ_STRLEN);
    strcpy(out->value, value);
    out->lang = malloc(OBJ_STRLEN);
    strcpy(out->lang, lang);
    return out;
}

int object_equal(st_Object *obj1, st_Object *obj2){
    if(strcmp(obj1->value, obj2->value)){
        return 0;
    }
    if(obj1->vars->keyiterator != obj2->vars->keyiterator){
        return 0;
    }
    for(int i = 0; i < obj1->vars->keyiterator ; i++)
        if(strcmp(*(obj1->vars->keys+i), *(obj2->vars->keys+i)))
            return 0;
    return 1;
}

void object_free(st_Object *object){
    hash_free(object->vars);
    free(object->value);
    free(object->lang);
    free(object);
}
