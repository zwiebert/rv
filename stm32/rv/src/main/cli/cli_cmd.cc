#include <real_time_clock.h>
#include "user_config.h"

#include <string.h>



#include "user_config.h"
#include "cli_imp.h"
#include <stdio.h>

#include "peri/uart.h"
#include "water_pump.h"
#include "rv_timer.hh"
#include "app_cxx.hh"

#define warning_unknown_option(x)
extern "C" void timer_set(int8_t channel);

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)
#define CMD_ASK_DURATIONS " dur=?"
#define CMD_ASK_DURATIONS_LEN (sizeof CMD_ASK_DURATIONS - 1)

#define KEY_REMAINING_PREFIX "rem"
#define KEY_REMAINING_TIME_PREFIX_LEN ((sizeof KEY_REMAINING_TIME_PREFIX) - 1)
#define CMD_ASK_REMAINING_TIMES " rem=?"
#define CMD_ASK_REMAINING_TIMES_LEN (sizeof CMD_ASK_REMAINING_TIMES - 1)

#define KEY_STATUS_PREFIX "status"
#define KEY_STATUS_PREFIX_LEN ((sizeof KEY_STATUS_PREFIX) - 1)
#define CMD_ASK_STATUS " status=?;"
#define CMD_ASK_STATUS_LEN (sizeof CMD_ASK_STATUS - 1)

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

	bool wantsDurations = false, wantsRemainingTimes = false, wantsReply = false, hasDuration = false,
			wantsRelayPump = false, wantsRelayPC,
			wantsTime = false;

	for (arg_idx = 1; arg_idx < len; ++arg_idx) {
		const char *key = p[arg_idx].key, *val = p[arg_idx].val;

		if (key == NULL) {
			return -1;
		} else if (strcmp(key, KEY_DURATION_PREFIX) == 0 && *val == '?') {
			wantsReply = wantsDurations = true;

		} else if (strcmp(key, KEY_REMAINING_PREFIX) == 0 && *val == '?') {
			wantsReply = wantsRemainingTimes = true;

		} else if (strcmp(key, KEY_STATUS_PREFIX) == 0 && *val == '?') {
			wantsReply = wantsDurations = wantsRemainingTimes = wantsRelayPC = wantsRelayPump = wantsTime = true;

		} else if (strncmp(key, KEY_DURATION_PREFIX, KEY_DURATION_PREFIX_LEN) == 0) {
			int channel=-1, timer_number=0;
			sscanf((key + KEY_DURATION_PREFIX_LEN), "%d.%d", &channel, &timer_number);
			if (strchr(val, ',')) {
				int on=0, off=0, repeats=0, period=0;
				sscanf(val,"%d,%d,%d,%d", &on, &off, &repeats, &period);
				rvt.set(channel, on, off, repeats, period, timer_number)->run();
				hasDuration = true;
			} else {
				int duration = atoi(val);
				rvt.set(channel, duration, timer_number)->run();
				hasDuration = true;
			}

		} else {
			warning_unknown_option(key);
		}
	}

	if (hasDuration) {
		rvt.loop(); //XXX
		esp32_write("dr\n", 3);
	}

	if (wantsReply) {
		esp32_write(JSON_PREFIX, JSON_PREFIX_LEN);
#ifndef TODO
		if (wantsDurations) {
			for (RvTimer *t = rvt.getTimerList()->getNext(); t; t = t->getNext()) {
				int secs = t->get_duration();
				if (secs) {
					sprintf(buf + strlen(buf), "\"%s%d.%d\":%d,", KEY_DURATION_PREFIX, t->getValveNumber(), t->getTimerNumber(), secs);
				}
			}
		}

		if (wantsRemainingTimes) {
			for (RvTimer *t = rvt.getTimerList()->getNext(); t; t = t->getNext()) {
				int secs = t->get_remaining();
				if (secs) {
					sprintf(buf + strlen(buf), "\"%s%d.%d\":%d,", KEY_REMAINING_PREFIX, t->getValveNumber(), t->getTimerNumber(), secs);
				}
			}
		}
#endif
		if (wantsRelayPC && wp_isPressControlOn()) {
			strcat(buf, "\"pc\":1,");
		}

		if (wantsRelayPump && wp_isPumpOn()) {
			strcat(buf, "\"pump\":1,");
		}

		if (wantsTime) {
			  time_t timer = time(NULL);
			  struct tm t;
			  localtime_r(&timer, &t);
			  strftime(buf+strlen(buf), BUF_SIZE - strlen(buf), "\"time\":\"%FT%H:%M:%S\",", &t);
		}

		if (*buf)
			esp32_write(buf, strlen(buf) - 1); // no terminating comma

		esp32_write(JSON_SUFFIX, JSON_SUFFIX_LEN);
		*buf = '\0';
	}
	return 0;
}
