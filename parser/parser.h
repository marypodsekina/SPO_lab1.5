#ifndef TEST_CMD_PARSER_H
#define TEST_CMD_PARSER_H

#include <json-c/json_object.h>
#include "../mt.h"
#include "../util/util.h"

api_t *parse_cmd_to_api_struct(const char *in);

#endif
