#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define OBJ_STRLEN 256

st_Object *object_init(char *value, char *lang, st_List *vars){
    st_Object *out = malloc(sizeof(st_Object));
    out->db = ((st_Var*)vars->value)->db;
    out->vars = vars;
    out->value = malloc(OBJ_STRLEN);
    strcpy(out->value, value);
    out->lang = malloc(OBJ_STRLEN);
    strcpy(out->lang, lang);

    st_HashTable *dependencies = hash_init(&hash_sdbm);
    st_List *curr;
    for(curr = vars; curr; curr = curr->next){
        hash_add(dependencies, ((st_Var*)curr->value)->name, morph(((st_Var*)curr->value)->name, out));
    }
    out->dependencies = dependencies;

    return out;
}

void object_free(st_Object *object){
    hash_free(object->dependencies);
    list_free(object->vars);
    free(object->value);
    free(object->lang);
    free(object);
}
