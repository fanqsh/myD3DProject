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

//���ô��ڳ�ʼ������������һЩ��ĳ�ʼ������
//��������ֻ���ó�ʼ�����ں���
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	bool result = false;
	//��ʼ������
	InitializeWindows(screenWidth, screenHeight);

	//����Input������������
	m_Input = new InputClass;
	if (!m_Input)
		return false;
	//��ʼ���������
	m_Input->Initialize();
	
	//����ͼ�ζ��󣬽�����Ⱦ��������
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
		return false;
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	//������ʱ������ʼ��
	m_timer = new TimerClass;
	if (!m_timer)
		return false;
	result = m_timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"m_timer->Initialize ��ʼ��ʧ�ܣ�", L"error!!", MB_OK);
		return false;
	}
	return true;
}

void SystemClass::Shutdown()
{
	//�������һЩ���ٹ���
	//������
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

	//ִ�д��ڵ�����
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result = 1;

	//��ʼ����Ϣ�ṹ
	ZeroMemory(&msg, sizeof(MSG));

	//ѭ��������Ϣ����
	done = false;

	while (!done)
	{
		//����windows��Ϣ
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//���յ�WM_QUIT��Ϣ���˳�����.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = bexit;	//������ESC��Ҳ�˳�����
			//��Ⱦ������������Է��ڴ˴�

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

	//����û��Ƿ���ESC�����������˳�
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}
	//���A,S,D,W,Q,E,Z,X,C�����£��ƶ������
	if(GetAsyncKeyState('W') & 0x8000)    //ǰ��
		m_Graphics->m_Camer->walk(-0.1);
	if(GetAsyncKeyState('S') & 0x8000)
		m_Graphics->m_Camer->walk(0.1);
	if(GetAsyncKeyState('A') & 0x8000)    //����
		m_Graphics->m_Camer->strafe(-0.1);
	if(GetAsyncKeyState('D') & 0x8000)
		m_Graphics->m_Camer->strafe(0.1);
	if(GetAsyncKeyState('Q') & 0x8000)    //����
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

	//��������ת�����
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

	//��ȡӦ�ó�����
	ApplicationHandle = this;

	//�õ�Ӧ�ó���ʵ�����
	m_hinstance = GetModuleHandle(NULL);

	//Ӧ�ó�������
	m_applicationName = L"��������";

	//���ô��������
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	//�ƶ��ص�������ָ����windows���Զ����ûص��������������Ϣ�¼�
	wc.lpfnWndProc		= WndProc;//�ƶ��ص�����
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

	//ע�ᴰ����
	RegisterClassEx(&wc);

	//��ȡwindows����ֱ���
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//�����Ƿ�ȫ�����ò�ͬ�ֱ���
	if (FULL_SCREEN)
	{
		//ȫ��ģʽ�����ô��ڴ�СΪwindows����ֱ���
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//��ʱ������ʾ�豸Ϊȫ��ģʽ��ע�⣺Ӧ�ó������˳�ʱ�򣬽��ָ�ϵͳĬ�����á�
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//���ô������Ͻ�����λ��Ϊ��0�� 0��
		posX = posY = 0;
	}
	else
	{
		//����ģʽ �� 800*600
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//ȫ���ʹ���ģʽʹ�ò�ͬ�Ĳ���
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

	//�������
	//ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	//��ʾ���
	//ShowCursor(true);

	//�ָ�Ĭ����ʾ����
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	//�ͷŴ��ھ��
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//�ͷ�Ӧ�ó���ʵ��
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//��ⰴ����Ϣ
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
			
			//���ڸı��Сʱ�����³�ʼ��ͼ�ζ���
			if (m_Graphics)
			{
			    bool result = m_Graphics->Initialize(screenWidth, screenHeight, hwnd);
			    if (!result)
				    return false;
			}
			return 0;

		}
		//������Ϣ���͸�windows����ȱʡ����
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




