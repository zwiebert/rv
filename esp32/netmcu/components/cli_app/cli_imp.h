#pragma once
#include "cli/cli.h"

extern const char pin_state_args[];


#define NODEFAULT() if (val==0) return cli_replyFailure()


void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);
//void cli_out_set_x(const char *obj_tag);
//void cli_out_close(void);
//void cli_out_x_reply_entry(const char *key, const char *val, int len);

bool config_receiver(const char *val);
bool config_transmitter(const char *val);

bool asc2u8(const char *s, uint8_t *n, uint8_t limit);



