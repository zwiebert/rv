#define UNITY_SUPPORT_TEST_CASES
#include "../test_runner/test_runner.h";
#include "unity.h"
//#include "cli/cli_json.c"


char *json_get_command_object(char *s, char **ret_name, char **next_name);

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define postcond(x) assert((x))
#define ICACHE_FLASH_ATTR
int msgid;

typedef struct {
  char *key;
  char *val;
} clpar;
#define MAX_PAR 20
struct cli_parm {
  clpar *par;
  unsigned size;
};


#define NAME      "timer"
#define CMD_OBJ   "\"g\":1,\"m\":0,\"f\":\"imDwArs\",\"daily\":\"0700\",\"astro\":15"
#define NAME2     "config"
#define CMD_OBJ2  "\"cu\": \"801234\",\"baud\": 115200,\"longitude\": 13.5,\"latitude\": 52.6,\"timezone\": 1.0,\"tz\": \"\""

#define JSON "{\"to\":\"tfmcu\",\"" NAME "\":{" CMD_OBJ "},\"" NAME2 "\":{" CMD_OBJ2 "}" "}";

static void test_json() {
  char *name, *cmd_obj, *next_name;
  int i;

  char json[] = JSON;

  cmd_obj = json_get_command_object(json, &name, &next_name);
  TEST_ASSERT_EQUAL_STRING(NAME, name);
  TEST_ASSERT_EQUAL_STRING(CMD_OBJ, cmd_obj);
  TEST_ASSERT_NOT_NULL(next_name);

  //printf("name: %s, obj: %s\n", name, cmd_obj);

  if (cmd_obj) {
    clpar cli_par[20] = {};
    struct cli_parm clp = { .par = cli_par, .size = 20 };
    int len = parse_json(name, cmd_obj, &clp);
    i=0;
    TEST_ASSERT_EQUAL_STRING(NAME, cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("g", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("1", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("m", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("0", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("f", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("imDwArs", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("daily", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("0700", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("astro", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("15", cli_par[i].val);
    ++i;



    cmd_obj = json_get_command_object(next_name, &name, &next_name);
    TEST_ASSERT_EQUAL_STRING(NAME2, name);
    TEST_ASSERT_EQUAL_STRING(CMD_OBJ2, cmd_obj);

    cmd_obj = json_get_command_object(next_name, &name, &next_name);
    TEST_ASSERT_NULL(cmd_obj);

  }
}



TEST_CASE("json", "[cli]")
{
     test_json();
}
