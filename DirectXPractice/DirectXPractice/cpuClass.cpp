#include "stdafx.h"
#include "cpuClass.h"

CpuClass::CpuClass()
{
}

CpuClass::CpuClass(const CpuClass&)
{
}

CpuClass::~CpuClass()
{
}

void CpuClass::Initialize()
{
	m_bCanReadCpu = true;
	m_LastSampleTime = 0;
	m_CpuUsage = 0;

	PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_QueryHandle);
	if (status != ERROR_SUCCESS)
	{
		m_bCanReadCpu = false;
	}

	status = PdhAddCounter(m_QueryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_CounterHandle);
	if (status != ERROR_SUCCESS)
	{
		m_bCanReadCpu = false;
	}

	m_LastSampleTime = GetTickCount();

	m_CpuUsage = 0;
}

void CpuClass::Shutdown()
{
	if (m_bCanReadCpu)
	{
		PdhCloseQuery(m_QueryHandle);
	}
}

void CpuClass::Frame()
{
	PDH_FMT_COUNTERVALUE value;

	if (m_bCanReadCpu)
	{
		if ((m_LastSampleTime + 1000) < GetTickCount())
		{
			m_LastSampleTime = GetTickCount();

			PdhCollectQueryData(m_QueryHandle);

			PdhGetFormattedCounterValue(m_CounterHandle, PDH_FMT_LONG, NULL, &value);

			m_CpuUsage = value.longValue;
		}
	}
}

int CpuClass::GetCpuPercentage()
{
	int usage = 0;

	if (m_bCanReadCpu)
	{
		usage = (int)m_CpuUsage;
	}

	return usage;
}
