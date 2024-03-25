/**
 * \file cli/cli_imp.h
 * \brief Private CLI header
 */

#ifndef cli_imp_h_
#define cli_imp_h_
#include "cli.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_BUF_SIZE 512
extern char cmd_buf[CMD_BUF_SIZE];

// implementation interface for  cli*.c files
extern const char pin_state_args[];


typedef struct {
  char *key;
  char *val;
} clpar;
#define MAX_PAR 20
extern clpar par[MAX_PAR];

struct cli_parm {
  clpar *par;
  size_t size;
};

extern const char help_parmCmd[] ;  ///< help string for cli command cmd (\ref process_parmCmd)
extern const char help_parmTimer[]; ///< help string for cli command timer (\ref process_parmTimer)
extern const char help_parmConfig[] ; ///< help string for cli command config (\ref process_parmConfig)
extern const char help_parmMcu[] ; ///< help string for cli command mcu (\ref process_parmMcu)
extern const char help_parmKvs[] ; ///< help string for cli command timer (\ref process_parmTimer)
extern const char help_parmHelp[] ; ///< help string for cli command timer (\ref process_parmKvs)
extern const char help_None[] ;  ///< default help string for cli commands

extern uint16_t msgid;


#define NODEFAULT() if (val==0) return reply_failure()


/* cli.c */
void print_enr(void);
void msg_print(const char *msg, const char *tag);
void reply_print(const char *tag);
void reply_message(const char *tag, const char *msg);
void reply_id_message(uint16_t id, const char *tag, const char *msg);
void cli_out_timer_reply_entry(const char *key, const char *val, int len);
void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);

/**
 * \brief handler for cli command "cmd"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmCmd(clpar p[], int len);
/**
 * \brief handler for cli command "config"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmConfig(clpar p[], int len);
/**
 * \brief handler for cli command "mcu"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmMcu(clpar p[], int len);
/**
 * \brief handler for cli command "timer"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmTimer(clpar p[], int len);
/**
 * \brief handler for cli command "help"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmHelp(clpar p[], int len);
/**
 * \brief handler for cli command "protobuf"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmProtoBuf(clpar p[], int len);
/**
 * \brief handler for cli command "kvs"
 * \param p   array of key/value pairs.
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parmKvs(clpar p[], int len);
/* cli.c */
void warning_unknown_option(const char *key);
void cli_msg_ready(void);

/**
 * \brief Get command line sent by companion MCU from USART
 * \return  pointer to static buffer containing the received command line
 */
char *get_commandline(void);

/**
 * \brief
 * \param s  string equal to "0", "1" or null
 * \return 0 for "0", 1 for "1" or null,  -1 for non-matching string
 */
int asc2bool(const char *s);
/**
 * \brief Parse command line and write key/val pairs to \ref par
 * \return number of key/val pairs written to \ref par
 */
int parse_commandline(char *cl);
void reply_success(void);
int reply_failure(void);


bool reply(bool success);
/**
 * \brief calls handler for cli command
 * \param p   array of key/value pairs, which also contains the command name
 *            index 0 contains the command name in the key (value is empty).
 *            indexes [1...len) are holding the parameter key/value pairs
 * \param len length of array p
 * \return  0 on success, -1 on error
 */
int process_parm(clpar p[], int len);
/**
 * \brief parses CLI command line and calls command handler
 * \param line   valid command line
 */
void cli_process_cmdline(char *line);
/**
 * \brief  Do work. Should be called periodically.
 *     - calls \ref get_commandline
 *     - calls \ref watchDog_checkCommandLine
 *     - calls \ref cli_process_json or \ref cli_process_cmdline
 */
void cli_loop(void);

typedef int (*process_parm_cb)(clpar parm[], int parm_len);
/**
 * \brief Parse JSON and write command name and key/val pairs to \ref par
 * \param json       JSON string to parse
 * \param proc_parm  cli command handler to call for the parsed data
 * \return 0
 */
int cli_process_json(char *json, process_parm_cb proc_parm) ;

#ifdef __cplusplus
}
#endif
#endif
