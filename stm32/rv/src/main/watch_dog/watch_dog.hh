/**
 * \file    watch_dog/watch_dog.hh
 * \brief   A watch-dog exchanging "alive" messages over USART with our companion MCU (NetMCU).
 *
 *          A call message is sent periodically by this watchdog to the companion MCU.
 *          The companion MCU has to respond to each call. If a certain
 *          count of missed responses are reached, the companion MCU is reset (by reset-pin).
 *
 *          On the other side, the companion MCU will reset this MCU if there are no calls coming from this watchdog.
 *
 * \author  bertw
 */

#ifndef MAIN_WATCH_DOG_HH_
#define MAIN_WATCH_DOG_HH_

const unsigned WDG_INTERVAL = 10; ///< Send \ref WDG_CALL every this secs to companion MCU
const char WDG_CALL[] = "alive?\n"; ///< Send this string to companion MCU every \ref WDG_INTERVAL secs
const unsigned WDG_CALL_LEN = sizeof WDG_CALL;
const char WDG_RESPONSE[] = "ALIVE!";  ///< Expect this string as response to \ref WDG_CALL
const unsigned WDG_RESPONSE_LEN = sizeof WDG_RESPONSE;
const unsigned WDG_RESPONSE_DELAY = 1;  ///< Expect \ref WDG_RESPONSE to arrive before this delay in secs
const unsigned  WDG_MAX_MISSED_RESPONSES = 30; ///<  Allow this number of missed responses before resetting companion MCU (needs to leave time for firmware updates)


/**
 * \brief Do work. Should be called periodically.
 *
 *   - It sends a message (\ref WDG_CALL) to companion MCU periodically
 *   - It checks for response (\ref WDG_RESPONSE)from companion MCU
 *   - It resets companion MCU if it no longer responses
 */
void watchDog_loop();

///  \brief Setup watch-dog at startup.
void watchDog_setup();

/**
 *  \brief           Check a command-line for alive message from companion MCU
 *
 *  \param cmdLine   a received command line string
 *  \return   false if the command-line was not an alive message
 */
extern "C" bool watchDog_checkCommandLine(const char *cmdLine);


#endif /* MAIN_WATCH_DOG_HH_ */
