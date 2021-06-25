#include <stdbool.h>
#include <bson.h>

#ifndef SPO_NOW_UTIL_H
#define SPO_NOW_UTIL_H

typedef enum COMMAND_TYPE {
    CREATE,
    READ,
    UPDATE,
    DELETE
} cmd_t;

typedef struct condStruct {
    char *key;
    char *operator;
    char *type;
    char *param_utf8;
    int param_int;
} cond_t;

cond_t *new_cond(char *key, char *operator, char *type, char *param_utf8, int param_int);

typedef struct elemStruct {
    char *key;
    char *value_utf8;
    int value_int;
    char *type;
} elem_t;

elem_t *new_elem(char *key, char *value_utf8, int value_int, char *type);

typedef struct pathCondStruct {
    cond_t **conditions;
    int cond_n;
    int *operators;
    char *actualPath;
} path_t;

path_t *new_path(cond_t **conditions, int cond_n, int *operators, char *actualPath);

typedef struct Api {
    enum COMMAND_TYPE command;
    elem_t **elems;
    int elem_n;
    char *nelem_part_path;
    path_t **paths;
    int path_n;
    int fromRoot;
} api_t;

api_t *
new_api(cmd_t command, elem_t **elems, int elem_n, char *nelem_part_path, path_t **paths, int path_n, int fromRoot);

int index_of(char *str, char *substr);

int is_cond(bson_t b, int cond_len, cond_t *conds[], const int operators[]);

#endif