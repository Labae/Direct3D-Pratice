#include "stdafx.h"
#include "lightClass.h"

LightClass::LightClass()
{
	ZeroMemory(&m_DiffuseColor, sizeof(m_DiffuseColor));
	ZeroMemory(&m_Direction, sizeof(m_Direction));
}

LightClass::LightClass(const LightClass&)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float r, float g, float b, float a)
{
	m_AmbientColor = XMFLOAT4(r, g, b, a);
}

void LightClass::SetDiffuseColor(float r, float g, float b, float a)
{
	m_DiffuseColor = XMFLOAT4(r, g, b, a);
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_Direction = XMFLOAT3(x, y, z);
}

void LightClass::SetSpecularPower(float power)
{
	m_SpecularPower = power;
}

void LightClass::SetSpecularColor(float r, float g, float b, float a)
{
	m_SpecularColor = XMFLOAT4(r, g, b, a);
}

XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_AmbientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_DiffuseColor;
}

XMFLOAT3 LightClass::GetDirection()
{
	return m_Direction;
}

float LightClass::GetSpecularPower()
{
	return m_SpecularPower;
}

XMFLOAT4 LightClass::GetSpecularColor()
{
	return m_SpecularColor;
}
