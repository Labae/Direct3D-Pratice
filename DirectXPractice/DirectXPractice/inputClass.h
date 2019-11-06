#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	bool IsLeftArrowPressed();
	bool IsRightArrowPressed();
	bool IsUpArrowPressed();
	bool IsDownArrowPressed();
	
private:
	bool ReadKeyBoard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_pDirectInput;
	IDirectInputDevice8* m_pKeyboard;
	IDirectInputDevice8* m_pMouse;
	UCHAR m_keyboardState[256];
	DIMOUSESTATE m_MouseState;
	int m_iScreenWidth, m_iScreenHeight;
	int m_iMouseX, m_iMouseY;
};

