#ifndef __VCB_INCL
#include "vcb/vcb.h"
#endif

char **strsplit(char *input, char delim){
    int i;
    int len = 0;
    for(i = 0; *(input+i) != '\0'; i++)
        if(*(input+i) == delim)
            len++;
    len++;
    char **output =malloc(len * sizeof(void*));
    int prev_space = 0;
    int trc = 0;
    for(i = 0; *(input+i) != '\0' && trc < len; i++)
        if(*(input+i) == delim){
            *(output+trc) = malloc((i-prev_space+1));
            for(int j = prev_space; j < i; j++)
                *(*(output+trc)+j-prev_space) = *(input+j);
            *(*(output+trc++)+i-prev_space) = '\0';
            prev_space = i+1;
        }
    *(output+trc) = malloc( (strlen(input)-prev_space+1));
    for(i = prev_space; *(input+i) != '\0'; i++)
        *(*(output+trc)+i-prev_space) = *(input+i);
    *(*(output+trc)+i-prev_space) = '\0';
    return output;
}

char **strsplits(char *input, char *delim){
    if(strlen(input) <= strlen(delim)){
        char **output = malloc(sizeof(char*));
        *output = input;
        return output;
    }

    int i;
    unsigned long long del_mask = 0ull;
    unsigned long long mask = 0ull;
    int del_len = strlen(delim);
    for(i = 0; i < del_len; i++){
        del_mask += (*(delim+i) ) << ((del_len-i-1) << 3);
        mask += (*(input+i)) << ((del_len-i-1) << 3);
    }
    int act_len = 0;
    int prev_space = 0;
    char **output_raw =malloc(strlen(input) * sizeof(void*));
    for(i = 0; *(input+i) != '\0'; i++){
        mask &= (1 << del_len) - 1;
        mask <<= 3;
        mask += *(input+i);
        if(mask == del_mask){
            *(output_raw+act_len) = malloc((i-del_len-prev_space+1));
            for(int j = prev_space; j < i-del_len; j++)
                *(*(output_raw+act_len)+j-prev_space) = *(input+j);
            *(*(output_raw+act_len++)+i-del_len-prev_space) = '\0';
            prev_space = i+1;
        }
    }
    char **output = malloc(act_len * sizeof(void*));
    for(i = 0; i < act_len; i++)
        *(output+i) = *(output_raw+i);
    return output;
}

char **arena_strsplit(st_Arena *arena, char *input, char delim){
    int i;
    int len = 0;
    for(i = 0; *(input+i) != '\0'; i++)
        if(*(input+i) == delim)
            len++;
    len++;
    char **output = arena_alloc(arena, (len+1) * sizeof(void*));
    int prev_space = 0;
    int trc = 0;
    for(i = 0; *(input+i) != '\0' && trc < len; i++)
        if(*(input+i) == delim){
            *(output+trc) = arena_alloc(arena, (i-prev_space+1));
            for(int j = prev_space; j < i; j++)
                *(*(output+trc)+j-prev_space) = *(input+j);
            *(*(output+trc++)+i-prev_space) = '\0';
            prev_space = i+1;
        }
    *(output+trc) = arena_alloc( arena, (strlen(input)-prev_space+1));
    for(i = prev_space; *(input+i) != '\0'; i++)
        *(*(output+trc)+i-prev_space) = *(input+i);
    *(*(output+trc++)+i-prev_space) = '\0';
    *(output+trc) = "";
    return output;
}

char **arena_strsplits(st_Arena *arena, char *input, char *delim){
    if(strlen(input) <= strlen(delim)){
        char **output = arena_alloc(arena, sizeof(char*));
        *output = input;
        return output;
    }

    int i;
    unsigned long long del_mask = 0ull;
    unsigned long long mask = 0ull;
    int del_len = strlen(delim);
    for(i = 0; i < del_len; i++){
        del_mask += (*(delim+i) ) << ((del_len-i-1) << 3);
        mask += (*(input+i)) << ((del_len-i-1) << 3);
    }
    int act_len = 0;
    int prev_space = 0;
    char **output_raw =arena_alloc(arena, strlen(input) * sizeof(void*));
    for(i = 0; *(input+i) != '\0'; i++){
        mask &= (1 << del_len) - 1;
        mask <<= 3;
        mask += *(input+i);
        if(mask == del_mask){
            *(output_raw+act_len) = arena_alloc(arena, (i-del_len-prev_space+1));
            for(int j = prev_space; j < i-del_len; j++)
                *(*(output_raw+act_len)+j-prev_space) = *(input+j);
            *(*(output_raw+act_len++)+i-del_len-prev_space) = '\0';
            prev_space = i+1;
        }
    }
    char **output = arena_alloc(arena, act_len * sizeof(void*));
    for(i = 0; i < act_len; i++)
        *(output+i) = *(output_raw+i);
    return output;
}

char *strsub(char *input, int start, int end){
    if(end < 0)
        end = strlen(input);
    char *out = malloc(end-start+1);
    for(int i = start; i < end; i++)
        *(out+i-start) = *(input+i);
    *(out+end-start) = '\0';
    return out;
}

char *arena_strsub(st_Arena *arena, char *input, int start, int end){
    if(end < 0)
        end = strlen(input);
    char *out = arena_alloc(arena, end-start+1);
    for(int i = start; i < end; i++)
        *(out+i-start) = *(input+i);
    *(out+end-start) = '\0';
    return out;
}

int strfind(char *input, char delim, int num){
    int i;
    for(i = 0; *(input+i) != '\0' && num >= 0; i++)
        if(*(input+i) == delim)
            num--;
    return --i;
}

int strcompl(char *input, int num){
    char op = *(input+num);
    char cl;
    switch(op){
        case '(':
            cl = ')';
            break;
        case '{':
            cl = '}';
            break;
        case '[':
            cl = ']';
            break;
        default:
            cl = ')';
            break;
    }
    int i;
    int br = 1;
    for(i = num+1; *(input+i) != '\0' && br > 0; i++)
        if(*(input+i) == op)
            br++;
        else if(*(input+i) == cl)
            br--;
    return --i;
}

int strmatch(char *input, char delim){
    int num = 0;
    for(int i = 0; *(input+i) != '\0'; i++)
        if(*(input+i) == delim)
            num++;
    return num;
}

int strmatchm(char *input, char *delim){
    int out = 0;
    for(int i = 0; *(delim+i); i++)
        out += strmatch(input, *(delim+i));
    return out;
}


char *strrep(char *input, char *replacement, int start, int end){
    int len = start-end;
    for(int i = 0; *(input+i); i++) len++;
    for(int i = 0; *(replacement+i); i++) len++;
    char *out = malloc(len+1);
    len = 0;
    for(int i = 0; i < start; i++)
        *(out+len++) = *(input+i);
    for(int i = 0; *(replacement+i) != '\0'; i++)
        *(out+len++) = *(replacement+i);
    for(int i = end+1; *(input+i) != '\0'; i++){
        *(out+len++) = *(input+i);
    }
    *(out+len) = '\0';
    return out;
}

char *arena_strrep(st_Arena *arena, char *input, char *replacement, int start, int end){
    int strlen_repl = strlen(replacement);
    int strlen_input = strlen(input);
    int space = ((strlen_repl > end-start)? strlen_repl-end+start+1: 1);
    char *out = arena_alloc(arena, strlen_input+space+1);
    int len = 0;
    for(int i = 0; i < strlen_input && len < strlen_input+space; i++)
        if(i < start)
            *(out+len++) = *(input+i);
        else if(i <= end && i-start < strlen_repl)
            *(out+len++) = *(replacement+i-start);
        else if(i > end)
            *(out+len++) = *(input+i);
    *(out+strlen_input+space) = '\0';
    return out;
}

char *strinsert(char* input, char *insert, int pos){
    int strlen_insert = strlen(insert);
    int strlen_input = strlen(input);
    char *out = malloc(strlen_input+strlen_insert+1);
    for(int i =0; i < strlen_input + strlen_insert; i++)
        if(i < pos)
            *(out+i) = *(input+i);
        else if(i-pos < strlen_insert)
            *(out+i) = *(insert+i-pos);
        else
            *(out+i) = *(input+i-strlen_insert);
    *(out+strlen_input+strlen_insert) = '\0';
    return out;
}

char *arena_strinsert(st_Arena *arena, char* input, char *insert, int pos){
    int strlen_insert = strlen(insert);
    int strlen_input = strlen(input);
    char *out = arena_alloc(arena, strlen_input+strlen_insert+1);
    for(int i =0; i < strlen_input + strlen_insert; i++)
        if(i < pos)
            *(out+i) = *(input+i);
        else if(i-pos < strlen_insert)
            *(out+i) = *(insert+i-pos);
        else
            *(out+i) = *(input+i-strlen_insert);
    *(out+strlen_input+strlen_insert) = '\0';
    return out;
}

int strcontain(char *input, char delim){
    for(int i = 0; *(input+i) != '\0'; i++)
        if(*(input+i) == delim)
            return 1;
    return 0;
}

int strcontains(char *input, char* delim){
    for(int i =0; *(input+i) != '\0'; i++)
        if(strcontain(delim, *(input+i)))
            return 1;
    return 0;
}

char **strsplitm(char *input, char *delims){
    int i;
    int len = strmatchs(input, delims)+1;
    char **out = malloc((len+1) *sizeof(void*));
    int con = 0;
    int prevspace = 0;
    for(i = 0; *(input+i) != '\0' && con < len; i++){
        if(strcontain(delims, *(input+i))){
            *(out+con) = malloc(i-prevspace+1);
            for(int j = prevspace; j < i; j++)
                *(*(out+con)+j-prevspace) = *(input+j);
            *(*(out+con++)+i-prevspace) = '\0';
            prevspace = i+1;
        }
    }
    if(con < len){
        *(out+con) = malloc(i-prevspace+1);
        for(int j = prevspace; j < i; j++)
            *(*(out+con)+j-prevspace) = *(input+j);
        *(*(out+con++)+i-prevspace) = '\0';
    }
    *(out+con) = malloc(1);
    **(out+con) = '\0';

    return out;
}

int strmatchs(char *input, char *delim){
    int num = 0;
    for(int i = 0; *(input+i) != '\0'; i++)
        if(strcontain(delim, *(input+i)))
            num++;
    return num;
}

char *tolowers(char *input){
    int strlen_i = strlen(input);
    char *out = malloc(strlen_i+1);
    for(int i = 0; *(input+i) != '\0'; i++)
        *(out+i) = tolower(*(input+i));
    *(out+strlen_i) = '\0';
    return out;
}

char *strorderof(char *input, char *delim){
    int len = strmatchm(input, delim);
    char *out = malloc(len+1);
    int con = 0;
    for(int i = 0; *(input+i) != '\0'; i++)
        if(strcontain(delim, *(input+i)))
            *(out+con++) = *(input+i);
    *(out+con) = '\0';
    return out;
}

char **arena_strsplitm(st_Arena *arena, char *input, char *delims){
    int i;
    int len = strmatchs(input, delims)+1;
    char **out = arena_alloc(arena, (len+1) *sizeof(void*));
    int con = 0;
    int prevspace = 0;
    for(i = 0; *(input+i) != '\0' && con < len; i++){
        if(strcontain(delims, *(input+i))){
            *(out+con) = arena_alloc(arena, i-prevspace+1);
            for(int j = prevspace; j < i; j++)
                *(*(out+con)+j-prevspace) = *(input+j);
            *(*(out+con++)+i-prevspace) = '\0';
            prevspace = i+1;
        }
    }
    if(con < len){
        *(out+con) = arena_alloc(arena, i-prevspace+1);
        for(int j = prevspace; j < i; j++)
            *(*(out+con)+j-prevspace) = *(input+j);
        *(*(out+con++)+i-prevspace) = '\0';
    }
    *(out+con) = arena_alloc(arena, 1);
    **(out+con) = '\0';

    return out;
}
char *arena_strorderof(st_Arena *arena, char *input, char *delim){
    int len = strmatchm(input, delim);
    char *out = arena_alloc(arena, len+1);
    int con = 0;
    for(int i = 0; *(input+i) != '\0'; i++)
        if(strcontain(delim, *(input+i)))
            *(out+con++) = *(input+i);
    *(out+con) = '\0';
    return out;
}
