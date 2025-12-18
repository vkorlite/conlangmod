#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define OBJ_STRLEN 256

st_Object *object_init(char *value, char *lang, st_List *vars){
    st_Object *out = malloc(sizeof(st_Object));
    out->db = ((st_Var*)vars->value)->db;
    st_HashTable *vars_hashtable = hash_init(&hash_sdbm);
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

void object_free(st_Object *object){
    hash_free(object->vars);
    free(object->value);
    free(object->lang);
    free(object);
}
