#if 0
/*
 * cli.c
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#include "cli.h"
#include "cli_imp.h"
#include "debug/debug.h"



int ICACHE_FLASH_ATTR
process_parmTimer(clpar p[], int len) {
	for (int i = 0; i < len; ++i) {
		db_printf("key=%s, val=%s\n", p[i].key, p[i].val);
	}

return 0;
}

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = {

                      { "timer", process_parmTimer, 0 },
  };




int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
  int i;

db_puts("process parm");
  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  return 0;
}


int reply_failure(void) { return 0;}
uint16_t msgid;


#else

/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */
#include "user_config.h"

#include "debug/debug.h"
#include "config/config.h"
#include "cli_imp.h"
#include "userio/status_output.h"
#include "userio/inout.h"

#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)
#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

uint16_t msgid;

const char *Obj_tag="";
#define SET_OBJ_TAG(tag) Obj_tag=(tag)
#define OBJ_TAG (Obj_tag+0)
#define OBJ_TAG_TIMER "timer"
#define OBJ_TAG_CONFIG "config"
#define OBJ_TAG_MCU "mcu"
#define OBJ_TAG_SEND "send"


static void ICACHE_FLASH_ATTR cli_out_top_tag(void) {
    io_puts("tf: ");
}

static void ICACHE_FLASH_ATTR cli_out_reply_tag(void) {
  if (msgid) {
    io_puts("reply="), io_putd(msgid), io_puts(": ");
  } else {
    io_puts("reply: ");
  }
}

static void ICACHE_FLASH_ATTR cli_out_obj_tag(void) {
    io_puts(OBJ_TAG), io_puts(":");
}

static void ICACHE_FLASH_ATTR cli_out_start_reply(void) {
  cli_out_top_tag();
  cli_out_reply_tag();
  cli_out_obj_tag();
}


static void ICACHE_FLASH_ATTR cli_out_start_timer_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_TIMER);
  cli_out_start_reply();
}


static void ICACHE_FLASH_ATTR cli_out_start_config_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_CONFIG);
  cli_out_start_reply();
}

static void ICACHE_FLASH_ATTR cli_out_start_mcu_reply(void) {
  SET_OBJ_TAG(OBJ_TAG_MCU);
  cli_out_start_reply();
}

typedef void (*void_fun_ptr)(void);
const int OUT_MAX_LEN = 80;

static void ICACHE_FLASH_ATTR cli_out_entry(void_fun_ptr tag, const char *key, const char *val, int len) {
  static int length;

  if (!key || len == -1) {
    if (length) {
      io_puts(";\n");
      length = 0;
    }
  } else {
    if (key)
      len += strlen(key) + 1;
    if (val)
      len += strlen(val);

    if ((length + len) > OUT_MAX_LEN && length > 0) {
      io_puts(";\n");
      length = 0;
    }
    if (length == 0) {
      tag();
    }
    length += len;
    if (key) {
      io_putc(' '), io_puts(key), io_putc('=');
    }
    if (val) {
      io_puts(val);
    }
  }
}

void cli_out_set_config(void) {
  SET_OBJ_TAG(OBJ_TAG_CONFIG);
}
void cli_out_set_x(const char *obj_tag) {
  SET_OBJ_TAG(obj_tag);
}

void ICACHE_FLASH_ATTR cli_out_x_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_reply, key, val, len);
}

void ICACHE_FLASH_ATTR cli_out_timer_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_timer_reply, key, val, len);
}

void ICACHE_FLASH_ATTR cli_out_config_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_config_reply, key, val, len);
}

void ICACHE_FLASH_ATTR cli_out_mcu_reply_entry(const char *key, const char *val, int len) {
  if (!so_tgt_test_cli_text())
    return;
  cli_out_entry(cli_out_start_mcu_reply, key, val, len);
}

int ENR; // error number
void ICACHE_FLASH_ATTR print_enr(void) {
  io_puts("enr: "), io_putd(ENR), io_putlf();
}

void ICACHE_FLASH_ATTR msg_print(const char *msg, const char *tag) {
  if (!so_tgt_test_cli_text())
    return;
  if (msg)
    io_puts(msg);
  if (msgid) {
    io_putc('@');
    io_putd(msgid);
  }
  if (tag) {
    io_putc(':');
    io_puts(tag);
  }
  io_puts(": ");
}

void ICACHE_FLASH_ATTR warning_unknown_option(const char *key) {
  if (!so_tgt_test_cli_text())
    return;
  msg_print("warning", "unknown-option"), io_puts(key), io_putc('\n');
}

void ICACHE_FLASH_ATTR reply_print(const char *tag) {
  if (!so_tgt_test_cli_text())
    return;
  msg_print("reply", tag);
}

void ICACHE_FLASH_ATTR reply_message(const char *tag, const char *msg) {
  if (!so_tgt_test_cli_text())
    return;
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
}

void ICACHE_FLASH_ATTR cli_msg_ready(void) {
  if (!so_tgt_test_cli_text())
    return;
  io_puts("\nready:\n");
}

void ICACHE_FLASH_ATTR reply_id_message(uint16_t id, const char *tag, const char *msg) {
  uint16_t old_id = msgid;
  if (!so_tgt_test_cli_text())
    return;

  msgid = id;
  reply_print(tag);
  if (msg)
    io_puts(msg);
  io_putlf();
  msgid = old_id;
}






int ICACHE_FLASH_ATTR
asc2bool(const char *s) {
  if (!s)
    return 1; // default value for key without value

  switch (*s) {
  case '0':
    return 0;
  case '1':
    return 1;
  default:
    return -1;
  }

}

void ICACHE_FLASH_ATTR
reply_success() {
  reply_message(0, "ok");
}

int ICACHE_FLASH_ATTR
reply_failure() {
  reply_message(0, "error");
  return -1;
}





bool ICACHE_FLASH_ATTR
reply(bool success) {
  if (success)
    reply_success();
  else
    reply_failure();
  return success;
}


int process_parmHelp(clpar p[], int len);

const char help_None[]  = "none";

const char help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";

struct {
  const char *parm;
  int (*process_parmX)(clpar p[], int len);
  const char *help;
} parm_handlers[] = {
        { "config", process_parmConfig, help_parmConfig },
        { "help", process_parmHelp, help_parmHelp },
        { "cmd", process_parmCmd, help_parmCmd },
        { "mcu", process_parmMcu, help_parmMcu },

        { "kvs", process_parmKvs, help_parmKvs },
#if 0
{ "send", process_parmSend, help_parmSend },





                      { "timer", process_parmTimer, help_parmTimer },
                      { "auto", process_parmTimer, help_parmTimer }, // alias

#if ENABLE_EXPERT
                      { "expert", process_parmExpert, help_None},
#endif
#ifdef USE_PAIRINGS
                      { "pair", process_parmPair, help_parmPair},
#endif
#endif
  };

int ICACHE_FLASH_ATTR
process_parm(clpar p[], int len) {
  int i;
  dbg_vpf(db_printf("process_parm: len=%d\n", len));

  for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
    if (strcmp(p[0].key, parm_handlers[i].parm) == 0)
      return parm_handlers[i].process_parmX(p, len);
  }
  dbg_vpf(db_printf("error: no parm_handler for <%s>\n", p[0].key));
  return 0;
}

void ICACHE_FLASH_ATTR
cli_process_cmdline(char *line) {
  dbg_vpf(db_printf("process_cmdline: %s\n", line));
  int n = parse_commandline(line);
  if (n < 0) {
    reply_failure();
  } else {
    process_parm(par, n);
  }
}

int ICACHE_FLASH_ATTR
process_parmHelp(clpar p[], int len) {
  int i;

  static const char usage[]  = "syntax: command option=value ...;\n"
      "commands are: ";

  io_putlf();

  // print help for help;
  if (len == 1) {
    io_puts(help_parmHelp);
    for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
      io_puts(parm_handlers[i].parm), io_puts(", ");
    }
    io_putlf();
    return 0;
  }

  // print help for help command; or help all;
  for (i = 1; i < len; ++i) {
    int k;
    const char *key = p[i].key; //, *val = p[i].val;
    bool keyProcessed = false;

    if (strcmp(key, "all") == 0) {
      io_puts(usage);
      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(", ");
      }
      io_putlf();

      for (i = 0; i < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++i) {
        io_puts(parm_handlers[i].parm), io_puts(" options:\n");
        io_puts(parm_handlers[i].help), io_putlf();
      }

    } else {

      for (k = 0; k < (sizeof(parm_handlers) / sizeof(parm_handlers[0])); ++k) {
        if (strcmp(parm_handlers[k].parm, key) == 0) {
          io_puts(parm_handlers[k].help), io_putlf();
          keyProcessed = true;
          break;
        }
      }

      if (!keyProcessed) {
        warning_unknown_option(key);
      }
    }
  }

  io_puts("\ncommon options:\n"
      "mid=N  N is used as an ID in the reply\n");

  return 0;
}

void ICACHE_FLASH_ATTR cli_loop(void) {
  char *cmdline;
  static bool ready;
  if ((cmdline = get_commandline())) {
    so_tgt_set(SO_TGT_CLI);
    if (cmdline[0] == '{') {
     cli_process_json(cmdline);
    } else {
      io_putlf();
      cli_process_cmdline(cmdline);
      cli_msg_ready();
    }
    so_tgt_default();
  } else if (!ready) {
    cli_msg_ready();
    ready = true;
  }
}

#if TEST_MODULE_CLI
bool ICACHE_FLASH_ATTR
testModule_cli()
{
  char cl[] = "timer g=5 astro=0;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = parse_commandline(cl);
  if (n > 0)
  process_parm(par, n);

  return n != -1;
}
#endif
#endif
