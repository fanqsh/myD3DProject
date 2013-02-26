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
	
	//����Ѿ����ڶ������ͷŵ�
	this->Shutdown();

	//����D3D����
	m_D3D = new D3DClass;
	if (!m_D3D)
		return false;

	//��ʼ��D3D����
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"m_D3D->Initialize ��ʼ��ʧ�ܣ�", L"Error!!", MB_OK);
		return false;
	}

	//�������������
	m_Camer = new CamerClass;
	if (!m_Camer)
		return false;

	//���������λ��
	D3DXVECTOR3 campos = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	m_Camer->SetPosition(&campos);

	//����ģ�Ͷ���
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	//��ʼ��ģ�Ͷ���
	result = m_Model->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"m_Model->Initialize ��ʼ��ʧ�ܣ�", L"Error!!", MB_OK);
		return false;
	}

	//����shader����
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
		return false;

	//��ʼ��shader����
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"m_ColorShader->Initialize ��ʼ��ʧ�ܣ�", L"Error!!", MB_OK);
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

	//�����ɫ
	m_D3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);
	
	//�õ���������
	m_Camer->GetViewMatrix(&viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	
	//��ģ�Ͷ�����������������ߣ�׼����Ⱦ
	m_Model->Render(m_D3D->GetDevice());
	
	//��shader��Ⱦ
	result = m_ColorShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	    return false;

	//��ʾͼ��

	m_D3D->EndScene();

	return true;
}