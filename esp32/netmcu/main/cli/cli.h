/*
 * cli.h
 *
 * Created: 24.08.2017 21:46:06
 *  Author: bertw
 */


#ifndef CLI_H_
#define CLI_H_

#include "user_config.h"

// user interface
char *get_commandline(void);
char *set_commandline(const char *src, uint8_t len); // copy src  (e.g. from mqtt event) to writable buffer
void cli_process_cmdline(char *line);
void cli_process_json(char *json);
void cli_loop(void);

#endif /* CLI_H_ */
