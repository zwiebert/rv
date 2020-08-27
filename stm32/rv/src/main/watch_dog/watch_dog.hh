/*
 * watch_dog.hh
 *
 *  Created on: 04.06.2019
 *      Author: bertw
 */

#ifndef MAIN_WATCH_DOG_HH_
#define MAIN_WATCH_DOG_HH_

const unsigned WDG_INTERVAL = 10;
const char WDG_CALL[] = "alive?\n";
const unsigned WDG_CALL_LEN = sizeof WDG_CALL;
const char WDG_RESPONSE[] = "ALIVE!";
const unsigned WDG_RESPONSE_LEN = sizeof WDG_RESPONSE;
const unsigned WDG_RESPONSE_DELAY = 1;
const unsigned  WDG_MAX_MISSED_RESPONSES = 30; //XXX wait long enough to allow ESP32 to flash its firmware


void watchDog_loop();
void watchDog_setup();

extern "C" bool watchDog_checkCommandLine(const char *cmdLine);


#endif /* MAIN_WATCH_DOG_HH_ */
