#include "stdafx.h"
#include "inputClass.h"
#include "graphicsClass.h"
//#include "soundClass.h"
#include "fpsClass.h"
#include "cpuClass.h"
#include "timeClass.h"
#include "positionClass.h"
#include "systemClass.h"

SystemClass::SystemClass()
{
	m_applicationName = NULL;
	m_hInstance = NULL;
	m_hwnd = NULL;
	m_pInput = nullptr;
	m_pGraphics = nullptr;
	//m_pSound = nullptr;
	m_pFps = nullptr;
	m_pCpu = nullptr;
	m_pTime = nullptr;
	m_pPosition = nullptr;
}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	bool result = false;

	// 윈도우 창 가로, 세로 길이 변수 초기화.
	int screenWidth = 0;
	int screenHeight = 0;

	// 윈도우 생성 초기화.
	InitializeWindows(screenWidth, screenHeight);

	// input 객체 생성.
	// 키보드 입력 처리에 사용.
	m_pInput = new InputClass;
	if (!m_pInput)
	{
		return false;
	}

	// input 객체 초기화.
	if (!m_pInput->Initialize(m_hInstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not Initialize Input", L"Error", MB_OK);
		return false;
	}

	// graphics 객체 생성.
	// 그래픽 렌더링 처리에 사용.
	m_pGraphics = new GraphicsClass;
	if (!m_pGraphics)
	{
		return false;
	}

	// graphics 객체 초기화.
	if (!m_pGraphics->Initialize(screenWidth, screenHeight, m_hwnd))
	{
		MessageBox(m_hwnd, L"Could not Initialize Graphics", L"Error", MB_OK);
		return false;
	}

	m_pFps = new FpsClass;
	if (!m_pFps)
	{
		return false;
	}

	m_pFps->Initialize();

	m_pCpu = new CpuClass;
	if (!m_pCpu)
	{
		return false;
	}

	m_pCpu->Initialize();

	m_pTime = new TimeClass;
	if (!m_pTime)
	{
		return false;
	}

	if (!m_pTime->Initialize())
	{
		MessageBox(m_hwnd, L"Could not Initialize Time", L"Error", MB_OK);
		return false;
	}

	m_pPosition = new PositionClass;
	if (!m_pPosition)
	{
		return false;
	}

	//m_pSound = new SoundClass;
	//if (!m_pSound)
	//{
	//	return false;
	//}

	//if (!m_pSound->Initialize(m_hwnd))
	//{
	//	return false;
	//}

	return true;
}

void SystemClass::Shutdown()
{
	//if (m_pSound)
	//{
	//	m_pSound->Shutdown();
	//	delete m_pSound;
	//	m_pSound = nullptr;
	//}

	if (m_pPosition)
	{
		delete m_pPosition;
		m_pPosition = nullptr;
	}

	if (m_pTime)
	{
		delete m_pTime;
		m_pTime = nullptr;
	}

	if (m_pCpu)
	{
		m_pCpu->Shutdown();
		delete m_pCpu;
		m_pCpu = nullptr;
	}

	if (m_pFps)
	{
		delete m_pFps;
		m_pFps = nullptr;
	}

	if (m_pGraphics)
	{
		m_pGraphics->Shutdown();
		delete m_pGraphics;
		m_pGraphics = nullptr;
	}

	if (m_pInput)
	{
		m_pInput->Shutdown();
		delete m_pInput;
		m_pInput = nullptr;
	}

	ShutdownWindows();
}

void SystemClass::Run()
{
	bool done = false;

	// 메세지 구조체 생성.
	MSG msg;
	// 메세지 구조체 초기화.
	ZeroMemory(&msg, sizeof(MSG));

	// 사용자로부터 종료 메세지를 받을때까지 메세지루프를 돕니다.
	while (!done)
	{
		// 윈도우 메세지 처리.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// 종료 메시지를 받을 경우 메시지루프를 탈출.
			if (msg.message == WM_QUIT)
			{
				done = true;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 프레임 함수 처리.
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame processing Failed", L"Error", MB_OK);
				done = true;
			}
		}

		if (m_pInput->IsEscapePressed() == true)
		{
			done = true;
		}
	}
}

LRESULT SystemClass::MessageHander(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

bool SystemClass::Frame()
{
	m_pTime->Frame();
	m_pFps->Frame();
	m_pCpu->Frame();

	if (!m_pInput->Frame())
	{
		return false;
	}

	m_pPosition->SetFrameTime(m_pTime->GetTime());

	bool keyDown = m_pInput->IsLeftArrowPressed();
	m_pPosition->TurnLeft(keyDown);

	keyDown = m_pInput->IsRightArrowPressed();
	m_pPosition->TurnRight(keyDown);

	keyDown = m_pInput->IsUpArrowPressed();
	m_pPosition->TurnUp(keyDown);

	keyDown = m_pInput->IsDownArrowPressed();
	m_pPosition->TurnDown(keyDown);

	float rotationY = 0.0f;
	m_pPosition->GetRotationY(rotationY);

	float rotationX = 0.0f;
	m_pPosition->GetRotationX(rotationX);

	if (!m_pGraphics->Frame(m_pFps->GetFps(), m_pCpu->GetCpuPercentage(), rotationX, rotationY))
	{
		return false;
	}

	if (!m_pGraphics->Render())
	{
		return false;
	}

	return  true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// 외부 포인터를 이 객체로 지정.
	ApplicationHandle = this;

	// 이 프로그램의 객체를 가져옴.
	m_hInstance = GetModuleHandle(NULL);

	// 프로그램의 이름을 지정.
	m_applicationName = L"Dx11Practice";

	// Window 클래스를 아래와 같이 설정.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// 윈도우 클래스 등록.
	RegisterClassEx(&wc);

	// 모니터 화면 해상도를 설정.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		// 풀스크린이라면 모니터 화면 해상도를 데스크톱 해상도로 설정.
		// 색상을 32Bit로 지정.
		DEVMODE dmScreenSettings; 
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린으로 디스플레이 설정.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// 윈도우 모드일 경우 800 * 600 으로 지정.
		screenWidth = 800;
		screenHeight = 600;

		// 윈도우 창을 가로, 세로 정가운데로 설정.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	// 윈도우를 화면에 표시하고 포커스를 지정.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
	// 풀스크린 모드라면 디스플레이 설정 초기화.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창을 제거.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 프로그램 객체 제거.
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// 외부참조포인터를 제거.
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHander(hwnd, umsg, wparam, lparam);
		}
	}
}
