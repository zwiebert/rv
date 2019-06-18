/*
 * systick_1ms.h
 *
 *  Created on: 19.06.2019
 *      Author: bertw
 */

#ifndef MAIN_SYSTICK_1MS_H_
#define MAIN_SYSTICK_1MS_H_



#ifdef __cplusplus
extern "C" {
#endif

uint64_t runTimeMs(void);
void systick_setup(void);

#ifdef __cplusplus
}
#endif



#endif /* MAIN_SYSTICK_1MS_H_ */
