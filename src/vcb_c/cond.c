#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define COND_STRLEN 256

char *evaluate(char *condstr, st_Object **input, size_t size){
    if(*condstr == '#'){
        int index = atoi(strsub(condstr, 1, strfind(condstr, '.', 0)));
        if(index < size){
            char *value = strsub(condstr, strfind(condstr, ',', 0)+1, strlen(condstr));
            if(strcmp(value, "value"))
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

int boolEval(char *condstr, st_Object **input, size_t size){
    char *toeval = evaluate(condstr, input, size);
    if(*toeval == '0')
        return 0;
    return 1;
}

int boolconcat(int eval, char prevsymbol, int currans){
    switch(prevsymbol){
        case '&':
            return currans & eval;
        case '|':
            return currans | eval;
    }
    return 0;
}

char *cond(char *condstr_raw, st_Object **input, size_t size){
    char *condstr = malloc(strlen(condstr_raw));
    int con = 0;
    for(int i =0; *(condstr_raw+i) != '\0'; i++)
        if(!isspace(*(condstr_raw+i)))
            *(condstr+con++) = *(condstr_raw+i);
    *(condstr+con) = '\0';
    //remove subconditions
    char *scl_cond = malloc(COND_STRLEN);
    int num = 0;
    char *scl_sub;
    int opfind, clfind;
    while(strmatch(scl_cond, '(')-1 - num){
        opfind = strfind(condstr, '(', num++);
        clfind = strcompl(condstr, opfind);
        scl_sub = strsub(scl_cond+1, opfind, clfind);
        scl_cond = strrep(NULL, scl_cond, cond(scl_sub, input, size), opfind, clfind);
    }
    //compute
    char *arithmetic_symbols = "+-*/";
    char *equality_symbols = "=<>";
    char *logical_symbols = "&|!";
    if(strcontains(scl_cond, arithmetic_symbols)){
        char **parts = strsplitm(scl_cond, arithmetic_symbols);
        int sum = 0;
        for(int i = 0; strcmp(*(parts+i), ""); i++)
            sum += atoi(evaluate(*(parts+i), input, size));
        char *out = malloc(10); 
        sprintf(out, "%d", sum);
        return out;
    } else if(strcontains(scl_cond, equality_symbols)){
        char **parts = strsplitm(scl_cond, equality_symbols);
        if(*(parts+1)){
            if(strcontain(scl_cond, '=')){
                return (strcmp(evaluate(*parts, input, size), evaluate(*(parts+1), input, size)))? "1": "0";
            } else if(strcontain(scl_cond, '>')){
                return (atoi(evaluate(*parts, input, size)) > atoi(evaluate(*(parts+1), input, size)))? "1": "0";
            } else if(strcontain(scl_cond, '<')){
                return (atoi(evaluate(*parts, input, size)) < atoi(evaluate(*(parts+1), input, size)))? "1": "0";
            }
        }else {
            fprintf(stderr, "vcb_c/cond.c, cond(): NOT ENOUGH EQUALITY SYMBOLS\n");
            exit(8);
        }
    } else if(strcontains(scl_cond, logical_symbols)){
        char *out = malloc(1);
        if(*scl_cond == '!'){
            *out = boolEval(strsub(scl_cond, 1, strlen(scl_cond)), input, size) + '0';
            return out;
        }
        int currans = -1;
        int prevspace = 0;
        char prevsymbol = 0;
        for(int i = 0; *(scl_cond+i) != '\0'; i++){
            if(strcontain(logical_symbols, *(scl_cond+i))){
                if(currans < 0)
                    currans = boolEval(strsub(scl_cond, 0, i), input, size);
                else
                    currans = boolconcat(boolEval(strsub(scl_cond, prevspace, i), input, size), prevsymbol, currans);
                switch(*(scl_cond+i)){
                    case '&':
                        prevsymbol = '&';
                        break;
                    case '|':
                        prevsymbol = '|';
                        break;
                }
                prevspace = i+1;
            }
        }
        *out = currans + '0';
        return out;
    }
    return scl_cond;
}

