/**
 * \file   cli/cli.h
 * \brief  Public CLI header.
 */

#ifndef CLI_H_
#define CLI_H_

#ifdef __cplusplus
extern "C" {
#endif


char *get_commandline(void);
void cli_loop(void);

#ifdef __cplusplus
}
#endif

#endif /* CLI_H_ */
