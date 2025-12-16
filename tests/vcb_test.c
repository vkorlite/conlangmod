#define PATH_TO_DATA "~/Documents/coding/conlangmod/"
#include "vcb_c/vcb.h"

char *string_fn(void *value){
    return (char *)value;
}

//hashtable tests
int test_1(){ //tests hash function
    int len = 3;
    char *input[] = {"hello", "saying", "really long test, idk if this one will work, it is way beyond the scope of the hash", ""};
    int *output =(int*) malloc(len * sizeof(int)+1);
    int i = 0;
    for(i = 0; i < len; i++)
        *(output+i) = hash_sdbm(input[i], len);
    *(output+i) = -1;
    int done = 0;
    while(*output++ >= 0)
        done++;
    return len-done;
}
int test_2(){ //tests hash_init, hash_add, hash_get, and hash_free
    st_HashTable *hashtable = hash_init(&hash_sdbm);
    int len = 10;
    char *input[] = {"I", "Just", 
        "Came", "To", 
        "Say", "Hello", 
        "Ruka", "Ti slapovi", 
        "U Nogama", "Ritmovi", 
        "Ja Jasim", "Na divljoji vodi", 
        "Raspukla me usta", "Od nektare gusta", 
        "Ti kore si hrasta", "U ocima hrusta", 
        "Slana riba", "Od slatkom medu", 
        "Cadava krv", "Na crvenom nebu"};
    int i = 0;
    for(i = 0; i <  (len << 1); i+=2){
        hash_add(hashtable, *(input+i), *(input+i+1));
    }    

    int done = 0;
    for(i = 0; i < (len << 1); i+=2)
        if(strcmp((char *)hash_get(hashtable, *(input+i)), *(input+(i+1))) == 0)
            done++;
        else 
            printf("test_2: failed at string \"%s\". Should've got \"%s\", but got \"%s\" instead\n", *(input+i), *(input+i+1), (char *) hash_get(hashtable, *(input+i)));
    if (len - done > 0)
        for(i = 0; i < hashtable->keyiterator; i++)
            printf("%s -> %s\n", *((hashtable->keys)+i), (char *) hash_get(hashtable, *(hashtable->keys+i)));
    hash_free(hashtable);
    return len - done;
}
int test_3(){//hash_rem
    st_HashTable *hashtable = hash_init(&hash_sdbm);
    char *raw_input = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent nec nisi in felis ultricies blandit. Suspendisse potenti. Quisque in iaculis justo, a egestas nibh. Quisque blandit, tellus at blandit venenatis, tellus neque finibus dui, sit amet faucibus urna magna et tellus. Nulla pulvinar, elit vel viverra tincidunt, ante lectus pretium velit, nec mollis dolor nulla id enim. Sed feugiat, tellus non blandit ultrices, sapien ante sodales dui, id molestie felis neque at neque. Nam felis ipsum, vulputate vitae lorem ac, auctor pellentesque metus. Sed ac efficitur magna. Aenean sed risus enim. Pellentesque sit amet lectus magna. Class aptent taciti sociosqu.";
    char **input = strsplit(raw_input, ' ');
    int len = 100;
    int done = 0;
    int done2 = 0;
    int repeats = 0;
    for(int i = 0; i < len-2; i+=2)
        for(int j = i+2; j < len; j+=2)
            if(strcmp(*(input+i), *(input+j)) == 0)
                repeats++;
     

    for(int i = 0; i < len; i+=2)
        hash_add(hashtable, *(input+i), *(input+i+1));

    for(int i = 0; i < len; i+=2)
        if(strcmp((char *) hash_get(hashtable, *(input+i)), *(input+i+1)) == 0)
            done++;
        else
            printf("done FAILED AT %s -> %s \n", *(input+i), *(input+i+1));

    for(int i =0; i < len; i+=2)
        hash_rem(hashtable, *(input+i));

    printf("rem done\n");

    hash_print(stdout, hashtable, &string_fn);

    for(int i = 0; i < len; i+=2)
        if(strcmp((char *)hash_get(hashtable, *(input+i)), "__NULL__") == 0)
            done2++;
        else
            printf("done2 FAILED AT %s -> %s \n", *(input+i), *(input+i+1));

    return (done+repeats)-done2;
}

//var tests
int test_4(){ //tests var_init
    st_HashTable *hashtable = hash_init(&hash_sdbm);
    st_List *dep_var1 = malloc(sizeof(st_List));
    dep_var1->value = "dep_var1";
    dep_var1->next = 0;
    hash_add(hashtable, "test_value1", dep_var1);
    st_Var *var = var_init("Test_var", "test", hashtable, "");
    int done = 0;
    if(strcmp(var->name, "Test_var"))
        done++;
    if(strcmp(var->lang, "test"))
        done++;
    return done;
}

int main(int argc, char *argv[]){
    int len = 4;
    int ((*test_arr[])()) = {*test_1, *test_2, *test_3, *test_4};
    for(int i = 0; i < len; i++)
        printf("test_%d: %d\n", i+1, (test_arr[i])());

}
