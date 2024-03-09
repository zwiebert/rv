/**
 * \file    watch_dog/watch_dog.hh
 * \brief   A watch-dog exchanging "alive" messages over USART with our partner MCU (NetMCU).
 * \author  bertw
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


/// \brief Do work.
void watchDog_loop();

///  \brief Setup watch-dog at startup.
void watchDog_setup();

///  \brief           Check if a command line is addressed to watch-dog
///  \param cmdLine   command line as null terminated string
///  \bool            - true if command line was consumed by watch-dog
///                   - false if command line was not addressed to watch-dog
extern "C" bool watchDog_checkCommandLine(const char *cmdLine);


#endif /* MAIN_WATCH_DOG_HH_ */
