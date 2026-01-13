#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

typedef struct{
    char *str;
    st_Object **objects;
    size_t size;
} args;
typedef struct{
    args *input;
    char *output;
} io;
char *string_fn(void *value){
    return (char *)value;
}

st_List *io_list_init(st_Arena *trash, int len){
    st_List *condlist = list_init(trash);
    st_List *curr = condlist;
    for(int i = 0; i < len; i++){
        io *leio = arena_alloc(trash, sizeof(io));
        args *temp = arena_alloc(trash, sizeof(args));
        temp->str = arena_alloc(trash, 256);
        temp->objects = NULL;
        temp->size = 0;
        leio->input = temp;
        leio->output = arena_alloc(trash, 256);
        curr->value = leio;
        curr->next = arena_alloc(trash, sizeof(st_List));
        curr = curr->next;
    }
    return condlist;
}

st_List *io_list_add(st_Arena *trash, st_List *curr, char *condstr, char *output, st_Object **objects, size_t size){
    strcpy(((io*)curr->value)->input->str, condstr);
    strcpy(((io*)curr->value)->output, output);
    ((io*)curr->value)->input->size = size;
    ((io*)curr->value)->input->objects = objects;
    return curr->next;
}

//hashtable tests
int test_1(){ //tests hash function
    int len = 3;
    char *input[] = {"hello", "saying", "really long test, idk if this one will work, it is way beyond the scope of the hash", ""};
    int *output = malloc(len * sizeof(int)+1);
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
    st_HashTable *hashtable = sys_hash_init();
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
    st_HashTable *hashtable = sys_hash_init();
    char *raw_input = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Praesent nec nisi in felis ultricies blandit. Suspendisse potenti. Quisque in iaculis justo, a egestas nibh. Quisque blandit, tellus at blandit venenatis, tellus neque finibus dui, sit amet faucibus urna magna et tellus. Nulla pulvinar, elit vel viverra tincidunt, ante lectus pretium velit, nec mollis dolor nulla id enim. Sed feugiat, tellus non blandit ultrices, sapien ante sodales dui, id molestie felis neque at neque. Nam felis ipsum, vulputate vitae lorem ac, auctor pellentesque metus. Sed ac efficitur magna. Aenean sed risus enim. Pellentesque sit amet lectus magna. Class aptent taciti sociosqu.";
    char **input = strsplit(raw_input, ' ');
    int len = 100;
    int done = 0;
    int done2 = 0;
    int repeats = 0;
    for(int i = 0; i < len-2; i+=2)
        for(int j = i+2; j < len; j+=2)
            if(strcmp(*(input+i), *(input+j)) == 0 && strcmp(*(input+i+1), *(input+j+1)))
                repeats++;

    for(int i = 0; i < len; i+=2)
        hash_add(hashtable, *(input+i), *(input+i+1));

    for(int i = 0; i < len; i+=2)
        if(strcmp((char *) hash_get(hashtable, *(input+i)), *(input+i+1)) == 0)
            done++;

    for(int i =0; i < len; i+=2)
        hash_rem(hashtable, *(input+i));

    for(int i = 0; i < len; i+=2)
        if(strcmp((char *)hash_get(hashtable, *(input+i)), "__NULL__") == 0)
            done2++;

    return (done+repeats)-done2;
}

//var tests
int test_4(){ //tests var_init
    st_HashTable *hashtable = sys_hash_init();
    st_List *dep_var1 = malloc(sizeof(st_List));
    dep_var1->value = "dep_var1";
    dep_var1->next = 0;
    hash_add(hashtable, "test_value1", dep_var1);
    st_Var *var = var_init("Test_var", "test", hashtable);
    int done = 0;
    if(strcmp(var->name, "Test_var"))
        done++;
    if(strcmp(var->lang, "test"))
        done++;
    var_free(var);
    return done;
}

int test_5(){ //tests var_write, var_open and var_ffetch
    st_HashTable *hashtable = sys_hash_init();
    st_List *dep_var1 = list_init(NULL);
    list_add(dep_var1, "dep_var1");
    hash_add(hashtable, "test_value1", dep_var1);
    st_Var *var_first = var_init("Test_var", "test", hashtable);
    var_first->db = var_open(var_first->lang);
    var_write(var_first);
    st_Var *var = var_ffetch("Test_var", "test");
    int done = 0;
    var_free(var);
    
    return done;
}

int test_6(){//test cond
    int len = 18;
    st_Arena *trash = arena_init((sizeof(st_List)+216+sizeof(args)+sizeof(io)+512)*2* len);
    st_List *condlist = io_list_init(trash, len);
    st_List *curr = condlist;
    //basic logical operations
    curr = io_list_add(trash, curr, "1&1", "1", NULL, 0);
    curr = io_list_add(trash, curr, "1|0", "1", NULL, 0);
    curr = io_list_add(trash, curr, "!1", "0", NULL, 0);
    //basic arithmetic operations
    curr = io_list_add(trash, curr, "1+1", "2", NULL, 0);
    curr = io_list_add(trash, curr, "1-1", "0", NULL, 0);
    curr = io_list_add(trash, curr, "2*3", "6", NULL, 0);
    curr = io_list_add(trash, curr, "6/2", "3", NULL, 0);
    //more than single digit :o
    curr = io_list_add(trash, curr, "15+190", "205", NULL, 0);
    curr = io_list_add(trash, curr, "500-45", "455", NULL, 0);
    curr = io_list_add(trash, curr, "40*6", "240", NULL, 0);
    curr = io_list_add(trash, curr, "48/8", "6", NULL, 0);
    //basic equality operations
    curr = io_list_add(trash, curr, "1=1", "1", NULL, 0);
    curr = io_list_add(trash, curr, "2>1", "1", NULL, 0);
    curr = io_list_add(trash, curr, "2<1", "0", NULL, 0);
    //subcondition test
    curr = io_list_add(trash, curr, "1+1=2", "1", NULL, 0);
    curr = io_list_add(trash, curr, "(2=1)+1=1", "1", NULL, 0);
    //object test
    st_Object *test1 = malloc(sizeof(st_Object));
    test1->value = malloc(3);
    strcpy(test1->value, "1");
    st_Object *test2 = malloc(sizeof(st_Object));
    test2->value = malloc(3);
    strcpy(test2->value, "2");
    st_Object **objects = malloc(sizeof(void*)*2);
    *objects = test1;
    *(objects+1) = test2;
    curr = io_list_add(trash, curr, "(#0.VALUE+1)=2", "1", objects, 1);
    curr = io_list_add(trash, curr, "#1.VALUE+#0.VALUE", "3", objects, 2);

    int out = 0;
    int i = 1;
    for(curr = condlist; curr->next; curr = curr->next){
        io *currio = (io*)curr->value;
        char *act = cond(currio->input->str, currio->input->objects, currio->input->size); 
        if(strcmp(act, currio->output)){
            printf("%d: %s -> %s, should've been %s\n", i, currio->input->str, act, currio->output);
            out++;
        }
        i++;
    }
    return out;
}

int test_7(){
    return 0;
}


int main(int argc, char *argv[]){
    int len = 6;
    int ((*test_arr[])()) = {*test_1, *test_2, *test_3, *test_4, *test_5, *test_6};
    for(int i = 0; i < len; i++)
        printf("test_%d: %d\n", i+1, (test_arr[i])());
    return 0;
}
