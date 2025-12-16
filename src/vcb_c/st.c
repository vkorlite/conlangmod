#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

char **strsplit(char *input, char delim){
    int i;
    int len = 0;
    for(i = 0; *(input+i) != '\0'; i++)
        if(*(input+i) == delim)
            len++;
    len++;
    char **output =(char**) malloc(len * sizeof(void*));
    int prev_space = 0;
    int trc = 0;
    for(i = 0; *(input+i) != '\0' && trc < len; i++)
        if(*(input+i) == delim){
            *(output+trc) =(char*) malloc((i-prev_space+1));
            for(int j = prev_space; j < i; j++)
                *(*(output+trc)+j-prev_space) = *(input+j);
            *(*(output+trc++)+i-prev_space) = '\0';
            prev_space = i+1;
        }
    *(output+trc) = (char*)malloc( (strlen(input)-prev_space+1));
    for(i = prev_space; *(input+i) != '\0'; i++)
        *(*(output+trc)+i-prev_space) = *(input+i);
    *(*(output+trc)+i-prev_space) = '\0';
    return output;
}

char **strspits(char *input, char *delim){
    if(strlen(input) <= strlen(delim)){
        char **output = (char**)malloc(sizeof(char*));
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
    char **output_raw =(char**) malloc(strlen(input) * sizeof(void*));
    for(i = 0; *(input+i) != '\0'; i++){
        mask &= (1 << del_len) - 1;
        mask <<= 3;
        mask += *(input+i);
        if(mask == del_mask){
            *(output_raw+act_len) =(char*) malloc((i-del_len-prev_space+1));
            for(int j = prev_space; j < i-del_len; j++)
                *(*(output_raw+act_len)+j-prev_space) = *(input+j);
            *(*(output_raw+act_len++)+i-del_len-prev_space) = '\0';
            prev_space = i+1;
        }
    }
    char **output = (char**)malloc(act_len * sizeof(void*));
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
