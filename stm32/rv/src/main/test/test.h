/**
 * \file test/test.h
 * \brief Test program for zone timers (Obsolete?)
 *
 * This program was called from main after setup and run a long time while programming timers and
 * checking the states while waiting for completion.
 *
 */

#ifndef MAIN_TEST_TEST_H_
#define MAIN_TEST_TEST_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef enum test_res { TR_RUNNING, TR_FAILED, TR_SUCCEEDED, TR_NOT_IMPLEMENTED } testRes_T;

testRes_T tr_failed();

testRes_T test_loop();
void test_setup();

testRes_T testRvTimer_loop();
void testRvTimer_setup();


#ifdef __cplusplus
}
#endif


#endif /* MAIN_TEST_TEST_H_ */
