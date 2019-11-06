#include "stdafx.h"
#include "modelListClass.h"

ModelListClass::ModelListClass()
{
	m_iModelCount = 0;
	m_pModelInfoList = nullptr;
}

ModelListClass::ModelListClass(const ModelListClass&)
{
}

ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;

	// 모델 수를 저장.
	m_iModelCount = numModels;

	// 모델 정보 리스트를 생성.
	m_pModelInfoList = new ModelInfoType[m_iModelCount];
	if (!m_pModelInfoList)
	{
		return false;
	}

	// 랜덤 제너레이터에 현재 시간을 시드로 등록.
	srand((UINT)time(NULL));

	for (int i = 0; i < m_iModelCount; i++)
	{
		// 임의의 색성 지정.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		// 색상 등록.
		m_pModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// 임의의 위치에 생성.
		m_pModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_pModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_pModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelListClass::Shutdown()
{
	if (m_pModelInfoList)
	{
		delete[] m_pModelInfoList;
		m_pModelInfoList = nullptr;
	}
}

int ModelListClass::GetModelCount()
{
	return m_iModelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color)
{
	positionX = m_pModelInfoList[index].positionX;
	positionY = m_pModelInfoList[index].positionY;
	positionZ = m_pModelInfoList[index].positionZ;

	color = m_pModelInfoList[index].color;
}
