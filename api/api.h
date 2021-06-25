#include <string.h>
#include "../util/util.h"

#ifndef SPO_NOW_API_H
#define SPO_NOW_API_H

void init();

char *apiCreate(char *path, int elem_length, elem_t **elems, FILE *new_stream);

char *apiRead(int pathcond_length, path_t **pathconds, int from_root, FILE *new_stream);

char *apiUpdate(char *path, int elem_length, elem_t **elems, FILE *new_stream);

char *apiDelete(char *path, FILE *new_stream);

#endif