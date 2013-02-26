#pragma once

//���ӳ���ʹ�õ�һЩD3D��
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx10.lib")

//D3D��һЩͷ�ļ�
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d10.h>
#include "D3DX10math.h"

class D3DClass
{
public:
	D3DClass(void);
	D3DClass(const D3DClass&);
	~D3DClass(void);

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D10Device* GetDevice();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled; //�Ƿ����ô�ֱͬ��
	int m_videoCardMemory; //�Դ��С
	char m_videoCardDescription[128]; //�Կ�����
	IDXGISwapChain* m_swapChain; //����������
	ID3D10Device* m_device;	//�豸����
	ID3D10RenderTargetView* m_renderTargetView; //��ȾĿ����ͼ
	ID3D10Texture2D* m_depthStencilBuffer;
	ID3D10DepthStencilState* m_depthStencilState;
	ID3D10DepthStencilView* m_depthStencilView; //���Ŀ����ͼ
	ID3D10RasterizerState* m_rasterState; //��Ⱦ״̬
	D3DXMATRIX m_projectionMatrix; //ͶӰ����
	D3DXMATRIX m_worldMatrix; //��������ϵ����
	D3DXMATRIX m_orthoMatrix; //����ͶӰ����


};