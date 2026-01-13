#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define COND_STRLEN 256

char *evaluate(char *condstr, st_Object **input, size_t size){
    if(*condstr == '#'){
        int index = atoi(strsub(condstr, 1, strfind(condstr, '.', 0)));
        if(index < size){
            char *value = strsub(condstr, strfind(condstr, ',', 0)+1, -1);
            if(strcmp(tolowers(value), "value"))
                return (*(input+(*(condstr+1)-'0')))->value;
            else{
                return ((st_Var*)hash_get((*(input+(*(condstr+1)-'0')))->vars, value))->value;
            }
        } else {
            return "0";
        }
    }
    return condstr;
}

int boolEval(char *condstr_raw, st_Object **input, size_t size){
    char *condstr = condstr_raw;
    if(*condstr_raw == '!'){
        condstr = strsub(condstr_raw, 1, -1);
        char *toeval = evaluate(condstr, input, size);
        if(*toeval == '0')
            return 1;
        return 0;
    }
    char *toeval = evaluate(condstr, input, size);
    if(*toeval == '0')
        return 0;
    return 1;
}

typedef struct {
    void *left;
    void *right;
    char symbol;
} Tree;

typedef union{
    Tree *tr;
    char *str;
} Branch;

typedef struct{
    char isleaf;
    Branch *value;
} Part;

Part *rec_tree_maker(st_Arena *trash, char **parts, char *dels){
    //base case
    char *priority = "|&=<>+-*/";
    Part *out = arena_alloc(trash, sizeof(Part));
    Branch *fork = arena_alloc(trash, sizeof(Branch));
    if(*dels == '\0'){
        out->isleaf = 1;
        fork->str = arena_alloc(trash, 256);
        strcpy(fork->str, *parts);
        out->value = fork;
        return out;
    }
    //find root
    out->isleaf = 0;
    Tree *trout = arena_alloc(trash, sizeof(Tree));
    out->value = arena_alloc(trash, sizeof(Branch));
    out->value->tr = trout;
    int pos = 0;
    int min = 10;
    for(int i = 0; *(dels+i) != '\0'; i++){
        int find = strfind(priority, *(dels+i), 0);
        if(min > find){
            min = find;
            pos = i;
        }
    }
    trout->symbol = *(dels+pos);
    //split into left dels & parts and right dels & parts
    int strlendels = strlen(dels);
    char *leftdels = arena_alloc(trash, pos+1);
    char *rightdels = arena_alloc(trash, strlendels-pos);
    char **leftparts = arena_alloc(trash, sizeof(void*)*((!pos)? 2*(pos+1): 2*pos+1));
    char **rightparts = arena_alloc(trash, sizeof(void*)*((!(strlendels-pos))?2*(strlendels-pos+1): 2*(strlendels-pos)+1));
    for(int i = 0; *(dels+i); i++)
        if(i < pos)
            *(leftdels+i) = *(dels+i);
        else if(i == pos)
            *(leftdels+i++) = '\0';
        else
            *(rightdels+i-pos-1) = *(dels+i);
    *(rightdels+strlendels-pos-1) = '\0';

    int i;
    for(i = 0; strcmp(*(parts+i), ""); i++){
        if( 2*i < 2*pos+1){ //symbols are in between the parts
            *(leftparts+i) = arena_alloc(trash, 256);
            strcpy(*(leftparts+i), *(parts+i)); 
        } else{
            if(!(i-pos-1)){
                *(leftparts+i) = arena_alloc(trash, 1);
                **(leftparts+i) = '\0';
            }
            *(rightparts+i-pos-1) = arena_alloc(trash, 256);
            strcpy(*(rightparts+i-pos-1), *(parts+i));
        }
    }
    *(rightparts+i-pos-1) = arena_alloc(trash, 1);
    **(rightparts+i-pos-1) = '\0';

    trout->left = rec_tree_maker(trash, leftparts, leftdels);
    trout->right = rec_tree_maker(trash, rightparts, rightdels);

    return out;
}

char *rec_tree_computer(st_Arena *trash, Part *tree, st_Object **input, size_t size, char first){
    if(tree->isleaf){
        if(*tree->value->str == '!'){
            char *out = arena_alloc(((first)? NULL: trash), 2); 
            *out = boolEval(tree->value->str, input, size)+'0';
            *(out+1) = '\0';
            return out;
        }
        return evaluate(tree->value->str, input, size);
    } else {
        char *out = arena_alloc( ((first)? NULL: trash), 256);
        Tree *trin = tree->value->tr;
        switch(trin->symbol){
            case '&':
                *out = (boolEval(rec_tree_computer(trash, (Part*)trin->left, input, size, 0), input, size) && boolEval(rec_tree_computer(trash, (Part*)trin->right, input, size, 0), input, size)) +'0';
                *(out+1) = '\0';
                break;
            case '|':
                *out = (boolEval(rec_tree_computer(trash, (Part*)trin->left, input, size, 0), input, size) || boolEval(rec_tree_computer(trash, (Part*)trin->right, input, size, 0), input, size)) +'0';
                *(out+1) = '\0';
                break;
            case '=':
                *out = '0' + (!strcmp(rec_tree_computer(trash, (Part*)trin->left, input, size, 0), rec_tree_computer(trash, (Part*)trin->right, input, size, 0)));
                *(out+1) = '\0';
                break;
            case '>':
                *out = '0' + (atoi(rec_tree_computer(trash, (Part*)trin->left, input, size, 0)) > atoi(rec_tree_computer(trash, (Part*)trin->right, input, size, 0)));
                *(out+1) = '\0';
                break;
            case '<':
                *out = '0' + (atoi(rec_tree_computer(trash, (Part*)trin->left, input, size, 0)) < atoi(rec_tree_computer(trash, (Part*)trin->right, input, size, 0)));
                *(out+1) = '\0';
                break;
            case '+':
                sprintf(out, "%d", (atoi(rec_tree_computer(trash, (Part*)trin->left, input, size, 0)) + atoi(rec_tree_computer(trash, (Part*)trin->right, input, size, 0))));
                break;
            case '-':
                sprintf(out, "%d", (atoi(rec_tree_computer(trash, (Part*)trin->left, input, size, 0)) - atoi(rec_tree_computer(trash, (Part*)trin->right, input, size, 0))));
                break;
            case '*':
                sprintf(out, "%d", (atoi(rec_tree_computer(trash, (Part*)trin->left, input, size, 0)) * atoi(rec_tree_computer(trash, (Part*)trin->right, input, size, 0))));
                break;
            case '/':
                sprintf(out, "%d", (atoi(rec_tree_computer(trash, (Part*)trin->left, input, size, 0)) / atoi(rec_tree_computer(trash, (Part*)trin->right, input, size, 0))));
                break;

        }
        return out;
    }
}

char *cond(char *condstr_raw, st_Object **input, size_t size){
    int strlen_raw = strlen(condstr_raw);
    st_Arena *trash = arena_init(1048*strlen_raw*sizeof(Tree));
    char *condstr = arena_alloc(trash, strlen(condstr_raw));
    int con = 0;
    for(int i =0; *(condstr_raw+i) != '\0'; i++)
        if(!isspace(*(condstr_raw+i)))
            *(condstr+con++) = *(condstr_raw+i);
    *(condstr+con) = '\0';
    //remove subconditions
    char *scl_cond = malloc(COND_STRLEN);
    strcpy(scl_cond, condstr);
    char *scl_sub, *scl_rep;
    int opfind, clfind;
    while(strmatch(scl_cond, '(')){
        opfind = strfind(condstr, '(', 0);
        clfind = strcompl(condstr, opfind);
        scl_sub = arena_strsub(trash, scl_cond, opfind+1, clfind);
        scl_rep = strrep(scl_cond, cond(scl_sub, input, size), opfind, clfind);
        free(scl_cond);
        scl_cond = scl_rep;
    }
    Part *tree = rec_tree_maker(trash, arena_strsplitm(trash, scl_cond, "&|=<>+-*/"), arena_strorderof(trash, scl_cond, "&|=<>+-*/"));
    char *out = rec_tree_computer(trash, tree, input, size, 1);
    arena_free(trash);
    return out;
}

