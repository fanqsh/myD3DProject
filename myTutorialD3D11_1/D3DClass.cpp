#include "D3DClass.h"

D3DClass::D3DClass(void)
{
	m_swapChain = 0;
	m_device = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;

}

D3DClass::D3DClass(const D3DClass& other)
{
}



D3DClass::~D3DClass(void)
{
}


bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;

	//���ִ�ֱͬ������
	m_vsync_enabled = vsync;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator = 0, denominator = 1, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D10Texture2D* backBufferPtr;
	D3D10_TEXTURE2D_DESC depthBufferDesc;
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D10_RASTERIZER_DESC rasterDesc;
	D3D10_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	//����һ�� DriectX graphics interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	//�ù����ӿڴ���һ�����Կ���������
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	//�õ��������������
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	//��������ͨ��Ϊ���¼��ָ�ʽ��
	//DXGI_FORMAT_R32G32B32_FLOAT : ÿ��Ԫ�ذ���3��32λ��������
	//DXGI_FORMAT_R16G16B16A16_UNORM : ÿ��Ԫ����4��16λ������ÿ����������ӳ�䵽[0, 1] ��Χ��UNORM��ʾ�޷��Ź�һ����
	//DXGI_FORMAT_R32G32_UINT: ÿ��Ԫ����2��32λ�޷���������
	//DXGI_FORMAT_R8G8B8A8_UNORM :ÿ��Ԫ����4��8λ�޷�����������ӳ�䵽 [0,1] ��Χ��
	//DXGI_FORMAT_R8G8B8A8_SNORM : ÿ��Ԫ����4��8λ�з��ŷ�������ӳ�䵽[ -1, 1]��Χ��
	//DXGI_FORMAT_R8G8B8A8_SINT: ÿ��Ԫ����4��8λ�з�����������ӳ�䵽[ ? 128, 127] ��Χ��
	//D3D ��ʼ���Ĵ�����Ҫ��D3DClass�ĳ�ʼ�������У���Ҫע���ȡ���Կ���ˢ������Ҫ�����ô�ֱͬ��ʱʹ�ã�

	//�õ��ʺ�DXGI_FORMAT_R8G8B8A8_UNORM����ʾģʽ
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, 
		&numModes, NULL);
	if (FAILED(result))
		return false;

	displayModeList = new DXGI_MODE_DESC[numModes];
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	//��������ģʽ���õ�ˢ������������ֵ numerator �� denominator
	for (i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth && displayModeList[i].Height == (unsigned int)screenHeight)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
		return false;

	//�����Դ��С
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//�����Կ�������
	//
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description,128);
	if (error != 0)
		return false;

	delete[] displayModeList;
	displayModeList = NULL;

	adapterOutput->Release();
	adapterOutput = NULL;

	adapter->Release();
	adapter = NULL;

	factory->Release();
	factory = NULL;

	//��ʼ������������
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//ʹ��һ���󻺳�
	swapChainDesc.BufferCount = 1;

	//֡����Ĵ�С��Ӧ�ó��򴰿ڴ�С���
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//�󻺳��surface�ĸ�ʽΪ DXGI_FORMAT_R8G8B8A8_UNORM
	//surface��ÿ��������4���޷��ŵ�8bit[ӳ�䵽0-1]����ʾ��NORM��ʾ��һ��
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	//��ʹ�ô�ֱͬ�������ú󻺳��ˢ����
	//��������ֱͬ����������ˢ���ʣ���fps�̶�

	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	}

	//���ú󻺳����;
	//���ǵ���ȾĿ�껺��Ϊ�󻺳�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//�󻺳�������ھ��
	swapChainDesc.OutputWindow = hwnd;

	//��ʹ�ö��ز���
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//����ȫ�����ߴ���ģʽ
	if (fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	//�趨ɨ����ordering�Լ�����Ϊunspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//�󻺳����ݳ��ֵ���Ļ�󣬷�������
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�����ʶ
	swapChainDesc.Flags = 0;
	
	//�������������豸�Լ��豸������
	result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device);
	if (FAILED(result))
		return false;

	//�õ��������еĺ󻺳�ָ��
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		return false;

	//�ú󻺳崴����ȾĿ����ͼ
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
		return false;

	//�ͷź󻺳壨���ü�����1��
	backBufferPtr->Release();
	backBufferPtr = NULL;

	//��ʼ����Ȼ���������
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//������Ȼ�������
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;//������Ȼ���Ϊ1
	depthBufferDesc.ArraySize = 1;//������Ȼ���Ϊ1����������,��2������������
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//������Ȼ���
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	//��ʼ�����ģ��״̬����
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//�������ģ��״̬����
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;

	//D3D10_DEPTH_WRITE_MASK_ZERO��ֹд��Ȼ���
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//���� font face ����ʹ�õ�ģ�����ģʽ
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	//���� back face ����ʹ�õ�ģ�����ģʽ
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	//�������ģ��״̬
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
		return false;

	//�������ģ��״̬��ʹ����Ч
	m_device->OMSetDepthStencilState(m_depthStencilState, 1);
	
	//��ʼ�����ģ����ͼ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//�������ģ����ͼ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//�������ģ����ͼ
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
		return false;

	//����ȾĪ����ͼ����Ȼ��嵽��Ⱦ����
	m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//���ù�դ��������ָ���������α���Ⱦ
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D10_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D10_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//������դ��״̬
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
		return false;

	//���ù�դ��״̬��ʹ����Ч
	m_device->RSSetState(m_rasterState);

	//�����ӿڣ���ʾȫ���󻺳�����
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//�����ӿ�
	m_device->RSSetViewports(1, &viewport);

	//����͸��ͶӰ����
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//����͸��ͶӰ����
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear,screenDepth);

	//��ʼ��world����Ϊ��λ����
	//�þ���ʵ�־ֲ����굽���������ת��
	D3DXMatrixIdentity(&m_worldMatrix);

	//��������ͶӰ������Ҫ����ʵʩ2D��Ⱦ
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	
	return true;
}


void D3DClass::Shutdown()
{
	// �ͷŽ�������Դǰ��������Ϊ����ģʽ��������ܻ�����쳣.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// ��������󻺳���ɫ.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//����󻺳�.

	m_device->ClearRenderTargetView(m_renderTargetView, color);
	//�����Ȼ���.
	m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	//��Ⱦ��ɺ󣬰Ѻ󻺳�����present��ǰ����
	if(m_vsync_enabled)
	{
		// ������Ļˢ����.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// �����ܿ��present�󻺳�.
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D10Device* D3DClass::GetDevice()
{
	return m_device;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}