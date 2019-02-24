#pragma once
#include<Windows.h>

class HPTimer
{
private:
	long long startTime;
	long long lastCallToUpdate;
	long long currentCallToUpdate;
	long long frequency;
public:
	HPTimer()
	{
		LARGE_INTEGER t;
		QueryPerformanceFrequency(&t);
		frequency = t.QuadPart;

		Reset();
	}
	~HPTimer() {}
	HPTimer(const HPTimer&) = delete;
	HPTimer& operator=(const HPTimer&) = delete;

	void Reset()
	{
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		startTime = t.QuadPart;
		currentCallToUpdate = t.QuadPart;
		lastCallToUpdate = t.QuadPart;
	}

	void Update()
	{
		lastCallToUpdate = currentCallToUpdate;
		LARGE_INTEGER t;
		QueryPerformanceCounter(&t);
		currentCallToUpdate = t.QuadPart;
	}

	double GetTimeTotal()
	{
		double d = (double)(currentCallToUpdate - startTime);
		return (double)(d / frequency);
	}

	double GetTimeDelta()
	{
		double d = (double)(currentCallToUpdate - lastCallToUpdate);
		return (double)(d / frequency);
	}
};