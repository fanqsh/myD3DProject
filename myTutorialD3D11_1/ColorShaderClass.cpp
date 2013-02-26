#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass(void)
{
	m_verterShader = NULL;
	m_pixelShader = NULL;
	m_layout = NULL;
	m_matrixBuffer = NULL;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass &)
{
}

ColorShaderClass::~ColorShaderClass(void)
{
}

bool ColorShaderClass::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;

	//初始化 vs 和 ps
	result = InitializeShader(device, hwnd, L"color.vs", L"color.ps");
	if (!result)
		return false;

	return true;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D10Device* device, int indexContent, 
							  D3DXMATRIX worldMatrix, D3DMATRIX viewMatrix, D3DMATRIX projectionMatrix)
{
	bool result;

	//设置shader参数
	result = SetSharderParameters(device, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
		return false;

	//用shader渲染指定缓冲顶点
	RenderShader(device, indexContent);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D10_BUFFER_DESC matrixBufferDesc;

	//初始化指针为空
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	//编译vs代码
	result = D3DX10CompileFromFile(vsFileName, NULL, NULL, "ColorVertexShader", "vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//编译失败，输出错误信息
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		//没有错误信息，可能是shader文件丢失
		else
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		
		return false;
	}

	//编译ps代码
	result = D3DX10CompileFromFile(psFileName, NULL, NULL, "ColorPixelShader", "ps_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//编译失败，输出错误信息
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		//没有错误信息，可能是shader文件丢失
		else
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);

		return false;
	}
	//从缓冲中创建vs shader 
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_verterShader);
	if (FAILED(result))
		return false;

	//从缓冲中创建ps shader
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), &m_pixelShader);
	if (FAILED(result))
		return false;

	// 设置数据布局，以便在shader中使用.
	// 定义要和ModelClass中的顶点结构一致.
	polygonLayout[0].SemanticName = "POSITION";//vs中的输入参数 
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 得到layout中的元素数量
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//创建顶点输入布局
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	//释放顶点和像素缓冲.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 设置动态矩阵描述.
	matrixBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;

	//创建const buff指针，以便访问常量
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
		return false;

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = NULL;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = NULL;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = NULL;
	}

	if (m_verterShader)
	{
		m_verterShader->Release();
		m_verterShader = NULL;
	}
}


void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	//获取buffer指针
	compileErrors = (char*)errorMessage->GetBufferPointer();

	//获取长度
	bufferSize = (unsigned long)errorMessage->GetBufferSize();

	//打开一个文件，写错误信息
	fout.open("shader_error.txt");

	//输出错误信息
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	//关闭文件
	fout.close();

	//释放错误信息
	errorMessage->Release();
	errorMessage = NULL;

	return;
}

bool ColorShaderClass::SetSharderParameters(ID3D10Device* device, D3DXMATRIX worldMatrix, 
											D3DXMATRIX viewMatrix, D3DXMATRIX projectMatrix)
{
	HRESULT result;
	D3D10_MAPPED_TEXTURE3D mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//传入shader前确保矩阵转置
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectMatrix, &projectMatrix);

	//锁定常量缓冲，确保能被写入
	result = m_matrixBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &mappedResource.pData);
	if (FAILED(result))
		return false;

	//获取const buffer指针
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//设置world view 以及 projection 矩阵
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectMatrix;

	//解锁缓冲
	m_matrixBuffer->Unmap();

	//设置缓冲位置
	bufferNumber = 0;

	//使用更新后的值设置常量缓冲
	device->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D10Device* device, int indexCount)
{
	//绑定顶点布局
	device->IASetInputLayout(m_layout);

	//设置渲染使用vs 和ps
	device->VSSetShader(m_verterShader);
	device->PSSetShader(m_pixelShader);

	//渲染三角形
	device->DrawIndexed(indexCount, 0, 0);

}