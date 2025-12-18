#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define HASH_LIST '1'
#define HASH_ELEMENT_NULL '0'
#define HASH_ELEMENT_FULL '2'
#define HASH_NULL "__NULL__"
#define HASH_NULL_SIZE 9
#define HASH_KEY_STRLEN 256

int hash_sdbm(char *input, int modulo){
    unsigned hash = 0u;
    while(*input != '\0')
        hash = *(input++) + (hash << 6) + (hash << 16) - hash;
    if(modulo > 2){
        return hash % (modulo-1);
    } else if(modulo > 1) {
        return hash % modulo;
    } else {
        return 0;
    }
}

st_HashTable *hash_init(int (*hash)(char*,int)){
    st_HashTable *output = malloc(sizeof(st_HashTable));

    output->keysize = 1;
    output->keyiterator = 0;
    output->keyarena = arena_init(sizeof(void*)+HASH_KEY_STRLEN*output->keysize+1);
    output->keys = arena_alloc(output->keyarena, sizeof(void*));
    *output->keys = arena_alloc(output->keyarena, HASH_KEY_STRLEN);
    strcpy(*output->keys, HASH_NULL);

    output->arrsize = 1;
    output->arriterator = 0;
    output->arrarena = arena_init(sizeof(void*)+output->arrsize*(sizeof(void*)+sizeof(st_HashValue)+sizeof(st_List)+1)+1);
    output->arr = arena_alloc(output->arrarena, output->arrsize*sizeof(void*));
    *output->arr = arena_alloc(output->arrarena, sizeof(st_HashValue));
    (*output->arr)->value = arena_alloc(output->arrarena, sizeof(void*));

    output->col = arena_alloc(output->arrarena, output->arrsize+1);
    *(output->col) = HASH_ELEMENT_NULL;

    output->hash = hash;
    return output;
}

void *hash_get(st_HashTable *hashtable, char *key){
    int place = hashtable->hash(key, hashtable->arrsize);
    if(*(hashtable->col+place) == HASH_ELEMENT_FULL)
        return (*(hashtable->arr+place))->value;
    else if(*(hashtable->col+place) == HASH_ELEMENT_NULL){
        return (void *) HASH_NULL;
    }else{
        st_List *curr = (*(hashtable->arr+place))->list;
        for(int i = 0; curr->next && i < hashtable->keyiterator && strcmp((*(hashtable->keys+i)), key); i++)
            if(hashtable->hash((*(hashtable->keys+i)), hashtable->arrsize) == place)
                curr = curr->next;
        return curr->value;
    }
}

void hash_resize(st_HashTable *hashtable){
    st_Arena *newarena = arena_init(sizeof(void*)+(hashtable->arrsize << 1)*(sizeof(st_HashValue)+sizeof(void*)+sizeof(st_List)+1)+1);
    st_HashValue **newarr = arena_alloc(newarena, (hashtable->arrsize << 1)*sizeof(void*));
    char *newcol = arena_alloc(newarena, (hashtable->arrsize << 1)+1);

    if(newarr == NULL){
        fprintf(stderr, "vcb/hashtable.c, hash_add(): FAILED TO ALLOCATE NEWARR");
        exit(10);
    }
    if(newcol == NULL){
        fprintf(stderr, "vcb/hashtable.c, hash_add(): FAILED TO ALLOCATE NEWCOL");
        exit(10);
    }

    for(int i = 0; i < ( hashtable->arrsize << 1 ); i++){/*set default values */
        *(newarr+i) = arena_alloc(newarena, sizeof(st_HashValue));
        *(newcol+i) = HASH_ELEMENT_NULL;
    }
    hashtable->arriterator = 0;

    int place;
    void *prevValue;
    for(int i = 0; i < hashtable->keyiterator; i++){ /* add to newarr */
        place = hashtable->hash(*(hashtable->keys+i), (hashtable->arrsize << 1));
        prevValue = hash_get(hashtable, *(hashtable->keys+i));

        if( *(newcol+place) == HASH_ELEMENT_NULL){
            (*(newarr+place))->value = prevValue;
            *(newcol+place) = HASH_ELEMENT_FULL;
            hashtable->arriterator++;
        } 
        else if( *(newcol+place) == HASH_ELEMENT_FULL){
            st_List *list = list_init(newarena);
            list_add(list, (*(newarr+place))->value);
            list_add(list, prevValue);
            (*(newarr+place))->list = list;
            *(newcol+place) = HASH_LIST;
        } 
        else 
            list_add((*(newarr+place))->list, prevValue);
    }
    arena_free(hashtable->arrarena);
    hashtable->arrarena = newarena;
    hashtable->arrsize <<= 1;
    hashtable->arr = newarr;
    hashtable->col = newcol;
}

void hash_key_resize(st_HashTable *hashtable){
    hashtable->keysize <<= 1;
    st_Arena *newarena = arena_init((sizeof(void*)+HASH_KEY_STRLEN)*hashtable->keysize+1);
    char **newkeys = arena_alloc(newarena, hashtable->keysize*sizeof(void*));
    if(newkeys == NULL){
        fprintf(stderr, "vcb/hashtable.c, hash_add(): FAILED TO ALLOCATE ARRAY FOR NEW KEYS");
        exit(10);
    }
    for(int i = 0; i < hashtable->keysize; i++){
        *(newkeys+i) = arena_alloc(newarena, HASH_KEY_STRLEN);
        if(i < hashtable->keysize >> 1)
            strcpy(*(newkeys+i), *(hashtable->keys+i));
    }
    arena_free(hashtable->keyarena);
    hashtable->keyarena = newarena;
    hashtable->keys = newkeys;
}

void hash_set(st_HashTable *hashtable, char *key, void *value){
    int place = hashtable->hash(key, hashtable->arrsize);
    if (*(hashtable->col+place) - HASH_LIST)
        (*(hashtable->arr+place))->value = value;
    else {
        st_List *list = (*(hashtable->arr+place))->list;
        for(int i = 0; i < hashtable->keyiterator; i++){
            if( list == NULL){
                fprintf(stderr, "vcb/hashtable.c, hash_set(): FAILED TO FIND ELEMENT, DID YOU MEAN TO USE hash_add()?\n");
                return;
            }
            if( strcmp( (*(hashtable->keys+i)), key) == 0) 
                break;
            else
                if( hashtable->hash( (*(hashtable->keys+i)), hashtable->arrsize) == place)
                    list = list->next; 
        }
        list->value = value;
    }
}

void hash_add(st_HashTable *hashtable, char* key, void *value){
    for(int i = 0; i < hashtable->keyiterator; i++) /* check if the key is already in the map */
        if(strcmp(*(hashtable->keys+i), key) == 0){
            fprintf(stderr, "vcb/hashtable.c, hash_add(): KEY ALREADY IN HASHTABLE, DOING hash_set() INSTEAD\n");
            hash_set(hashtable, key, value);
            return;
        }
    
    if(hashtable->keyiterator >= hashtable->keysize) /* check if hashtable->keys needs to be extended */
        hash_key_resize(hashtable);

    if(hashtable->arriterator >= ((hashtable->arrsize) >> 1)) /*hashtable->arr needs to be extended*/
        hash_resize(hashtable);

    int place = hashtable->hash(key, hashtable->arrsize);
    if(*(hashtable->col+place) == HASH_ELEMENT_NULL){ /* check for collisions */ 
        *(hashtable->col+place) = HASH_ELEMENT_FULL;
        (*(hashtable->arr+place))->value = value;
        hashtable->arriterator++;
    } else if(*(hashtable->col+place) == HASH_ELEMENT_FULL){
        st_List *list = list_init(hashtable->arrarena);
        list_add(list, (*(hashtable->arr+place))->value);
        list_add(list, value);
        (*(hashtable->arr+place))->list = list;
        *(hashtable->col+place) = HASH_LIST;
    } else 
        list_add((*(hashtable->arr+place))->list, value);

    // static int num_of_execs = 0;
    // printf("num_of_execs: %d\n", num_of_execs++);
    if(*(hashtable->keys+hashtable->keyiterator) == NULL)
        *(hashtable->keys+hashtable->keyiterator) = arena_alloc(hashtable->keyarena, HASH_KEY_STRLEN);
    strcpy(*(hashtable->keys+hashtable->keyiterator++), key);

    // printf("hashtable->arrsize: %d\n", (int) hashtable->arrsize);
}

void **hash_getAll(st_HashTable *hashtable){
    void **output = malloc((hashtable->keyiterator) * sizeof(void*));
    int len = 0;
    for(int i = 0; i < hashtable->arrsize; i++)
        if((*(hashtable->col+i)) == HASH_ELEMENT_FULL)
            *(output+len++) = (*(hashtable->arr+i))->value;
        else if( *(hashtable->col+i) == HASH_LIST){
            st_List *list = ((st_List*) (*(hashtable->arr+i))->list);
            while(list->next){
                *(output+len++) = list->value;
                list = list->next;
            }
        }
    return output;
}


void hash_rem(st_HashTable *hashtable, char *key){
    if(hashtable->keyiterator <= 0){
        fprintf(stderr, "vcb/hashtable.c, hash_rem(): NO KEYS TO REMOVE\n");
        return;
    }

    int place = hashtable->hash(key, hashtable->arrsize);

    if(*(hashtable->col + place) == HASH_ELEMENT_FULL){
        hash_set(hashtable, key, HASH_NULL);
        *(hashtable->col + place) =HASH_ELEMENT_NULL;
    }else if(*(hashtable->col + place) == HASH_LIST) {
        st_List *list = (*(hashtable->arr+place))->list;
        st_List *curr = list;   
        for(int i = 0; curr->next && i < hashtable->keyiterator && strcmp(key, *(hashtable->keys+i)); i++)
            if(hashtable->hash(key, hashtable->arrsize) == place)
                curr = curr->next;
        if(curr->next->next)
            curr->next = curr->next->next;
        else 
            curr->next = 0;
        if(!list->next){
            void *value = list->value;
            *(hashtable->col + place) = HASH_ELEMENT_FULL;
            (*(hashtable->arr + place))->value = value;
        }
    }

    char found = 0;
    for(int i = 0; i < hashtable->keyiterator; i++)
        if(found)
            *(hashtable->keys+i-1) = *(hashtable->keys+i);
        else
            if(strcmp(*(hashtable->keys+i), key) == 0)
                found = 1;
}

void hash_print(FILE *out, st_HashTable *hashtable, char *(*string_function)(void*)){
    fprintf(out, "\n");
    for(int j = 0; j < hashtable->arrsize; j++)
        if( (*(hashtable->col+j)) == HASH_ELEMENT_FULL)
            fprintf(out, "%s \n", string_function((*(hashtable->arr+j))->value));
        else if( (*(hashtable->col+j)) == HASH_LIST){
            fprintf(out, "table: \n");
            st_List *list = (*(hashtable->arr+j))->list;
            while(list->next){
                fprintf(out, "\t %s \n", string_function(list->value));
                list = list->next;
            }
            fprintf(out, "\t %s \n", string_function(list->value));
        }
    fprintf(out, "\n");
}

void hash_free(st_HashTable *hashtable){
    arena_free(hashtable->arrarena);
    arena_free(hashtable->keyarena);
    free(hashtable);
}
