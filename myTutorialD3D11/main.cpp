#include <Windows.h>

//�����������
LPCWSTR m_applicationName;
//Ӧ�ó���ʵ�����
HINSTANCE m_hinstance;
//ȫ�ֵĴ��ھ��
HWND m_hwnd;

//�����ж��Ƿ���ESC�����������˳�
bool bexit = false;

//��ʼ�������࣬�������ó��򴰿�
void InitializeWindows(int& screenWidth, int& screenHeight);
//���ó�ʼ�����ں������Լ������ӿ���ĳ�ʼ������
bool Initialize();
//������Ϣѭ��
void Run();

//�رմ���
void ShutdownWindows();

//�������Ǵ��ڻص�����
static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProce(HWND, UINT, WPARAM, LPARAM);

//�ж��Ƿ�ȫ���� ȫ��ģʽ�ʹ���ģʽʹ�ò�ͬ�������ڲ���
const bool FULL_SCREEN = false;

//��ں���
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	Initialize();
	Run();
	ShutdownWindows();
	return 0;
}

//���ó�ʼ��������һЩ������ʼ������
//������ֻ���ó�ʼ�����ں���
bool Initialize()
{
	int screenWidth = 0, screenHeight = 0;

	//��ʼ������
	InitializeWindows(screenWidth, screenHeight);
	return true;
}

void InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

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

//Run�����У�������Ӧ����windows��Ϣ�Լ��������ǵ�reader����
//������Ϣ
void Run()
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

			if (result)
			{
				done = true;
			}
		}
	}
	return;
}

//WndProc����Ϊ���ڻص�������������Ϣ���������������
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
		//��ⰴ����Ϣ
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
			bexit = true;
		//������Ϣ���͸�windows����ȱʡ����
	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

//ShutdownWindows������Ҫ�����ڳ���������ͷ���Դ
void ShutdownWindows()
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

	return;
}
