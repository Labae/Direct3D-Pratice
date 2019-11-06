#include "stdafx.h"
#include "inputClass.h"

InputClass::InputClass()
{
	m_pDirectInput = nullptr;
	m_pKeyboard = nullptr;
	m_pMouse = nullptr;
}

InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_iScreenWidth = screenWidth;
	m_iScreenHeight = screenHeight;

	m_iMouseX = m_iMouseY = 0;

	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)& m_pDirectInput, NULL)))
	{
		return false;
	}

	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL)))
	{
		return false;
	}

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	if (FAILED(m_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
	{
		return false;
	}

	if (FAILED(m_pKeyboard->Acquire()))
	{
		return false;
	}

	if (FAILED(m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
	{
		return false;
	}

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
	{
		return false;
	}

	if (FAILED(m_pMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return false;
	}

	if (FAILED(m_pMouse->Acquire()))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
		m_pMouse = nullptr;
	}

	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
		m_pKeyboard = nullptr;
	}

	if (m_pDirectInput)
	{
		m_pDirectInput->Release();
		m_pDirectInput = nullptr;
	}
}

bool InputClass::Frame()
{
	if (!ReadKeyBoard())
	{
		return false;
	}

	if (!ReadMouse())
	{
		return false;
	}

	ProcessInput();

	return true;
}

bool InputClass::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_iMouseX;
	mouseY = m_iMouseY;
}

bool InputClass::IsLeftArrowPressed()
{
	if (m_keyboardState[DIK_LEFTARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsRightArrowPressed()
{
	if (m_keyboardState[DIK_RIGHTARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsUpArrowPressed()
{
	if (m_keyboardState[DIK_UPARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsDownArrowPressed()
{
	if (m_keyboardState[DIK_DOWNARROW] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::ReadKeyBoard()
{
	HRESULT result = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)& m_keyboardState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result = m_pMouse->GetDeviceState(sizeof(m_MouseState), (LPVOID)& m_MouseState);

	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	m_iMouseX += m_MouseState.lX;
	m_iMouseY += m_MouseState.lY;

	if (m_iMouseX < 0) 
	{ 
		m_iMouseX = 0;
	}

	if (m_iMouseY < 0)
	{
		m_iMouseY = 0;
	}

	if (m_iMouseX > m_iScreenWidth)
	{
		m_iMouseX = m_iScreenWidth;
	}

	if (m_iMouseY > m_iScreenHeight)
	{
		m_iMouseY = m_iScreenHeight;
	}
}
