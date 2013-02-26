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

	//��ʼ�� vs �� ps
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

	//����shader����
	result = SetSharderParameters(device, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
		return false;

	//��shader��Ⱦָ�����嶥��
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

	//��ʼ��ָ��Ϊ��
	errorMessage = NULL;
	vertexShaderBuffer = NULL;
	pixelShaderBuffer = NULL;

	//����vs����
	result = D3DX10CompileFromFile(vsFileName, NULL, NULL, "ColorVertexShader", "vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//����ʧ�ܣ����������Ϣ
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		//û�д�����Ϣ��������shader�ļ���ʧ
		else
			MessageBox(hwnd, vsFileName, L"Missing Shader File", MB_OK);
		
		return false;
	}

	//����ps����
	result = D3DX10CompileFromFile(psFileName, NULL, NULL, "ColorPixelShader", "ps_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//����ʧ�ܣ����������Ϣ
		if (errorMessage)
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		//û�д�����Ϣ��������shader�ļ���ʧ
		else
			MessageBox(hwnd, psFileName, L"Missing Shader File", MB_OK);

		return false;
	}
	//�ӻ����д���vs shader 
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_verterShader);
	if (FAILED(result))
		return false;

	//�ӻ����д���ps shader
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), &m_pixelShader);
	if (FAILED(result))
		return false;

	// �������ݲ��֣��Ա���shader��ʹ��.
	// ����Ҫ��ModelClass�еĶ���ṹһ��.
	polygonLayout[0].SemanticName = "POSITION";//vs�е�������� 
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

	// �õ�layout�е�Ԫ������
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//�����������벼��
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
		return false;

	//�ͷŶ�������ػ���.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���ö�̬��������.
	matrixBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;

	//����const buffָ�룬�Ա���ʳ���
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

	//��ȡbufferָ��
	compileErrors = (char*)errorMessage->GetBufferPointer();

	//��ȡ����
	bufferSize = (unsigned long)errorMessage->GetBufferSize();

	//��һ���ļ���д������Ϣ
	fout.open("shader_error.txt");

	//���������Ϣ
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	//�ر��ļ�
	fout.close();

	//�ͷŴ�����Ϣ
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

	//����shaderǰȷ������ת��
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectMatrix, &projectMatrix);

	//�����������壬ȷ���ܱ�д��
	result = m_matrixBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &mappedResource.pData);
	if (FAILED(result))
		return false;

	//��ȡconst bufferָ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//����world view �Լ� projection ����
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectMatrix;

	//��������
	m_matrixBuffer->Unmap();

	//���û���λ��
	bufferNumber = 0;

	//ʹ�ø��º��ֵ���ó�������
	device->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D10Device* device, int indexCount)
{
	//�󶨶��㲼��
	device->IASetInputLayout(m_layout);

	//������Ⱦʹ��vs ��ps
	device->VSSetShader(m_verterShader);
	device->PSSetShader(m_pixelShader);

	//��Ⱦ������
	device->DrawIndexed(indexCount, 0, 0);

}