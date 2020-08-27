/*
 * cli.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
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
