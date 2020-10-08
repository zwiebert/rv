#include "../test_runner/test_runner.h"
#include "unity.h"
#include "cli/cli_imp.h"


char *json_get_command_object(char *s, char **ret_name, char **next_name);

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define postcond(x) assert((x))
uint16_t msgid;

#define MAX_PAR 20


#define NAME      "timer"
#define CMD_OBJ   "\"g\":1,\"m\":0,\"f\":\"imDwArs\",\"daily\":\"0700\",\"astro\":15"
#define NAME2     "config"
#define CMD_OBJ2  "\"cu\": \"801234\",\"baud\": 115200,\"longitude\": 13.5,\"latitude\": 52.6,\"timezone\": 1.0,\"tz\": \"\""
#define NAME3     "cmd"
#define CMD_OBJ3  "\"dur0.0\":\"60,3,0,0,0,0,0,0\""

#define JSON "{\"to\":\"tfmcu\",\"" NAME "\":{" CMD_OBJ "},\"" NAME2 "\":{" CMD_OBJ2 "}" ",\"" NAME3 "\":{" CMD_OBJ3  "}" "}";
//#define JSON "{\"to\":\"tfmcu\",\"" NAME "\":{" CMD_OBJ "},\"" NAME2 "\":{" CMD_OBJ2 "}" "}";

static int count = 0;

static int test_process_parm(clpar cli_par[], int parm_len) {
  int i=0;

  if (count == 0) {
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
  } else if (count == 1) {
    TEST_ASSERT_EQUAL_STRING(NAME2, cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("cu", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("801234", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("baud", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("115200", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("longitude", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("13.5", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("latitude", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("52.6", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("timezone", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("1.0", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("tz", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("", cli_par[i].val);
    ++i;
  } else if (count == 2) {
    TEST_ASSERT_EQUAL_STRING(NAME3, cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("", cli_par[i].val);
    ++i;
    TEST_ASSERT_EQUAL_STRING("dur0.0", cli_par[i].key);
    TEST_ASSERT_EQUAL_STRING("60,3,0,0,0,0,0,0", cli_par[i].val);
    ++i;
  }

  ++count;
}

static void test_json() {
  char json[] = JSON;
  count = 0;
  cli_process_json(json, test_process_parm);
}



void setUp() {

}

void tearDown() {

}





TEST_CASE("json", "[cli]")
{
     test_json();
     test_json();
}

