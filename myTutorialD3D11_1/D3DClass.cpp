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

	//保持垂直同步设置
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

	//创建一个 DriectX graphics interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	//用工厂接口创建一个主显卡的适配器
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
		return false;

	//得到主适配器的输出
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	//纹理数据通常为以下几种格式：
	//DXGI_FORMAT_R32G32B32_FLOAT : 每个元素包括3个32位浮点数。
	//DXGI_FORMAT_R16G16B16A16_UNORM : 每个元素有4个16位分量，每个分量都被映射到[0, 1] 范围，UNORM表示无符号归一化。
	//DXGI_FORMAT_R32G32_UINT: 每个元素有2个32位无符号整数。
	//DXGI_FORMAT_R8G8B8A8_UNORM :每个元素有4个8位无符号整数，被映射到 [0,1] 范围。
	//DXGI_FORMAT_R8G8B8A8_SNORM : 每个元素有4个8位有符号分量，被映射到[ -1, 1]范围。
	//DXGI_FORMAT_R8G8B8A8_SINT: 每个元素有4个8位有符号整数，被映射到[ ? 128, 127] 范围。
	//D3D 初始化的代码主要在D3DClass的初始化函数中，需要注意的取得显卡的刷新率主要是设置垂直同步时使用：

	//得到适合DXGI_FORMAT_R8G8B8A8_UNORM的显示模式
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

	//遍历所有模式，得到刷新率两个参数值 numerator 和 denominator
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

	//保存显存大小
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//保存显卡描述符
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

	//初始化交换链描述
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//使用一个后缓冲
	swapChainDesc.BufferCount = 1;

	//帧缓冲的大小和应用程序窗口大小相等
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//后缓冲的surface的格式为 DXGI_FORMAT_R8G8B8A8_UNORM
	//surface的每个像素用4个无符号的8bit[映射到0-1]来表示。NORM表示归一化
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//若使用垂直同步，设置后缓冲的刷新率
	//若开启垂直同步，则锁定刷新率，则fps固定

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

	//设置后缓冲的用途
	//我们的渲染目标缓冲为后缓冲
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//后缓冲输出窗口句柄
	swapChainDesc.OutputWindow = hwnd;

	//不使用多重采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//设置全屏或者窗口模式
	if (fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	//设定扫描线ordering以及缩放为unspecified
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//后缓冲内容呈现到屏幕后，放弃内容
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//不设标识
	swapChainDesc.Flags = 0;
	
	//创建交换链，设备以及设备上下文
	result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device);
	if (FAILED(result))
		return false;

	//得到交换链中的后缓冲指针
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		return false;

	//用后缓冲创建渲染目标视图
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
		return false;

	//释放后缓冲（引用计数减1）
	backBufferPtr->Release();
	backBufferPtr = NULL;

	//初始化深度缓冲描述符
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//设置深度缓冲描述
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;//对于深度缓冲为1
	depthBufferDesc.ArraySize = 1;//对于深度缓冲为1，对于纹理,这2个参数更有用
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//创建深度缓冲
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	//初始化深度模版状态描述
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//设置深度模版状态描述
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;

	//D3D10_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲
	depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//对于 font face 像素使用的模版操作模式
	depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	//对于 back face 像素使用的模版操作模式
	depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	//创建深度模版状态
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
		return false;

	//设置深度模版状态，使其生效
	m_device->OMSetDepthStencilState(m_depthStencilState, 1);
	
	//初始化深度模版视图
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//设置深度模版视图描述
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//创建深度模版视图
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
		return false;

	//绑定渲染莫彪视图和深度缓冲到渲染管线
	m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//设置光栅化描述，指定多边形如何被渲染
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

	//创建光栅化状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
		return false;

	//设置光栅化状态，使其生效
	m_device->RSSetState(m_rasterState);

	//设置视口，显示全部后缓冲内容
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//创建视口
	m_device->RSSetViewports(1, &viewport);

	//设置透视投影矩阵
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//创建透视投影矩阵
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear,screenDepth);

	//初始化world矩阵为单位矩阵
	//该矩阵实现局部坐标到世界坐标的转换
	D3DXMatrixIdentity(&m_worldMatrix);

	//创建正交投影矩阵，主要用来实施2D渲染
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
	
	return true;
}


void D3DClass::Shutdown()
{
	// 释放交换链资源前，先设置为窗口模式，否则可能会产生异常.
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


	// 设置清除后缓冲颜色.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//清除后缓冲.

	m_device->ClearRenderTargetView(m_renderTargetView, color);
	//清除深度缓冲.
	m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	//渲染完成后，把后缓冲内容present到前缓冲
	if(m_vsync_enabled)
	{
		// 锁定屏幕刷新率.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 尽可能快的present后缓冲.
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
