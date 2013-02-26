#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "InputClass.h"
#include "GraphicsClass.h"
#include "TimerClass.h"

static bool bexit = false;
const float PI = 3.14159265358979323f;

class SystemClass
{
public:
	SystemClass(void);
	SystemClass(const SystemClass&);
	~SystemClass(void);

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	int cc;
	InputClass* m_Input;
	GraphicsClass* m_Graphics;
	TimerClass* m_timer;

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
};

//定义静态的回调函数以及应用程序句柄
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;