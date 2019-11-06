#pragma once
class TimeClass
{
public:
	TimeClass();
	TimeClass(const TimeClass&);
	~TimeClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_iFrequency;
	float m_fTickPerMs;
	INT64 m_iStartTime;
	float m_fFrameTime;
};

