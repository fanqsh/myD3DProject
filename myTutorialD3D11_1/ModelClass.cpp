#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_indexBuffer = NULL;
	m_vertexBuffer = NULL;
}
ModelClass::ModelClass(const ModelClass &)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D10Device* device)
{
	bool result;

	//��ʼ�����㻺��Ͷ�����������
	result = InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();

	return;
}

void ModelClass::Render(ID3D10Device* device)
{
	RenderBuffers(device);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

bool ModelClass::InitializeBuffers(ID3D10Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//���������ȴ���������ʱ���壬��Ŷ�����������ݣ��Ա����ʹ��
	//���ö��㻺���СΪ3����һ��������
	m_vertexCount = 8;

	//�������������С
	m_indexCount = 36;

	//����������ʱ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	//������������
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;
	
	//����˳ʱ�뷽��������Σ����ֹ���
	//���ö�������
	vertices[0].position = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	vertices[0].color = WHITE;

	vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	vertices[1].color = RED;

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 1.0f);
	vertices[2].color = BLACK;

	vertices[3].position = D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	vertices[3].color = YELLOW;

	vertices[4].position = D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	vertices[4].color = GREEN;

	vertices[5].position = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	vertices[5].color = BLUE;

	vertices[6].position = D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	vertices[6].color = CYAN;

	vertices[7].position = D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	vertices[7].color = MAGENTA;


	//����������������
	indices[0] = 0;		//z = 1
	indices[1] = 1;
	indices[2] = 3;
	indices[3] = 2;
	indices[4] = 0;
	indices[5] = 3;

	indices[6] = 2;		//y = -1
	indices[7] = 3;
	indices[8] = 5;
	indices[9] = 4;
	indices[10] = 2;
	indices[11] = 5;
	
	indices[12] = 5;	//z = -1
	indices[13] = 7;
	indices[14] = 4;
	indices[15] = 5;
	indices[16] = 6;
	indices[17] = 7;

	indices[18] = 1;	//y = 1
	indices[19] = 0;
	indices[20] = 6;
	indices[21] = 0;
	indices[22] = 7;
	indices[23] = 6;

	indices[24] = 0;	//x = 1
	indices[25] = 2;
	indices[26] = 4;
	indices[27] = 0;
	indices[28] = 4;
	indices[29] = 7;

	indices[30] = 3;	//x = -1
	indices[31] = 1;
	indices[32] = 5;
	indices[33] = 5;
	indices[34] = 1;
	indices[35] = 6;

	//���ö��㻺����������
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	
	//ָ�򱣴涥�����ݵ���ʱ����
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//�������㻺��
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	//����������������
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	//ָ����ʱ��������
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	
	//������������
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	//�ͷ���ʱ�ڴ�
	delete[] vertices;
	vertices = NULL;

	delete[] indices;
	indices = NULL;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//�ͷŶ��㻺��
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}

	//�ͷ���������
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	//�� input assemberl �׶ΰ󶨶��㻺�壬�Ա��ܹ�����Ⱦ
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//�� input assemberl �׶ΰ󶨶��㻺�壬�Ա��ܹ�����Ⱦ
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//������Ԫ���壬��Ⱦ�������б�
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}



