#include "stdafx.h"
#include "positionClass.h"

PositionClass::PositionClass()
{
	m_fFrameTime = 0.0f;
	m_fRotationY = 0.0f;
	m_fLeftTurnSpeed = 0.0f;
	m_fRightTurnSpeed = 0.0f;
	m_fRotationX = 0.0f;
	m_fUpTurnSpeed = 0.0f;
	m_fDownTurnSpeed = 0.0f;
}

PositionClass::PositionClass(const PositionClass&)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetFrameTime(float time)
{
	m_fFrameTime = time;
}

void PositionClass::GetRotationY(float& y)
{
	y = m_fRotationY;
}

void PositionClass::GetRotationX(float& x)
{
	x = m_fRotationX;
}

void PositionClass::TurnLeft(bool keyDown)
{
	if (keyDown)
	{
		m_fLeftTurnSpeed += m_fFrameTime * GET_FRAME_SPEED(1.0f);

		if (m_fLeftTurnSpeed > (m_fFrameTime * GET_FRAME_SPEED(15.0f)))
		{
			m_fLeftTurnSpeed = m_fFrameTime * GET_FRAME_SPEED(15.0f);
		}
	}
	else
	{
		m_fLeftTurnSpeed -= m_fFrameTime * GET_FRAME_SPEED(0.5f);
		if (m_fLeftTurnSpeed < 0.0f)
		{
			m_fLeftTurnSpeed = 0.0f;
		}
	}

	m_fRotationY -= m_fLeftTurnSpeed;
	if (m_fRotationY < 0.0f)
	{
		m_fRotationY += 360.0f;
	}
}

void PositionClass::TurnRight(bool keyDown)
{
	if (keyDown)
	{
		m_fRightTurnSpeed += m_fFrameTime * GET_FRAME_SPEED(1.0f);

		if (m_fRightTurnSpeed > (m_fFrameTime * GET_FRAME_SPEED(15.0f)))
		{
			m_fRightTurnSpeed = m_fFrameTime * GET_FRAME_SPEED(15.0f);
		}
	}
	else
	{
		m_fRightTurnSpeed -= m_fFrameTime * GET_FRAME_SPEED(0.5f);
		if (m_fRightTurnSpeed < 0.0f)
		{
			m_fRightTurnSpeed = 0.0f;
		}
	}

	m_fRotationY += m_fRightTurnSpeed;
	if (m_fRotationY > 360.0f)
	{
		m_fRotationY -= 360.0f;
	}
}

void PositionClass::TurnUp(bool keyDown)
{
	if (keyDown)
	{
		m_fUpTurnSpeed += m_fFrameTime * GET_FRAME_SPEED(1.0f);

		if (m_fUpTurnSpeed > (m_fFrameTime * GET_FRAME_SPEED(15.0f)))
		{
			m_fUpTurnSpeed = m_fFrameTime * GET_FRAME_SPEED(15.0f);
		}
	}
	else
	{
		m_fUpTurnSpeed -= m_fFrameTime * GET_FRAME_SPEED(0.5f);
		if (m_fUpTurnSpeed < 0.0f)
		{
			m_fUpTurnSpeed = 0.0f;
		}
	}

	m_fRotationX -= m_fUpTurnSpeed;
	if (m_fRotationX < 0.0f)
	{
		m_fRotationX += 360.0f;
	}
}

void PositionClass::TurnDown(bool keyDown)
{
	if (keyDown)
	{
		m_fDownTurnSpeed += m_fFrameTime * GET_FRAME_SPEED(1.0f);

		if (m_fDownTurnSpeed > (m_fFrameTime * GET_FRAME_SPEED(15.0f)))
		{
			m_fDownTurnSpeed = m_fFrameTime * GET_FRAME_SPEED(15.0f);
		}
	}
	else
	{
		m_fDownTurnSpeed -= m_fFrameTime * GET_FRAME_SPEED(0.5f);
		if (m_fDownTurnSpeed < 0.0f)
		{
			m_fDownTurnSpeed = 0.0f;
		}
	}

	m_fRotationX += m_fDownTurnSpeed;
	if (m_fRotationX > 360.0f)
	{
		m_fRotationX -= 360.0f;
	}
}
