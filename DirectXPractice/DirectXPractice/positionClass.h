#pragma once
class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetFrameTime(float);
	void GetRotationY(float&);
	void GetRotationX(float&);

	void TurnLeft(bool);
	void TurnRight(bool);
	void TurnUp(bool);
	void TurnDown(bool);

private:
	float m_fFrameTime;
	float m_fRotationY;
	float m_fLeftTurnSpeed;
	float m_fRightTurnSpeed;
	float m_fRotationX;
	float m_fUpTurnSpeed;
	float m_fDownTurnSpeed;
};

