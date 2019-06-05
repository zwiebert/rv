#ifndef RV_TIMER_HH
#define RV_TIMER_HH

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

class RvTimer;
class RvTimers;

class RvTimer {
	friend class RvTimers;
public:
	RvTimer *pred = 0, *succ = 0;
private:
	int mTimeOn = 0, mTimeOff = 0, mRepeats = 0, mPeriod = 0;
	time_t mNextRun = 0, mNextOnOff = 0;
	bool mIsRunning = false, mIsOn = false;
	int mDoneOn = 0;
	int mValveNumber = -1;
	switch_valve_cb mSwitchValveCb = 0;
public:
	int getValveNumber() {
		return mValveNumber;
	}
	bool match(int valve_number) {
		return valve_number == mValveNumber;
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

	void set(int on_duration, int off_duration, int repeats, int period) {
		mTimeOn = on_duration;
		mTimeOff = off_duration;
		mRepeats = repeats;
		mPeriod = period;
	}
	void set(int on_duration) {
		set(on_duration, 0, 0, 0);
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
private:
	int lph = 0;
	RvTimer mTimers[RV_TIMER_COUNT];
	RvTimer mFreeTimers, mUsedTimers;
	switch_valve_cb mSvCb; // XXX

public:
	void timer_to_list(RvTimer *list, RvTimer *timer);

	RvTimers(switch_valve_cb cb) :
			mSvCb(cb) {
		mFreeTimers.pred = &mFreeTimers; // pointer to tail
		mUsedTimers.pred = &mUsedTimers; // pointer to tail

		for (int i = 0; i < RV_TIMER_COUNT; ++i) {
			timer_to_list(&mFreeTimers, &mTimers[i]);
		}
	}

	void register_callback(switch_valve_cb cb) {
		mSvCb = cb; // XXX
	}

	RvTimer *set(int valve_number, int on_duration, int off_duration,
			int repeats, int period) {
		RvTimer *timer = 0;
		for (RvTimer *t = mUsedTimers.succ; t; t = t->succ) { // XXX: for now only allow one timer per valve
			if (t->getValveNumber() == valve_number) {
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
			timer->register_callback(mSvCb, valve_number);
			timer->set(on_duration, off_duration, repeats, period);
			return timer;
		}
		return 0;
	}

	RvTimer *set(int valve_number, int on_duration) {
		return set(valve_number, on_duration, 0, 0, 0);
	}

	void unset(int valve_number) {
		for (RvTimer *t = mUsedTimers.succ; t; t = t->succ) {
			if (t->match(valve_number)) {
				timer_to_list(&mFreeTimers, t);
			}
		}
	}

	void loop();

	RvTimer *getRvTimer(int valve_number) {
		return &mTimers[valve_number];
	}
public:
	RvTimer &getTimerList() {
		return mUsedTimers;
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
