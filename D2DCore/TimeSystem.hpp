//========================================================================
// TimeSystem.hpp
//========================================================================

#pragma once

#include <Windows.h>

#include "Singleton.hpp"

class TimeSystem : public Singleton<TimeSystem> {
	friend class Singleton<TimeSystem>;
private:
	LARGE_INTEGER freq, start, previous, current;
	float delta = 0.0f;

public:
	void StartUp() {
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);
		previous = start;
		delta = 0.0f;
	}

	void Update() {
		QueryPerformanceCounter(&current);
		delta = static_cast<float>(current.QuadPart - previous.QuadPart) / static_cast<float>(freq.QuadPart);
		previous = current;
	}

	float GetDelta() const { return delta; }
	float Elapsed() const {
		return static_cast<float>(current.QuadPart - start.QuadPart)
			/ static_cast<float>(freq.QuadPart);
	}
};