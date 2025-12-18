#ifndef __STDLIB_INCL
#include <stdlib.h>
#define __STDLIB_INCL ;
#endif 

#ifndef __STDIO_INCL
#include <stdio.h>
#define __STDIO_INCL ;
#endif

#ifndef __SQLITE3_INCL
#include <sqlite3.h>
#define __SQLITE3_INCL ;
#endif

#ifndef __STRING_INCL
#include <string.h>
#define __STRING_INCL ;
#endif

#ifndef __CTYPE_INCL
#include <ctype.h>
#define __CTYPE_INCL ;
#endif

#ifdef __cplusplus
extern "C"{
#endif 

#define __VCB_INCL ;

typedef struct{
    void *ptr;
    void *curr;
    size_t size;
} st_Arena;

struct st_List{
    struct st_List *next;
    st_Arena* arena;
    char ex;
    void *value;
};
typedef struct st_List st_List;

typedef union{
    st_List* list; 
    void* value;
} st_HashValue;

struct st_HashTable{
    size_t keysize; /* size of keys array */
    size_t keyiterator; /* number of keys */
    size_t arrsize; /* size of value array */
    size_t arriterator; /* number of slots filled in value array (lists are counted as 1) */
    int (*hash)(char*,int);
    st_Arena *keyarena;
    st_Arena *arrarena;
    char **keys;
    char *col; /* whether the element at an index is a list ( '1') or a value ('0' if NULL or '2' if ELEMENT) */ 
    st_HashValue **arr;
};
typedef struct st_HashTable st_HashTable;

typedef struct st_Var {
    char *name;
    char *lang;
    sqlite3 *db;
    char col; /*whether value is str or cond*/
    st_Arena *valarena; /*contains value, values and dependencies*/
    char *value;
    st_List *values; /*value list*/
    st_HashTable *dependencies; /* key: (st_Value*)->value or (char *) cond((st_Value*)->cond), value: st_List of char*/
} st_Var;

typedef struct {
    char *lang;
    sqlite3 *db;
    char *value;
    st_HashTable *vars;
}st_Object;

/*useful strings and string functions*/
extern char *path_to_data;
extern char **strsplit(char *input, char delim); /*split string by char delim*/
extern char **arena_strsplit(st_Arena *arena, char *input, char delim); /*strsplit, but with arena*/
extern char **strsplits(char *input, char *delim); /*split string by string delim*/
extern char **strsplitm(char *input, char *delim); /*split string by mulitple delims*/
extern char *strsub(char *input, int start, int end); /*find substring of string*/
extern char *arena_strsub(st_Arena *arena, char *input, int start, int end); /*strsub, but with arena*/
extern int strfind(char *input, char delim, int num); /*find the nth occurence of the delimeter in the str*/
extern int strcompl(char *input, int num); /*find the compliment brackets in the str*/
extern int strmatch(char *input, char delim); /*find the num of occurences of a delim in a str*/
extern int strmatchm(char *input, char *delim);
extern char *strrep(st_Arena *arena, char *input, char *replacement, int start, int end);
extern char *strinsert(st_Arena *arena, char *input, char *insert, int pos);
extern int strcontain(char *input, char delim);
extern int strcontains(char *input, char *delim);

/*st_Arena functions*/
extern st_Arena *arena_init(size_t size);
extern void *arena_alloc(st_Arena *arena, size_t size);
extern void arena_free(st_Arena *arena);

/* st_List functions */
extern st_List *list_init(st_Arena *arena);
extern void list_add(st_List *list, void *value);
extern void *list_get(st_List *list, int index);
extern void list_rem(st_List *list, int (*sign_function)(void *));
extern void list_free(st_List *list);
extern void list_copy(st_List *receiver, st_List *giver);

/* st_HashTable functions and vars*/
extern int(*system_hash)(char*,int);
extern int hash_sdbm(char *input, int modulo); /*in-built hash function */
extern st_HashTable *hash_init(int(*hash)(char*,int)); /*initialize a hashtable*/
extern size_t hash_size(st_HashTable *hashtable); /*get size of hashtable*/
extern void *hash_get(st_HashTable *hashtable, char *key); /*get value from key*/
extern void **hash_getAll(st_HashTable *hashtable); /*get all values from hashtable */
extern void hash_add(st_HashTable *hashtable, char *key, void *value); /*add value to hashtable*/
extern void hash_set(st_HashTable *hashtable, char *key, void *value); /*set value to an already present key */
extern void hash_rem(st_HashTable *hashtable, char *key); /*remove a key from the hashtable*/
extern void hash_free(st_HashTable *hashtable); /*free st_Hash*/
extern void hash_print(FILE *out, st_HashTable *hashtable, char *(*string_function)(void*)); /*print the contents of a hashtable, the string function converts the value to (char *) */

/* sql functions */
extern void sql_close(sqlite3* db);

/* st_Var functions */
extern sqlite3 *var_open(char *lang); /*opens the var sql database for a langauage*/
extern st_Var *var_init(char *name, char *lang, st_HashTable *dependencies); /*initialize a st_Var, without database */
extern st_Var *var_ffetch(char *name, char *lang); /*fetch from database, when this is the first var*/
extern st_Var *var_fetch(char *name, st_Var *base); /*fetch from databse*/
extern void var_write(st_Var *var); /*update database*/
extern void var_set(st_Var *var, char *value); /*set value if value is in values*/
extern void var_irrset(st_Var *var, char *value); /*set value, don't care about values*/
extern void var_free(st_Var *var); /*free st_Var*/

/* st_Object functions */
extern st_Object *object_init(char *value, char *lang, st_List *vars);
extern int object_equal(st_Object *obj1, st_Object *obj2);
extern void object_free(st_Object *obj);

/* coms, bombs */
extern char *cond(char *condstr_raw, st_Object **input, size_t size);
extern void *com(char *comstr, st_Object **input, size_t size);
extern st_List *domain(char *domstr, st_Object **input, size_t size);
extern void *bomb(char *bombstr, st_Object **input, size_t size);

#ifdef __cplusplus
}
#endif
