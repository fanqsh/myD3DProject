#include "GraphicsClass.h"

GraphicsClass::GraphicsClass(void)
{
	m_D3D = NULL;
	m_Camer = NULL;
	m_Model = NULL;
	m_ColorShader = NULL;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{

}
GraphicsClass::~GraphicsClass(void)
{
}

bool GraphicsClass:: Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	
	//如果已经存在对象，先释放掉
	this->Shutdown();

	//创建D3D对象
	m_D3D = new D3DClass;
	if (!m_D3D)
		return false;

	//初始化D3D对象
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"m_D3D->Initialize 初始化失败！", L"Error!!", MB_OK);
		return false;
	}

	//创建摄像机对象
	m_Camer = new CamerClass;
	if (!m_Camer)
		return false;

	//设置摄像机位置
	D3DXVECTOR3 campos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_Camer->SetPosition(&campos);

	//创建模型对象
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	//初始化模型对象
	result = m_Model->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"m_Model->Initialize 初始化失败！", L"Error!!", MB_OK);
		return false;
	}

	//创建shader对象
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
		return false;

	//初始化shader对象
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"m_ColorShader->Initialize 初始化失败！", L"Error!!", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = NULL;
	}
	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	result = Render();

	if (!result)
	    return false;

	return true;
}


bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix, worldMatrix, projectionMatrix;
	bool result;

	//输出蓝色
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);
	
	//得到三个矩阵
	m_Camer->GetViewMatrix(&viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	
	//把模型顶点和索引缓冲放入管线，准备渲染
	m_Model->Render(m_D3D->GetDevice());
	
	//用shader渲染
	result = m_ColorShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	    return false;

	//显示图像

	m_D3D->EndScene();

	return true;
}