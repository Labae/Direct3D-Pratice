#pragma once

#pragma comment(lib, "winmm.lib")

#include <mmsystem.h>

class FpsClass
{
public:
	FpsClass();
	FpsClass(const FpsClass&);
	~FpsClass();

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_iFps, m_iCount;
	ULONG m_lStartTime;
};

