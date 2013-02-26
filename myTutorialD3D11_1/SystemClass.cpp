#include "SystemClass.h"

SystemClass::SystemClass(void)
{
	bexit = false;
	m_Input = 0;
	m_Graphics = 0;
	m_timer = 0;
}

SystemClass::SystemClass(const SystemClass&)
{
}

SystemClass::~SystemClass()
{
}

//调用串口初始化函数和其它一些类的初始化函数
//本例子中只调用初始化窗口函数
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	bool result = false;
	//初始化窗口
	InitializeWindows(screenWidth, screenHeight);

	//创建Input对象处理按键输入
	m_Input = new InputClass;
	if (!m_Input)
		return false;
	//初始化输入对象
	m_Input->Initialize();
	
	//创建图形对象，进行渲染所有物体
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
		return false;
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	//创建计时器并初始化
	m_timer = new TimerClass;
	if (!m_timer)
		return false;
	result = m_timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"m_timer->Initialize 初始化失败！", L"error!!", MB_OK);
		return false;
	}
	return true;
}

void SystemClass::Shutdown()
{
	//其他类的一些销毁工作
	//···
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	if (m_timer)
	{
		delete m_timer;
		m_timer = 0;
	}

	//执行窗口的销毁
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result = 1;

	//初始化消息结构
	ZeroMemory(&msg, sizeof(MSG));

	//循环进行消息处理
	done = false;

	while (!done)
	{
		//处理windows消息
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//接收到WM_QUIT消息，退出程序.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = bexit;	//若按了ESC，也退出程序
			//渲染或其它处理可以放在此处

			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
	return;
}

bool SystemClass::Frame()
{
	bool result = false;

	//检测用户是否按下ESC键，按下则退出
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}
	//如果A,S,D,W,Q,E,Z,X,C键按下，移动摄像机
	if(GetAsyncKeyState('W') & 0x8000)    //前后
		m_Graphics->m_Camer->walk(-0.1);
	if(GetAsyncKeyState('S') & 0x8000)
		m_Graphics->m_Camer->walk(0.1);
	if(GetAsyncKeyState('A') & 0x8000)    //左右
		m_Graphics->m_Camer->strafe(-0.1);
	if(GetAsyncKeyState('D') & 0x8000)
		m_Graphics->m_Camer->strafe(0.1);
	if(GetAsyncKeyState('Q') & 0x8000)    //上下
		m_Graphics->m_Camer->fly(-0.1);
	if(GetAsyncKeyState('E') & 0x8000)
		m_Graphics->m_Camer->fly(0.1);
	if(GetAsyncKeyState('Z') & 0x8000)
		m_Graphics->m_Camer->pich(PI/180);
	if(GetAsyncKeyState('X') & 0x8000)
		m_Graphics->m_Camer->yaw(PI/180);
	if(GetAsyncKeyState('C') & 0x8000)
		m_Graphics->m_Camer->roll(PI/180);

	if(GetAsyncKeyState('R') & 0x8000)
		m_Graphics->m_Camer->SetPosition(0.0f, 0.0f, -10.0f);

	//动画，旋转摄像机
	m_Graphics->m_Camer->roll(m_timer->GetTime()*1000);

	result = m_Graphics->Frame();
	if (!result)
		return false;

	return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//获取应用程序句柄
	ApplicationHandle = this;

	//得到应用程序实例句柄
	m_hinstance = GetModuleHandle(NULL);

	//应用程序名字
	m_applicationName = L"二货联盟";

	//设置窗口类参数
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	//制定回调函数，指定后windows会自动调用回调函数处理各类消息事件
	wc.lpfnWndProc		= WndProc;//制定回调函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_hinstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= m_applicationName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	//注册窗口类
	RegisterClassEx(&wc);

	//获取windows桌面分辨率
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//根据是否全屏设置不同分辨率
	if (FULL_SCREEN)
	{
		//全屏模式下设置窗口大小为windows桌面分辨率
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//临时设置显示设备为全屏模式，注意：应用程序呢退出时候，将恢复系统默认设置。
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//设置窗口左上角坐标位置为（0， 0）
		posX = posY = 0;
	}
	else
	{
		//窗口模式 ： 800*600
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//全屏和窗口模式使用不同的参数
	if (FULL_SCREEN)
	{
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	}
	else
	{
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_OVERLAPPEDWINDOW,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	}

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//隐藏鼠标
	//ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	//显示光标
	//ShowCursor(true);

	//恢复默认显示设置
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	//释放窗口句柄
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//释放应用程序实例
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//检测按键消息
	case WM_KEYDOWN:
		{
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}
	case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}
	case WM_SIZE:
		{
			int screenWidth = 0, screenHeight = 0;
			screenWidth = LOWORD(lparam);
			screenHeight = HIWORD(lparam);
			
			//窗口改变大小时，重新初始化图形对象
			if (m_Graphics)
			{
			    bool result = m_Graphics->Initialize(screenWidth, screenHeight, hwnd);
			    if (!result)
				    return false;
			}
			return 0;

		}
		//其它消息发送给windows进行缺省处理
	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
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
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}




