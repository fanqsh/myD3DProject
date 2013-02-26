#pragma once

#include <windows.h>

class TimerClass 
{
public:
	TimerClass(void);
	TimerClass(const TimerClass&);
	~TimerClass(void);

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};