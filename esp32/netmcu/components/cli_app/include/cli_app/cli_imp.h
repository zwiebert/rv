#ifndef cli_imp_h_
#define cli_imp_h_
#include "cli/cli.h"

extern const char pin_state_args[];


extern const char cli_help_parmKvs[];
extern const char cli_help_parmCmd[] ;
extern const char cli_help_parmConfig[] ;
extern const char cli_help_parmMcu[] ;
extern const char cli_help_parmStatus[] ;
extern const char cli_help_parmHelp[] ;

extern "C" {
int process_parmConfig(clpar p[], int len);
int process_parmHelp(clpar p[], int len);
int process_parmCmd(clpar p[], int len);
int process_parmMcu(clpar p[], int len);
int process_parmKvs(clpar p[], int len);
int process_parmStatus(clpar p[], int len);
int process_parmProtoBuf(clpar p[], int len);
}


#define NODEFAULT() if (val==0) return cli_replyFailure()

void cli_out_timer_reply_entry(const char *key, const char *val, int len);
void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);
void cli_out_set_x(const char *obj_tag);
void cli_out_close(void);
void cli_out_x_reply_entry(const char *key, const char *val, int len);

void cli_msg_ready(void);
bool config_receiver(const char *val);
bool config_transmitter(const char *val);

int asc2bool(const char *s);
bool  asc2u8(const char *s, uint8_t *n, uint8_t limit);

#endif

