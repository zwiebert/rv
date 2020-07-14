/*
 * report.h
 *
 *  Created on: 16.06.2019
 *      Author: bertw
 */

#ifndef MAIN_REPORT_H_
#define MAIN_REPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

void report_event(const char *msg);
void report_valve_status(uint16_t valve_bits, uint16_t valve_mask);
void report_pump_status(bool state);
void report_pc_status(bool state);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_REPORT_H_ */
