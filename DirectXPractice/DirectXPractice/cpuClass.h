#pragma once

#pragma comment(lib, "pdh.lib")

#include <Pdh.h>

class CpuClass
{
public:
	CpuClass();
	CpuClass(const CpuClass&);
	~CpuClass();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_bCanReadCpu;
	HQUERY m_QueryHandle;
	HCOUNTER m_CounterHandle;
	ULONG m_LastSampleTime;
	long m_CpuUsage;
};

