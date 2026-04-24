#include "basic.hpp"
#include "assert.hpp"
#include "string.hpp"

const char* error_get_string(Error error_code) {
	LOCAL_PERSIST const char* ERROR_STRINGS[(int)Error::COUNT] = {
		STRINGIFY(ERROR_SUCCESS),
		STRINGIFY(ERROR_RESOURCE_NOT_FOUND),
		STRINGIFY(ERROR_RESOURCE_TOO_BIG),
		STRINGIFY(ERROR_NULL_PARAMETER),
		STRINGIFY(ERROR_INVALID_PARAMETER),
	};

	RUNTIME_ASSERT(((int)error_code >= 0) && ((int)error_code < (int)Error::COUNT));

	return ERROR_STRINGS[(int)error_code];
}

Timer Timer::create() {
	Timer timer = {};
	timer.should_tick = false;
	timer.elapsed = 0.0;
	timer.duration = 0.0;

	return timer;
}

void Timer::start(float duration) {
	this->should_tick = true;
	this->elapsed = 0.0f;
	this->duration = duration;
}

bool Timer::tick(float dt) {
	if (!this->should_tick) return false;

	this->elapsed = this->elapsed + dt;
	return this->elapsed <= this->duration;
}

void Timer::stop() {
	this->should_tick = false;
	this->elapsed = 0.0f;
	this->duration = 0.0f;
}