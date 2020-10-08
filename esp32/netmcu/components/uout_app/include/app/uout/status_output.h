/*
 * status_output.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_STATUS_OUTPUT_H_
#define USER_MAIN_STATUS_OUTPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "uout/status_output.h"
#include "so_msg.h"
#include "so_config.h"
#include "so_types.h"


#define SO_CFG_size (SO_CFG_end - SO_CFG_begin - 1)


void so_output_message(const struct TargetDesc &td, so_msg_t mt, const void *arg);
so_msg_t so_parse_config_key(const char *k);


#endif /* USER_MAIN_STATUS_OUTPUT_H_ */
