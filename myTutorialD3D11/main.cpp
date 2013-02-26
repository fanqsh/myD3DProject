#include <Windows.h>

//窗口类的名字
LPCWSTR m_applicationName;
//应用程序实例句柄
HINSTANCE m_hinstance;
//全局的窗口句柄
HWND m_hwnd;

//用于判断是否按下ESC键，按下则退出
bool bexit = false;

//初始化窗口类，创建引用程序窗口
void InitializeWindows(int& screenWidth, int& screenHeight);
//调用初始化窗口函数，以及其它接口类的初始化函数
bool Initialize();
//处理消息循环
void Run();

//关闭窗口
void ShutdownWindows();

//这两个是窗口回调函数
static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProce(HWND, UINT, WPARAM, LPARAM);

//判断是否全屏， 全屏模式和窗口模式使用不同创建窗口参数
const bool FULL_SCREEN = false;

//入口函数
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	Initialize();
	Run();
	ShutdownWindows();
	return 0;
}

//调用初始化函数和一些其它初始化函数
//本例中只调用初始化窗口函数
bool Initialize()
{
	int screenWidth = 0, screenHeight = 0;

	//初始化窗口
	InitializeWindows(screenWidth, screenHeight);
	return true;
}

void InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

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

//Run函数中，我们响应调度windows消息以及调用我们的reader函数
//处理消息
void Run()
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

			if (result)
			{
				done = true;
			}
		}
	}
	return;
}

//WndProc函数为窗口回调函数，程序消息处理都在这个函数中
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
		return MessageHandler(hwnd, umessage, wparam, lparam);
	}
}

LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//检测按键消息
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
			bexit = true;
		//其它消息发送给windows进行缺省处理
	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

//ShutdownWindows函数主要用来在程序结束后，释放资源
void ShutdownWindows()
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

	return;
}
