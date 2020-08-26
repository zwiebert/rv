/*
 * cli_json.c
 *
 *  Created on: 28.03.2019
 *      Author: bertw
 */

#ifndef TEST_HOST
#include "user_config.h"
#include "string.h"
#include "cli_imp.h"
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "debug/debug.h"
#define postcond(x)
#define dbg_vpf(x) x
#define D(x) x
uint16_t msgid;

#else
#include "cli_imp.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define postcond(x) assert((x))
#define dbg_vpf(x)
#define D(x) x
#define reply_failure()
#define db_printf printf
#endif


#include <jsmn/jsmn.h>

#define DB_NO_OPT

#ifdef DB_NO_OPT
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif

static char *stringFromToken(char *json, const jsmntok_t *tok) {
  json[tok->end] = '\0';
  return json + tok->start;
}


int handle_parm_json(char *json, jsmntok_t *tok, const char *name) {
  const int oc = tok[0].size;
  ++tok;
  for (int i = 0; i < oc; ++i) {
    if (tok[i].type == JSMN_OBJECT) {
      handle_parm_json(json, &tok[i], stringFromToken(json, &tok[i - 1]));
    } else {

    }
  }
  return 0;
}



int cli_process_json(char *json, process_parm_cb proc_parm) {
 // dbg_vpf(db_printf("process_json: %s\n", json));

  jsmn_parser jsp;
#define MAX_TOKENS 64
  jsmntok_t tok[MAX_TOKENS];

  jsmn_init(&jsp);
  int nt = jsmn_parse(&jsp, json, strlen(json), tok, MAX_TOKENS);
  if (nt < 0) {
    return nt;
  }

  if (tok[0].type == JSMN_OBJECT) {
    for (int i = 1; i < nt; ++i) {

      if (tok[i].type == JSMN_OBJECT) {
        int coi = i; // command object index
        int n_childs = tok[i].size;

        char *cmd_obj = "";
        if (tok[i - 1].type == JSMN_STRING) {
          cmd_obj = stringFromToken(json, &tok[i - 1]);
        }

        D(db_printf("cmd_obj: %s\n", cmd_obj));

        if (strcmp(cmd_obj, "json") == 0) {
          int n = handle_parm_json(json, tok, cmd_obj);
          i += n;
        } else {
          clpar par[nt];
          int pi = 0;

          par[pi].key = cmd_obj;
          par[pi++].val = "";

          for (i = coi + 1; i < nt && n_childs > 0; ++i) {
            par[pi].key = stringFromToken(json, &tok[i++]);
            par[pi].val = stringFromToken(json, &tok[i]);
            ++pi;
            --n_childs;
          }
          D(db_printf("proc_parm: %s\n", par->key));
          proc_parm(par, pi);
        }
      }
    }
  }
  return 0;
}
#ifdef DB_NO_OPT
#pragma GCC pop_options
#endif
