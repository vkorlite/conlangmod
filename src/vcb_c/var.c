#ifndef __VCB_INCL
#include "vcb_c/vcb.h"
#endif

#define VAR_STRLEN 256
#define VAR_MAXVAL 100

void sql_close(sqlite3* db){
    int rc = sqlite3_close(db);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, sql_close(): UNABLE TO CLOSE DATABASE, %s\n", sqlite3_errmsg(db));
        exit(10);
    }
}

sqlite3 *var_open(char *lang){
    sqlite3 *db;
    char *find_path = malloc(VAR_STRLEN);
    sprintf(find_path, "%sdb/%s/%s.db", getenv("CONLANGMOD_REC_DIR"), lang, lang);
    int rc = sqlite3_open(find_path, &db);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_fetch(): COULD NOT OPEN DATABASE, %s\n", sqlite3_errmsg(db));
        exit(10);
    }
    free(find_path);
    return db;
}

st_Var *var_init( char *name, char *lang, st_HashTable *dependencies, char *morph){
    st_Var *out = malloc(sizeof(st_Var));
    out->name = malloc(VAR_STRLEN);
    out->lang = malloc( VAR_STRLEN);
    out->morph = malloc(VAR_STRLEN);
    strcpy(out->name, name);
    strcpy(out->lang, lang);
    strcpy(out->morph, morph);

    out->valarena = arena_init(dependencies->keyiterator * ( sizeof(st_List) + VAR_STRLEN + 1 )+1);
    st_List *list = list_init(out->valarena);
    st_List *curr = list;
    for(int i = 0; i < dependencies->keyiterator; i++){
        curr->value = arena_alloc(out->valarena, VAR_STRLEN);
        strcpy(curr->value, *dependencies->keys);
        if(i < dependencies->keyiterator - 1 ){
            curr->next = arena_alloc(out->valarena, sizeof(st_List));
            curr = curr->next;
        } else
            curr->next = 0;
    }
    out->values = list;
    out->dependencies = dependencies;
    return out;
}

void var_set(st_Var *var, char *value){
    for(st_List *list = var->values; list; list = list->next)
        if(strcmp((char *)list->value,value) == 0){
            strcpy(var->value, value);
            return;
        }
}

void var_irrset(st_Var *var, char *value){
    strcpy(var->value, value);
}

st_Var *var_ffetch(char *name, char *lang){
    st_Var *var = malloc(sizeof(st_Var));
    var->db = var_open(lang);
    var->lang = lang;
    return var_fetch(name, var);
}

st_Var *var_fetch(char *name, st_Var *base){
    sqlite3* db = base->db;
    sqlite3_stmt *stmt;

    st_Var *var = malloc(sizeof(st_Var));
    var->name = name;
    var->db = db;
    var->lang = base->lang;

    st_Arena *trash = arena_init(VAR_STRLEN * 2 * VAR_MAXVAL);
    st_Arena *arena = arena_init((sizeof(st_List)*2 + VAR_STRLEN )*VAR_MAXVAL+VAR_STRLEN*2+1);

    int i;
    char *sql = arena_alloc(trash, VAR_STRLEN);
    sprintf(sql, "SELECT Morph,Possible FROM Var WHERE Name='%s';", name);
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); 
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_fetch(): COULD NOT GET VAR, %s\n", sqlite3_errmsg(db));
        exit(10);
    }

    char *value_str = arena_alloc(trash, VAR_STRLEN);
    char *morph_str = arena_alloc(trash, VAR_STRLEN);
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        strcpy(morph_str, (char* )sqlite3_column_text(stmt, 0));
        strcpy(value_str, (char *)sqlite3_column_text(stmt, 1));
    }

    var->morph = morph_str;

    char **value_arr = arena_strsplit(trash, value_str, ' ');
    st_List *value_list = list_init(arena);
    st_List *curr_list = value_list;
    if(strcmp(value_str, "")){
        curr_list->value = arena_alloc(arena, VAR_STRLEN);
        strcpy(curr_list->value, *value_arr);
        for(i = 0; strcmp(*(value_arr+i), ""); i++){
            curr_list->next = arena_alloc(arena, sizeof(st_List));
            curr_list = curr_list->next;
            curr_list->value = arena_alloc(arena, VAR_STRLEN);
            strcpy(curr_list->value, *(value_arr+i));
        }
        curr_list->next = 0;
    }
    var->values = value_list;
    //
    sprintf(sql, "SELECT Val,Dependent FROM Var_depend WHERE Name='%s';", name);
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_fetch(): COULD NOT GET VAR, %s\n", sqlite3_errmsg(db));
        exit(10);
    }

    st_HashTable *hashtable = hash_init(&hash_sdbm);
    char *var_str = arena_alloc(arena, VAR_STRLEN);
    char in;
    st_List *new_list = arena_alloc(arena, sizeof(st_List));
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        strcpy(value_str, (char*)sqlite3_column_text(stmt, 0));
        strcpy(var_str, (char*) sqlite3_column_text(stmt, 1));
        in = 1;
        for(i = 0; i < hashtable->keyiterator && in; i++)
            if(strcmp(*(hashtable->keys+i), value_str) == 0)
                in = 0;
        if(in){
            new_list->value = var_str;
            new_list->next = 0;
            hash_add(hashtable, value_str, new_list);
        } else {
            new_list = hash_get(hashtable, value_str);
            while(new_list->next){
                new_list = new_list->next;
            }
            new_list->next = arena_alloc(arena, sizeof(st_List));
            new_list = new_list->next;
            new_list->value = var_str;
            new_list->next = 0;
        }
    }
    var->dependencies = hashtable;
    var->valarena = arena;
    arena_free(trash);
    return var;
}

void var_depend_write(sqlite3 *db, char *name, char *key, char *value, char **sql, st_Arena *trash, sqlite3_stmt **stmt){
    sprintf(*sql, "select * from Var_depend where Name='%s' and Val='%s' and Dependent='%s';", name, key, value);
    int rc = sqlite3_prepare_v2(db, *sql, -1, stmt, NULL);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_depend_write(), COULD NOT READ FROM DATABASE, %s\n", sqlite3_errmsg(db));
        exit(10);
    }
    if((rc = sqlite3_step(*stmt)) == SQLITE_ROW)
        return;

    char *err = arena_alloc(trash, VAR_STRLEN);
    sprintf(*sql, "insert into Var_depend values('%s', '%s', '%s');", name, key, value);
    rc = sqlite3_exec(db, *sql, 0, 0, &err);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_write(), COULD NOT INSERT INTO VAR_DEPEND, %s\n", err);
        exit(10);
    }
}

void var_write(st_Var *var){
    char update = 0;
    sqlite3 *db = var->db;
    sqlite3_stmt *stmt;

    st_Arena *trash = arena_init( VAR_STRLEN*(2*VAR_MAXVAL+3));

    char *sql = arena_alloc(trash, VAR_STRLEN);
    sprintf(sql, "select * from Var where Name='%s';", var->name);
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_write(), COULD NOT OPEN TABLE, %s\n", sqlite3_errmsg(db));
        exit(10);
    }
    if((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        update = 1;

    char *possible = arena_alloc(trash, VAR_STRLEN*VAR_MAXVAL);
    st_List *curr_list = var->values;
    while(curr_list->next){
        strcat(possible, (char*)curr_list->value);
        strcat(possible, " ");
        curr_list = curr_list->next;
    }
    strcat(possible, (char*)curr_list->value);

    if(update)
        sprintf(sql, "update Var set Morph='%1s', Possible='%s' where Name='%s';", var->morph, possible, var->name);
    else 
        sprintf(sql, "insert into Var(Name, Morph, Possible) values ('%s', '%s', '%s');", var->name, var->morph, possible); 
    char *err = arena_alloc(trash, VAR_STRLEN);
    rc = sqlite3_exec(db, sql, 0, 0, &err);
    if(rc != SQLITE_OK){
        fprintf(stderr, "vcb/var.c, var_write(), COULD NOT WRITE INTO TABLE, %s\n", err);
        exit(10);
    }

    st_HashTable *hashtable = var->dependencies;
    char *key;
    st_List *depend_list;
    for(int i = 0; i < hashtable->keyiterator; i++){
        key = *(hashtable->keys+i);
        depend_list = hash_get(hashtable, key);
        while(depend_list->next){
            var_depend_write(db, var->name, key, (char*)depend_list->value, &sql, trash, &stmt);
            depend_list = depend_list->next;
        }
    }
    arena_free(trash);
}

void var_free(st_Var *var){
    arena_free(var->valarena);
    hash_free(var->dependencies);
}
