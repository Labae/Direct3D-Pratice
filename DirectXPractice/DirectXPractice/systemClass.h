#pragma once

class InputClass;
class GraphicsClass;
//class SoundClass;
class CpuClass;
class FpsClass;
class TimeClass;
class PositionClass;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();
	
	LRESULT CALLBACK MessageHander(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	InputClass* m_pInput;
	GraphicsClass* m_pGraphics;
	//SoundClass* m_pSound;
	FpsClass* m_pFps;
	CpuClass* m_pCpu;
	TimeClass* m_pTime;
	PositionClass* m_pPosition;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;
