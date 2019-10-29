#pragma once

const ULONG MAX_SAMPLE_COUNT = 50;

class CGameTimer
{
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	ULONG m_nCurrentFrameRate;
	ULONG m_nFramesPerSecond;
	float m_fFPSTimeElapsed;

public:
	void Reset();
	void Tick(float fLockFPS = 0.0f);
	ULONG GetFrameRate(LPCSTR lpszString = nullptr, int nCharcaters = 0);
	float GetTimeElapsed();
	CGameTimer();
	virtual ~CGameTimer();
};
