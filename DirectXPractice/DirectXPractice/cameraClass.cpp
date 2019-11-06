#include "stdafx.h"
#include "cameraClass.h"

CameraClass::CameraClass()
{
	ZeroMemory(&m_Position, sizeof(m_Position));
	ZeroMemory(&m_Rotation, sizeof(m_Rotation));
	ZeroMemory(&m_ViewMatirx, sizeof(m_ViewMatirx));
}

CameraClass::CameraClass(const CameraClass&)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return m_Position;
}

XMFLOAT3 CameraClass::GetRotation()
{
	return m_Rotation;
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// ������ ����Ű�� ���͸� ����.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// ī�޶� ��ġ ����.
	position = m_Position;
	positionVector = XMLoadFloat3(&position);

	// ī�޶� ���� �ִ� ��ġ ����.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	// ȸ������ ���� ������ ����.
	pitch = m_Rotation.x * 0.0174532925f;
	yaw = m_Rotation.y * 0.0174532925f;
	roll = m_Rotation.z * 0.0174532925f;

	// ȸ�� ��� ����.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// ȸ�� ��ķ� ��ȯ�Ͽ� �䰡 �������� �ùٸ��� ȸ���ϵ��� ����.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// ȸ�� �� ī�޶��� ��ġ�� ��� ��ġ�� ����.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// �� ��� ����.
	m_ViewMatirx = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_ViewMatirx;
}
