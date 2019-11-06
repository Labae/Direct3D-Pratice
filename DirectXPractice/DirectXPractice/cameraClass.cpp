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

	// 위쪽을 가르키는 벡터를 설정.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	// 카메라 위치 설정.
	position = m_Position;
	positionVector = XMLoadFloat3(&position);

	// 카메라가 보고 있는 위치 설정.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	// 회전값을 라디안 단위로 설정.
	pitch = m_Rotation.x * 0.0174532925f;
	yaw = m_Rotation.y * 0.0174532925f;
	roll = m_Rotation.z * 0.0174532925f;

	// 회전 행렬 생성.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 회전 행렬로 변환하여 뷰가 원점에서 올바르게 회전하도록 설정.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 회전 된 카메라의 위치를 뷰어 위치로 설정.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 뷰 행렬 생성.
	m_ViewMatirx = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_ViewMatirx;
}
