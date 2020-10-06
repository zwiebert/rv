/**
 * \file   cli/cli_app.h
 * \brief  CLI main header file
 */

#ifdef __cplusplus
extern "C" {
#endif
#pragma once

#include <cli/cli_types.h>

extern const char cli_help_parmKvs[];
extern const char cli_help_parmCmd[];
extern const char cli_help_parmConfig[];
extern const char cli_help_parmMcu[];
extern const char cli_help_parmStatus[];
extern const char cli_help_parmHelp[];


extern "C" {
int process_parmConfig(clpar p[], int len, const struct TargetDesc &td);
int process_parmHelp(clpar p[], int len, const struct TargetDesc &td);
int process_parmCmd(clpar p[], int len, const struct TargetDesc &td);
int process_parmMcu(clpar p[], int len, const struct TargetDesc &td);
int process_parmKvs(clpar p[], int len, const struct TargetDesc &td);
int process_parmStatus(clpar p[], int len, const struct TargetDesc &td);
int process_parmProtoBuf(clpar p[], int len, const struct TargetDesc &td);
}


bool cli_checkStm32CommandLine(char *line);

void cliApp_setup();

#ifdef __cplusplus
}
#endif
