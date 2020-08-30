/*
 * cli_app.h
 *
 *  Created on: 19.02.2020
 *      Author: bertw
 */

#ifndef CLI_CLI_APP_H_
#define CLI_CLI_APP_H_

#include "cli_imp.h"

extern "C" {
int process_parmConfig(clpar p[], int len);
int process_parmHelp(clpar p[], int len);
int process_parmCmd(clpar p[], int len);
int process_parmMcu(clpar p[], int len);
int process_parmKvs(clpar p[], int len);
int process_parmStatus(clpar p[], int len);
int process_parmProtoBuf(clpar p[], int len);
}

bool cli_checkStm32CommandLine(char *line);

void cliApp_setup();

#endif /* CLI_CLI_APP_H_ */
