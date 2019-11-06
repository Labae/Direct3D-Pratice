#include "stdafx.h"
#include "timeClass.h"

TimeClass::TimeClass()
{
}

TimeClass::TimeClass(const TimeClass&)
{
}

TimeClass::~TimeClass()
{
}

bool TimeClass::Initialize()
{
	m_iFrequency = 0;
	m_fTickPerMs = 0;
	m_iStartTime = 0;
	m_fFrameTime = 0;

	QueryPerformanceFrequency((LARGE_INTEGER*)& m_iFrequency);
	if (m_iFrequency == 0)
	{
		return false;
	}

	m_fTickPerMs = (float)(m_iFrequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)& m_iStartTime);

	return true;
}

void TimeClass::Frame()
{
	INT64 iCurrentTime = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)& iCurrentTime);

	float fTimeDifference = (float)(iCurrentTime / m_fTickPerMs);

	m_fFrameTime = fTimeDifference / m_fTickPerMs;
	m_iStartTime = iCurrentTime;
}

float TimeClass::GetTime()
{
	return m_fFrameTime;
}
