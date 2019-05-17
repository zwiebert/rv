#include "user_config.h"

#include <string.h>



#include "user_config.h"
#include "cli_imp.h"
#include "valve_timer.h"
#include <stdio.h>

#include "peri/uart.h"

#define warning_unknown_option(x)
void timer_set(int8_t channel);

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)
#define CMD_ASK_DURATIONS "cmd dur=?;"
#define CMD_ASK_DURATIONS_LEN (sizeof CMD_ASK_DURATIONS - 1)

#define KEY_REMAINING_PREFIX "rem"
#define KEY_REMAINING_TIME_PREFIX_LEN ((sizeof KEY_REMAINING_TIME_PREFIX) - 1)
#define CMD_ASK_REMAINING_TIMES "cmd rem=?;"
#define CMD_ASK_REMAINING_TIMES_LEN (sizeof CMD_ASK_REMAINING_TIMES - 1)

#define JSON_PREFIX "{\"from\":\"rv\", \"data\":{"
#define JSON_PREFIX_LEN ((sizeof JSON_PREFIX) - 1)
#define JSON_SUFFIX "}};\n"
#define JSON_SUFFIX_LEN ((sizeof JSON_SUFFIX) - 1)

#define BUF_SIZE 256


const char help_parmCmd[]  =
    "zone=[0-13]      zone number\n"
    "duration=[0-60]  how long to irrigate\n"
;

int ICACHE_FLASH_ATTR
process_parmCmd(clpar p[], int len) {
	int arg_idx;
	char buf[BUF_SIZE] = "";

	bool wantsDurations = false, wantsRemainingTimes = false, wantsReply = false, hasDuration = false;

	for (arg_idx = 1; arg_idx < len; ++arg_idx) {
		const char *key = p[arg_idx].key, *val = p[arg_idx].val;

		if (key == NULL) {
			return -1;
		} else if (strcmp(key, KEY_DURATION_PREFIX) == 0 && *val == '?') {
			wantsReply = wantsDurations = true;

		} else if (strcmp(key, KEY_REMAINING_PREFIX) == 0 && *val == '?') {
			wantsReply = wantsRemainingTimes = true;

		} else if (strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
			int channel = atoi(key + KEY_DURATION_PREFIX_LEN);
			int duration = atoi(val);
			valveTimer_setTimerDurationByMinutes(channel, duration);
			hasDuration = true;

		} else {
			warning_unknown_option(key);
		}
	}

	if (hasDuration) {
		timer_set(TIMER_SET_DONE);
		valveTimer_loop(); //XXX: to det immidiate result for dur=?
	}

	if (wantsReply) {
		esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);

		if (wantsDurations) {
			for (int i = 0; i < VALVE_TIMER_COUNT; ++i) {
				uint8_t mints = valveTimer_getProgrammedMinutes(i);
				if (mints) {
					sprintf(buf + strlen(buf), "\"%d\":%d,", KEY_DURATION_PREFIX, i, mints);
				}
			}
			if (*buf)
				esp32_write(buf, strlen(buf) - 1);
		}

		if (wantsRemainingTimes) {
			for (int i = 0; i < VALVE_TIMER_COUNT; ++i) {
				uint8_t mints = valveTimer_getRemainingMinutes(i);
				if (mints) {
					sprintf(buf + strlen(buf), "\"%s%d\":%d,", KEY_REMAINING_PREFIX, i, mints);
				}
			}
			if (*buf)
				esp32_write(buf, strlen(buf) - 1);
		}

		esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
		*buf = '\0';
	}
	return 0;
}
