#ifndef RV_TIMER_HH
#define RV_TIMER_HH

#include "user_config.h"
#include "misc/int_macros.h"

#include <time.h>
#ifdef MOD_TEST
#include <iostream>
volatile time_t start_time;
volatile int x;
void breaker() {
	// start_time = time(0);
}

#endif

const int RV_TIMER_COUNT = 20;
const int RV_VALVE_COUNT = 12;
const int RV_MAX_LPH = 1200;

extern int Lph[RV_VALVE_COUNT];

typedef void (*switch_valve_cb)(int valve_number, bool state);
typedef void (*switch_valves_cb)(uint16_t valve_bits, uint16_t valve_mask);



class RvTimer;
class RvTimers;

class RvTimer {
	friend class RvTimers;
public:
	RvTimer *pred = 0, *succ = 0;
	void unlinkThis() {
		if (pred)
			pred->succ = succ;
		if (succ)
			succ->pred = pred;
	}
	RvTimer *getNext() { return succ->mValveNumber == -2 ? 0 : succ; }
private:
	int mValveNumber = -1;
	int mTimerNumber=0; // multiple time per valve with different numbers

	int mTimeOn = 0, mTimeOff = 0, mRepeats = 0, mPeriod = 0;
	bool isRunOnce() { return mTimeOn && !mTimeOff && !mRepeats && !mPeriod; }

	int mTodSpanBegin = 0, mTodSpanEnd = 0;
	bool hasTodSpan() { return mTodSpanBegin || mTodSpanEnd; }

	time_t mNextRun = 0, mNextOnOff = 0;
	bool mIsRunning = false, mIsOn = false;
	int mDoneOn = 0;

	switch_valve_cb mSwitchValveCb = 0;
public:
	RvTimer(bool isListHead=false) {
		if (isListHead) {
			mValveNumber = -2;
			this->pred = this->succ = this;
		}
	}
	bool isListHead() { return mValveNumber == -2; }

	int getValveNumber() {
		return mValveNumber;
	}
	int getTimerNumber() {
		return mTimerNumber;
	}
	bool match(int valve_number, int id=0) {
		return valve_number == mValveNumber && id == mTimerNumber;
	}

	void register_callback(switch_valve_cb cb, int valve_number) {
		mSwitchValveCb = cb;
		mValveNumber = valve_number;
	}

	void switch_valve(bool state) {
		if (mSwitchValveCb) {
			mSwitchValveCb(mValveNumber, state);
		}
	}

	void run() {
		mNextOnOff = time(0);
		mIsRunning = true;
		mDoneOn = 0;
	}

	void stop() {
		mIsRunning = false;
		mNextOnOff = 0;
		if (mIsOn) {
			switch_valve(false);
			mIsOn = false;
		}

		if (mPeriod)
			mNextRun = time(0) + mPeriod;
	}
	void pause();

	int get_duration() { return mTimeOn; }
	int get_remaining() { return isOn() ? mNextOnOff - time(0) : mTimeOn; }

	void set(int on_duration, int off_duration, int repeats, int period, int id) {
		mTimeOn = on_duration;
		mTimeOff = off_duration;
		mRepeats = repeats;
		mPeriod = period;
		mTimerNumber = id;
	}
	void set(int on_duration, int id) {
		set(on_duration, 0, 0, 0, id);
	}

	bool isReadyToRun() {
		return !mIsRunning && mNextRun && mNextRun < time(0);
	}
	bool isReadyToOnOff() {
		return mIsRunning && mNextOnOff && mNextOnOff < time(0);
	}
	bool isDone() {
		return !mIsRunning && mDoneOn >= mRepeats && !mNextRun;
	}
	bool isRunning() {
		return mIsRunning;
	}
	bool isOn() {
		return mIsRunning && mIsOn;
	}
	bool isOff() {
		return mIsRunning && !mIsOn;
	}

	void changeOnOff() {
		if (mIsOn) {
			++mDoneOn;
			if (mDoneOn >= mRepeats) {
				mNextOnOff = 0;
				stop();
				return;
			}
		}

		mIsOn = !mIsOn;
		switch_valve(mIsOn);
		if (mIsOn && mTimeOn) {
			mNextOnOff = time(0) + mTimeOn;
		} else if (!mIsOn && mTimeOff) {
			mNextOnOff = time(0) + mTimeOff;
		}
	}

	bool checkState() {
		if (isReadyToRun())
			return true;
		if (isReadyToOnOff())
			return true;

		return false;
	}
};

class RvTimers {
	static uint16_t valve_bits, valve_mask;
	static void switch_valve_2(int valve_number, bool state) {
		SET_BIT(valve_mask, valve_number);
		if (state)
			SET_BIT(valve_bits, valve_number);
	}
private:
	int lph = 0; // XXX: don't count timers for the same valve more than once
	RvTimer mTimers[RV_TIMER_COUNT];
	RvTimer mFreeTimers, mUsedTimers;
	switch_valve_cb mSvCb; // XXX
	switch_valves_cb mSvsCb;

public:
	void timer_to_list(RvTimer *list, RvTimer *timer);

	RvTimers(switch_valve_cb cb, switch_valves_cb cb2) :
		mFreeTimers(true),  mUsedTimers(true), mSvCb(cb), mSvsCb(cb2){

		for (int i = 0; i < RV_TIMER_COUNT; ++i) {
			timer_to_list(&mFreeTimers, &mTimers[i]);
		}
	}

	void register_callback(switch_valve_cb cb) {
		mSvCb = cb; // XXX
	}

	RvTimer *set(int valve_number, int on_duration, int off_duration,
			int repeats, int period, int id) {
		RvTimer *timer = 0;
		for (RvTimer *t = mUsedTimers.getNext(); t; t = t->getNext()) {
			if (t->match(valve_number, id)) {
				if (t->isOn())
					lph -= Lph[valve_number];

				t->stop();

				timer = t;
				break;
			}
		}

		if (!timer) {
			timer = mFreeTimers.succ;
			if (timer)
				timer_to_list(&mUsedTimers, timer);
		}

		if (timer) {
			timer->register_callback((mSvCb ? mSvCb : switch_valve_2), valve_number);
			timer->set(on_duration, off_duration, repeats, period, id);
			return timer;
		}
		return 0;
	}

	RvTimer *set(int valve_number, int on_duration, int id=0) {
		return set(valve_number, on_duration, 0, 0, 0, id);
	}

	void unset(int valve_number, int id) {
		for (RvTimer *t = mUsedTimers.getNext(); t; t = t->getNext()) {
			if (t->match(valve_number, id)) {
				timer_to_list(&mFreeTimers, t);
			}
		}
	}

	void loop();

	RvTimer *getRvTimer(int valve_number) {
		return &mTimers[valve_number];
	}
public:
	RvTimer *getTimerList() {
		return &mUsedTimers;
	}
};

#ifdef MOD_TEST
#include <stdlib.h>
#include <iostream>

void switch_valve(int valve_number, bool state) {
	std::cout << (time(0) - start_time) << ": valve " << valve_number << " switched " << (state ? "on" : "off") << std::endl;
	std::cout.flush();
}

extern "C" void app_switch_valve(int valve_number, bool state);

int main() {
	start_time = time(0);
	RvTimers rvt = RvTimers(switch_valve);

	// rvt.set(0,2,4,3,60)->run();
	// rvt.set(1,3,5,4,130)->run();
	// rvt.set(2,3)->run();

	rvt.set(6, 30, 0, 0, 60*2)->run();
	rvt.set(7, 30, 0, 0, 60*2)->run();
	while (1) {
		rvt.loop();
		//sleep(100);
	}
}

#endif

#endif
// Local Variables:
// compile-command: "/c/MinGW/bin/g++ -ggdb rv_timer.cc"
// End:
