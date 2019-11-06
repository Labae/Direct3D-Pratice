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

	// �� ���� ����.
	m_iModelCount = numModels;

	// �� ���� ����Ʈ�� ����.
	m_pModelInfoList = new ModelInfoType[m_iModelCount];
	if (!m_pModelInfoList)
	{
		return false;
	}

	// ���� ���ʷ����Ϳ� ���� �ð��� �õ�� ���.
	srand((UINT)time(NULL));

	for (int i = 0; i < m_iModelCount; i++)
	{
		// ������ ���� ����.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		// ���� ���.
		m_pModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// ������ ��ġ�� ����.
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
