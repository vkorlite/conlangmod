#ifndef __VCB_INCL
#include "vcb/vcb.h"
#endif

st_List *domain(char *domstr, st_Object **input, size_t size){
    char **subconds = strsplit(domstr, ',');
    st_List *out = list_init(NULL);
    for(int i = 0; i < size; i++)
        if(*cond(*subconds, &(*(input+i)), 1) - '0'){
            st_List *temp = list_init(NULL);
            list_add(temp, *(input+1));
            list_add(out, temp);
        }

    for(int i = 0; strcmp(*(subconds+i), ""); i++){
        st_List *temp_output = list_init(NULL);
        st_List *curr;
        for(curr = out; curr->next; curr = curr->next){
            st_List *temp_input = list_init(NULL);
            list_copy(temp_input, curr);
            for(int j = 0; j < size; j++){
                int unique = 1;
                for(st_List *curr = temp_input; curr->next && unique; curr = curr->next)
                    if(object_equal((st_Object*)curr->value, *(input+j)))
                        unique = 0;
                if(unique && object_equal((st_Object*)curr->value, *(input+j)))
                    unique = 0;
                if(unique){
                    st_List *copy_temp_input = list_init(NULL);
                    list_copy(copy_temp_input, temp_input);
                    list_add(copy_temp_input, *(input+j));
                    list_add(temp_output, copy_temp_input);
                }
            }
            list_free(temp_input);
        }
        st_List *temp_input = list_init(NULL);
        list_copy(temp_input, curr);
        for(int j = 0; j < size; j++){
            int unique = 1;
            for(st_List *curr = temp_input; curr->next && unique; curr = curr->next)
                if(object_equal((st_Object*)curr->value, *(input+j)))
                    unique = 0;
            if(unique && object_equal((st_Object*)curr->value, *(input+j)))
                unique = 0;
            if(unique){
                st_List *copy_temp_input = list_init(NULL);
                list_copy(copy_temp_input, temp_input);
                list_add(copy_temp_input, *(input+j));
                list_add(temp_output, copy_temp_input);
            }
        }
        list_free(temp_input);
        list_free(out);
        out = temp_output;
    }
    int i;
    for(i = 0; strcmp(*(subconds+i), ""); i++)
        free(*(subconds+i));
    free(*(subconds+i));
    free(subconds);
    return out;
}
