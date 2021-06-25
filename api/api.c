#include "api.h"
#include "../util/util.h"
#include <bson.h>
#include <json.h>

void init() {
    bson_json_reader_t *reader;
    bson_error_t error;
    bson_t doc = BSON_INITIALIZER;

    FILE *bson;
    bson = fopen("mongo.bson", "w");
    reader = bson_json_reader_new_from_file("self.json", &error);

    if (bson_json_reader_read(reader, &doc, &error) < 0) {
        perror("dead");
    }

    fwrite(bson_get_data(&doc), 1, doc.len, bson);
    bson_json_reader_destroy(reader);
    bson_destroy(&doc);
    fclose(bson);
}

bson_t *read_bson_() {
    bson_reader_t *reader;
    bson_error_t error;
    const bson_t *docum;
    reader = bson_reader_new_from_file("mongo.bson", &error);
    docum = bson_reader_read(reader, NULL);
    return docum;
}

void write_bson(bson_t b) {
    bson_json_reader_t *reader;
    bson_error_t err;
    bson_t docum = b;

    FILE *bson;
    bson = fopen("mongo.bson", "w");
    reader = bson_json_reader_new_from_file("self.json", &err);

    if (bson_json_reader_read(reader, &docum, &err) < 0) {
        perror("dead");
    }
    fwrite(bson_get_data(&docum), 1, docum.len, bson);
}

bson_t recurrent_read_recurse(bson_iter_t iter, int pathcond_count, int pathcond_length, path_t **pathconds) {
    bson_t inn;
    bson_init(&inn);

    while (bson_iter_next(&iter)) {
        if (BSON_ITER_HOLDS_UTF8 (&iter)) {
            if (pathcond_count == pathcond_length)
                if (pathcond_length > 0)
                    if (pathconds[pathcond_count - 1]->cond_n > 0) {
                        for (int i = 0; i < pathconds[pathcond_count - 1]->cond_n; i++)
                            if (strcmp(pathconds[pathcond_count - 1]->conditions[i]->key, bson_iter_key(&iter)) == 0)
                                bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);
                    } else bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);
        } else if (BSON_ITER_HOLDS_INT(&iter)) {
            if (pathcond_count == pathcond_length)
                if (pathcond_length > 0)
                    if (pathconds[pathcond_count - 1]->cond_n > 0) {
                        for (int i = 0; i < pathconds[pathcond_count - 1]->cond_n; i++)
                            if (strcmp(pathconds[pathcond_count - 1]->conditions[i]->key, bson_iter_key(&iter)) == 0)
                                bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));
                    } else bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));


        } else if (BSON_ITER_HOLDS_DOCUMENT(&iter)) {
            bson_t *b_res;
            uint32_t object_len;
            const uint8_t *object_buf;

            bson_iter_document(&iter, &object_len, &object_buf);
            b_res = bson_new_from_data(object_buf, object_len);
            bson_iter_t child;
            bson_iter_init(&child, b_res);
            int fl;
            int ind_fl = 1;
            if (pathcond_count == pathcond_length) {
                ind_fl = 0;
            }

            if (pathcond_length > 0) {
                if ((strcmp(pathconds[ind_fl ? pathcond_count : 0]->actualPath, bson_iter_key(&iter)) == 0) ||
                    (strcmp(pathconds[ind_fl ? pathcond_count : 0]->actualPath, "*") == 0)) {
                    if (pathconds[ind_fl ? pathcond_count : 0]->cond_n == 0) {
                        fl = 1;
                    } else if (is_cond(*b_res, pathconds[ind_fl ? pathcond_count : 0]->cond_n,
                                       pathconds[ind_fl ? pathcond_count : 0]->conditions,
                                       pathconds[ind_fl ? pathcond_count : 0]->operators)) {
                        fl = 1;
                        printf("yes");
                    } else fl = 0;
                } else fl = 0;
            } else fl = 0;

            bson_t inn_inn;
            if (ind_fl == 0)
                if (fl == 0)
                    inn_inn = recurrent_read_recurse(child, 0, pathcond_length, pathconds);
                else
                    inn_inn = recurrent_read_recurse(child, pathcond_count, pathcond_length, pathconds);
            else if (fl == 0)
                inn_inn = recurrent_read_recurse(child, 0, pathcond_length, pathconds);
            else
                inn_inn = recurrent_read_recurse(child, pathcond_count + 1, pathcond_length, pathconds);

            bson_iter_t inn_inn_iter;
            bson_iter_init(&inn_inn_iter, &inn_inn);

            if (bson_iter_next(&inn_inn_iter)) {
                bson_append_document(&inn, bson_iter_key(&iter), -1, &inn_inn);
            }
            bson_destroy(&inn_inn);
            bson_destroy(b_res);
        }
    }
    return inn;
}

bson_t recurrent_read_root(bson_iter_t iter, int pathcond_count, int pathcond_length, path_t **pathconds) {
    bson_t inn;
    bson_init(&inn);
    int end = 0;

    while (bson_iter_next(&iter)) {
        if (BSON_ITER_HOLDS_UTF8 (&iter)) {
            if (pathcond_count == pathcond_length)
                if (pathcond_length > 0)
                    if (pathconds[pathcond_count - 1]->cond_n > 0) {
                        for (int i = 0; i < pathconds[pathcond_count - 1]->cond_n; i++)
                            if (strcmp(pathconds[pathcond_count - 1]->conditions[i]->key, bson_iter_key(&iter)) == 0)
                                bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);
                    } else bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);

        } else if (BSON_ITER_HOLDS_INT(&iter)) {
            if (pathcond_count == pathcond_length)
                if (pathcond_length > 0)
                    if (pathconds[pathcond_count - 1]->cond_n > 0) {
                        for (int i = 0; i < pathconds[pathcond_count - 1]->cond_n; i++)
                            if (strcmp(pathconds[pathcond_count - 1]->conditions[i]->key, bson_iter_key(&iter)) == 0)
                                bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));
                    } else bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));

        } else if (BSON_ITER_HOLDS_DOCUMENT(&iter)) {
            bson_t *b_res;
            uint32_t object_len;
            const uint8_t *object_buf;

            bson_iter_document(&iter, &object_len, &object_buf);
            b_res = bson_new_from_data(object_buf, object_len);
            bson_iter_t child;
            bson_iter_init(&child, b_res);
            int fl = 1;
            int ind_fl = 1;
            if (pathcond_count == pathcond_length) {
                ind_fl = 0;
                end = 1;
            }

            if (pathcond_length > 0) {
                if ((strcmp(pathconds[ind_fl ? pathcond_count : 0]->actualPath, bson_iter_key(&iter)) == 0) ||
                    (strcmp(pathconds[ind_fl ? pathcond_count : 0]->actualPath, "*") == 0)) {

                    if (pathconds[ind_fl ? pathcond_count : 0]->cond_n == 0) {
                        fl = 1;
                    } else if (is_cond(*b_res, pathconds[ind_fl ? pathcond_count : 0]->cond_n,
                                       pathconds[ind_fl ? pathcond_count : 0]->conditions,
                                       pathconds[ind_fl ? pathcond_count : 0]->operators)) {
                        fl = 1;
                    } else fl = 0;
                } else fl = 0;
            } else fl = 0;

            bson_t inn_inn;
            if (ind_fl == 0)
                if (fl == 0)
                    inn_inn = recurrent_read_recurse(child, 0, pathcond_length, pathconds);
                else
                    inn_inn = recurrent_read_recurse(child, pathcond_count, pathcond_length, pathconds);
            else if (fl == 0)
                inn_inn = recurrent_read_recurse(child, 0, pathcond_length, pathconds);
            else
                inn_inn = recurrent_read_recurse(child, pathcond_count + 1, pathcond_length, pathconds);

            bson_iter_t inn_inn_iter;
            bson_iter_init(&inn_inn_iter, &inn_inn);

            if (bson_iter_next(&inn_inn_iter) && end == 0) {
                bson_append_document(&inn, bson_iter_key(&iter), -1, &inn_inn);
            }
            bson_destroy(&inn_inn);
            bson_destroy(b_res);
        }
    }
    return inn;
}

char *apiRead(int pathcond_length, path_t **pathconds, int from_root, FILE *new_stream) {
    bson_t *b = read_bson_();
    char *buff = bson_as_canonical_extended_json(b, NULL);
    bson_free(buff);
    bson_iter_t iter;
    bson_iter_init(&iter, b);

    bson_t res;
    if (from_root != 0) res = recurrent_read_root(iter, 0, pathcond_length, pathconds);
    else res = recurrent_read_recurse(iter, 0, pathcond_length, pathconds);
    return bson_as_canonical_extended_json(&res, NULL);
}

bson_t recurrent_create(bson_iter_t iter, char *path, char *path_eq, int elem_length, elem_t *elems[]) {
    bson_t inn;
    bson_init(&inn);

    while (bson_iter_next(&iter)) {
        if (BSON_ITER_HOLDS_UTF8 (&iter)) {
            bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);
        } else if (BSON_ITER_HOLDS_INT (&iter)) {
            bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));
        } else {
            bson_t *b_res;
            uint32_t object_len;
            const uint8_t *object_buf;

            bson_iter_document(&iter, &object_len, &object_buf);
            b_res = bson_new_from_data(object_buf, object_len);
            bson_iter_t child;
            bson_iter_init(&child, b_res);

            char full_path[2048];
            bzero(full_path, 2048);
            if (strcmp(path, "") != 0) {
                strcpy(full_path, path);
                strcat(full_path, ".");
            }
            strcat(full_path, bson_iter_key(&iter));
            bson_t inner_inner = recurrent_create(child, full_path, path_eq, elem_length, elems);
            bson_append_document(&inn, bson_iter_key(&iter), -1, &inner_inner);
        }

        if (strcmp(path, path_eq) == 0) {
            for (int i = 0; i < elem_length; i++) {
                if (strcmp(bson_iter_key(&iter), elems[i]->key) != 0) {
                    printf("creating->%s\n", elems[i]->key);
                    if (strcmp(elems[i]->type, "utf8") == 0)
                        bson_append_utf8(&inn, elems[i]->key, -1, elems[i]->value_utf8, -1);
                    if (strcmp(elems[i]->type, "int") == 0)
                        bson_append_int32(&inn, elems[i]->key, -1, elems[i]->value_int);
                    if (strcmp(elems[i]->type, "doc") == 0) {
                        bson_t b_inserted;
                        bson_init(&b_inserted);
                        bson_append_document(&inn, elems[i]->key, -1, &b_inserted);
                    }
                }
            }
        }
    }
    return inn;
}

char *apiCreate(char *path, int elem_length, elem_t **elems, FILE *new_stream) {
    bson_t *b = read_bson_();
    bson_iter_t iter;
    bson_iter_init(&iter, b);
    bson_iter_t baz;
    if (strcmp(path, "") != 0)
        if (!(bson_iter_init(&iter, b) && bson_iter_find_descendant(&iter, path, &baz)))
            return "{\"error\": \"path_is_invalid\"}";
    if (bson_iter_init(&iter, b) && bson_iter_find_descendant(&iter, path, &baz) && (BSON_ITER_HOLDS_UTF8 (&baz)))
        return "{\"error\": \"creating node inside node\"}";

    bson_iter_init(&iter, b);
    bson_t result = recurrent_create(iter, "", path, elem_length, elems);
    write_bson(result);
    return "{\"status\": \"ok\"}";
}

bson_t recurrent_delete(bson_iter_t iter, char *path, char *path_eq) {
    bson_t inn;
    bson_init(&inn);

    while (bson_iter_next(&iter)) {
        char full_path[2048];
        bzero(full_path, 2048);
        if (strcmp(path, "") != 0) {
            strcpy(full_path, path);
            strcat(full_path, ".");
        }
        strcat(full_path, bson_iter_key(&iter));

        if (strcmp(path, path_eq) == 0) continue;
        if (strcmp(full_path, path_eq) == 0) continue;

        if (BSON_ITER_HOLDS_UTF8 (&iter)) {
            bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);
        } else if (BSON_ITER_HOLDS_INT (&iter)) {
            bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));
        } else {
            bson_t *b_res;
            uint32_t object_len;
            const uint8_t *object_buf;
            bson_iter_document(&iter, &object_len, &object_buf);
            b_res = bson_new_from_data(object_buf, object_len);
            bson_iter_t child;
            bson_iter_init(&child, b_res);
            bson_t inner_inner = recurrent_delete(child, full_path, path_eq);
            bson_append_document(&inn, bson_iter_key(&iter), -1, &inner_inner);
        }
    }
    return inn;
}

char *apiDelete(char *path, FILE *new_stream) {
    bson_t *b = read_bson_();
    bson_iter_t iter;
    bson_iter_init(&iter, b);
    bson_iter_t baz;
    if (strcmp(path, "") != 0)
        if (!((bson_iter_init(&iter, b) && bson_iter_find_descendant(&iter, path, &baz))))
            return "{\"error\": \"does not exists\"}";

    bson_iter_init(&iter, b);
    bson_t result = recurrent_delete(iter, "", path);
    write_bson(result);
    return "{\"status\": \"ok\"}";
}

bson_t recurrent_update(bson_iter_t iter, char *path, char *path_eq, int elem_length, elem_t *elems[]) {
    bson_t inn;
    bson_init(&inn);

    while (bson_iter_next(&iter)) {
        if (BSON_ITER_HOLDS_UTF8 (&iter)) {
            int fl = 0;
            char *new_val = "";
            for (int i = 0; i < elem_length; i++)
                if (strcmp(bson_iter_key(&iter), elems[i]->key) == 0) {
                    fl = 1;
                    new_val = elems[i]->value_utf8;
                }
            if ((fl == 1) && (strcmp(path, path_eq) == 0))
                bson_append_utf8(&inn, bson_iter_key(&iter), -1, new_val, -1);
            else
                bson_append_utf8(&inn, bson_iter_key(&iter), -1, bson_iter_utf8(&iter, NULL), -1);
        } else if (BSON_ITER_HOLDS_INT (&iter)) {
            int fl = 0;
            int new_val = 0;
            for (int i = 0; i < elem_length; i++)
                if (strcmp(bson_iter_key(&iter), elems[i]->key) == 0) {
                    fl = 1;
                    new_val = elems[i]->value_int;
                }
            if ((fl == 1) && (strcmp(path, path_eq) == 0))
                bson_append_int32(&inn, bson_iter_key(&iter), -1, new_val);
            else
                bson_append_int32(&inn, bson_iter_key(&iter), -1, bson_iter_int32(&iter));
        } else {
            bson_t *b_res;
            uint32_t object_len;
            const uint8_t *object_buf;
            bson_iter_document(&iter, &object_len, &object_buf);
            b_res = bson_new_from_data(object_buf, object_len);
            bson_iter_t child;
            bson_iter_init(&child, b_res);

            char full_path[2048];
            bzero(full_path, 2048);
            if (strcmp(path, "") != 0) {
                strcpy(full_path, path);
                strcat(full_path, ".");
            }
            strcat(full_path, bson_iter_key(&iter));
            bson_t inner_inner = recurrent_update(child, full_path, path_eq, elem_length, elems);
            bson_append_document(&inn, bson_iter_key(&iter), -1, &inner_inner);
        }
    }
    return inn;
}

char *apiUpdate(char *path, int elem_length, elem_t **elems, FILE *new_stream) {
    bson_t *b = read_bson_();
    printf("new_path=%s\n", path);
    bson_iter_t iter;
    bson_iter_init(&iter, b);
    bson_iter_t baz;
    if (!(bson_iter_init(&iter, b) && bson_iter_find_descendant(&iter, path, &baz)))
        return "{\"error\": \"path_is_invalid\"}";

    bson_iter_init(&iter, b);
    bson_t result = recurrent_update(iter, "", path, elem_length, elems);
    write_bson(result);
    return "{\"status\": \"ok\"}";
}