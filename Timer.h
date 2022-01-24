#ifndef __ROSE_TIMER__
#define __ROSE_TIMER__

#include <chrono>

/*
class Timer {
private:
	std::chrono::system_clock clock;
	uint64_t timestamp;
protected:
	__inline uint64_t getCurrentTimeInMillis() const {
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch());
		return ms.count();
	}
	__inline uint64_t getLastTimestamp() const {
		return timestamp;
	}
public:
	Timer() {
		timestamp = getCurrentTimeInMillis();
	}
	virtual ~Timer() { }

	__inline void setTimestamp(uint64_t newTimestamp) {
		timestamp = newTimestamp;
	}
	virtual uint64_t getPassedTimeInMillis() const {
		return getCurrentTimeInMillis() - timestamp;
	}
	virtual uint64_t updateTimestamp() {
		uint64_t result = getPassedTimeInMillis();
		timestamp = getCurrentTimeInMillis();
		return result;
	}
	__inline void setTimestampToCurrentTimepoint() {
		timestamp = getCurrentTimeInChronoMillis();
	}
};
*/

class Timer {
private:
	std::chrono::steady_clock clock;
	std::chrono::time_point<std::chrono::steady_clock> timestamp;
protected:
	__inline std::chrono::time_point<std::chrono::steady_clock> getCurrentTimepointInChronoMillis() const {
		return clock.now();
	}
public:
	Timer() {
		setTimestampToCurrentTimepoint();
	}
	virtual ~Timer() { }

	__inline static uint64_t getCurrentTimepointInMillis() {
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
		return ms.count();
	}

	__inline void setTimestamp(uint64_t newTimestamp) {
		timestamp = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>(std::chrono::milliseconds(newTimestamp));
	}
	virtual uint64_t getPassedTimeInMillis() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(getCurrentTimepointInChronoMillis() - timestamp).count();
	}
	virtual uint64_t updateTimestamp() {
		uint64_t result = getPassedTimeInMillis();
		timestamp = getCurrentTimepointInChronoMillis();
		return result;
	}
	__inline void setTimestampToCurrentTimepoint() {
		timestamp = getCurrentTimepointInChronoMillis();
	}
};

class WrappingTimer : public Timer {
private:
	uint64_t timeInMillisTillWrap;
protected:
	__inline uint64_t getDurationForWrappingInMillis() const {
		return timeInMillisTillWrap;
	}
public:
	WrappingTimer() : WrappingTimer(-1) {

	}
	WrappingTimer(uint64_t wrapTimeInMillis) {
		timeInMillisTillWrap = wrapTimeInMillis;
	}
	virtual ~WrappingTimer() {

	}
	virtual uint64_t updateTimestamp() {
		uint64_t timePassed = getPassedTimeInMillis();
		if (timePassed >= timeInMillisTillWrap) {
			onTimeWrap();
			setTimestamp(getCurrentTimepointInMillis() - timePassed + timeInMillisTillWrap);
		}
		return timePassed;
	}
	virtual void onTimeWrap() {
	}
	__inline void setDurationForWrappingInMillis(const uint64_t wrapTime) {
		this->timeInMillisTillWrap = wrapTime;
	}
};
#endif 