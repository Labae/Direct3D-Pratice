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

	// ������ â ����, ���� ���� ���� �ʱ�ȭ.
	int screenWidth = 0;
	int screenHeight = 0;

	// ������ ���� �ʱ�ȭ.
	InitializeWindows(screenWidth, screenHeight);

	// input ��ü ����.
	// Ű���� �Է� ó���� ���.
	m_pInput = new InputClass;
	if (!m_pInput)
	{
		return false;
	}

	// input ��ü �ʱ�ȭ.
	if (!m_pInput->Initialize(m_hInstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not Initialize Input", L"Error", MB_OK);
		return false;
	}

	// graphics ��ü ����.
	// �׷��� ������ ó���� ���.
	m_pGraphics = new GraphicsClass;
	if (!m_pGraphics)
	{
		return false;
	}

	// graphics ��ü �ʱ�ȭ.
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

	// �޼��� ����ü ����.
	MSG msg;
	// �޼��� ����ü �ʱ�ȭ.
	ZeroMemory(&msg, sizeof(MSG));

	// ����ڷκ��� ���� �޼����� ���������� �޼��������� ���ϴ�.
	while (!done)
	{
		// ������ �޼��� ó��.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ���� �޽����� ���� ��� �޽��������� Ż��.
			if (msg.message == WM_QUIT)
			{
				done = true;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// ������ �Լ� ó��.
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
	// �ܺ� �����͸� �� ��ü�� ����.
	ApplicationHandle = this;

	// �� ���α׷��� ��ü�� ������.
	m_hInstance = GetModuleHandle(NULL);

	// ���α׷��� �̸��� ����.
	m_applicationName = L"Dx11Practice";

	// Window Ŭ������ �Ʒ��� ���� ����.
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

	// ������ Ŭ���� ���.
	RegisterClassEx(&wc);

	// ����� ȭ�� �ػ󵵸� ����.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	if (FULL_SCREEN)
	{
		// Ǯ��ũ���̶�� ����� ȭ�� �ػ󵵸� ����ũ�� �ػ󵵷� ����.
		// ������ 32Bit�� ����.
		DEVMODE dmScreenSettings; 
		ZeroMemory(&dmScreenSettings, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ������ ���÷��� ����.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// ������ ����� ��� 800 * 600 ���� ����.
		screenWidth = 800;
		screenHeight = 600;

		// ������ â�� ����, ���� ������� ����.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� ����.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
	// Ǯ��ũ�� ����� ���÷��� ���� �ʱ�ȭ.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� ����.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���α׷� ��ü ����.
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// �ܺ����������͸� ����.
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
