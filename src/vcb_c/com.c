#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define COM_STRLEN 256
typedef enum {
    FORALL_BOOL,
    EXISTS_BOOL,
    STRICT_BOOL,
    FORALL_LIST,
    EXISTS_LIST,
    NUMERIC
} opt;

void *com(char *comstr, st_Object **input, size_t size){
    char *domstr = malloc(COM_STRLEN);
    char *optstr = malloc(COM_STRLEN);
    char *condstr = malloc(COM_STRLEN);

    strcpy(domstr, strsub(comstr, strfind(comstr, '{', 0)+1, strcompl(comstr, strfind(comstr, '{', 0)) ) );
    strcpy(optstr, strsub(comstr, strfind(comstr, '{', 1)+1, strcompl(comstr, strfind(comstr, '{', 1)) ) );
    strcpy(condstr, strsub(comstr, strfind(comstr, '{', 2)+1, strcompl(comstr, strfind(comstr, '{', 2)) ) );

    st_List *domain_l = domain(domstr, input, size);

    opt option = FORALL_LIST;

    int optstr_s = strlen(optstr);
    if(optstr_s == 1)
        switch(tolower(*optstr)){
            case 'a':
                option = FORALL_BOOL;
                break;
            case 'e':
                option = EXISTS_BOOL;
                break;
            case 's':
                option = STRICT_BOOL;
                break;
            case 'l':
                option = FORALL_LIST;
                break;
            case 'f':
                option = EXISTS_LIST;
                break;
            case 'n':
                option = NUMERIC;
                break;
            default:
                option = FORALL_LIST;
        }
    else if(optstr_s > 1){
        char *decider = tolowers(optstr);
        if(!(strcmp(decider, "all") && strcmp(decider, "forall") && strcmp( decider, "allbool") && strcmp(decider, "forallbool") && strcmp(decider, "ab"))){
            option = FORALL_BOOL;
        } else if(!(strcmp(decider, "exists") && strcmp(decider, "existsbool") && strcmp(decider, "eb"))){
            option = EXISTS_BOOL;
        } else if(!(strcmp(decider, "strict") && strcmp(decider, "strictbool") && strcmp(decider, "strictlyexists") && strcmp(decider, "strictlyexistsbool"))){
            option = STRICT_BOOL;
        } else if(!(strcmp(decider, "listall") && strcmp(decider, "listforall") && strcmp(decider, "alllist") && strcmp(decider, "al") && strcmp(decider, "foralllist"))){
            option = FORALL_LIST;
        } else if(!(strcmp(decider, "listexists") && strcmp(decider, "existslist") && strcmp(decider, "find") && strcmp(decider, "el"))){
            option = EXISTS_LIST;
        } else if(!strcmp(decider, "numeric") || !strcmp(decider, "num")){
            option = NUMERIC;
        } else {
            option = FORALL_LIST;
        }
    }

    void *out = NULL;
    if(option <= STRICT_BOOL){
        char *outc = malloc(1);
        int pass = 0;
        st_List *curr;
        for(curr = domain_l; curr->next; curr=curr->next){
            st_List* curr_l = curr->value;
            size_t curr_s = list_size(curr_l);
            st_Object **temp_arr = malloc(sizeof(void*)*curr_s);
            int len = 0;
            st_List *curr_lit;
            for(curr_lit = curr_l; curr_lit->next; curr_lit = curr_lit->next)
                *(temp_arr+len++) = curr_lit->value;
            *(temp_arr+len) = curr_lit->value;
            if(*cond(condstr, temp_arr, len) - '0')
                pass++;
        }
        st_List* curr_l = curr->value;
        size_t curr_s = list_size(curr_l);
        st_Object **temp_arr = malloc(sizeof(void*)*curr_s);
        int len = 0;
        st_List *curr_lit;
        for(curr_lit = curr_l; curr_lit->next; curr_lit = curr_lit->next)
            *(temp_arr+len++) = curr_lit->value;
        *(temp_arr+len) = curr_lit->value;
        if(*cond(condstr, temp_arr, len) - '0')
            pass++;
        if(option == FORALL_BOOL)
            *outc = (!(list_size(domain_l)-pass))? '1': '0';
        else if(option == EXISTS_BOOL)
            *outc = (pass)? '1': '0';
        else if(option == STRICT_BOOL)
            *outc = (!(pass-1))? '1': '0';
        out = outc;
    } else if(option == NUMERIC){
    } else{}
    return out;
}
