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

	//初始化顶点缓冲和顶点索引缓冲
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

	//首先我们先创建两个临时缓冲，存放顶点和索引数据，以便后面使用
	//设置顶点缓冲大小为3，即一个三角形
	m_vertexCount = 8;

	//设置索引缓冲大小
	m_indexCount = 36;

	//创建顶点临时缓冲
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
		return false;

	//创建索引缓冲
	indices = new unsigned long[m_indexCount];
	if (!indices)
		return false;
	
	//创建顺时针方向的三角形，左手规则
	//设置顶点数据
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


	//设置索引缓冲数据
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

	//设置顶点缓冲数据描述
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	
	//指向保存顶点数据的临时缓冲
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//创建顶点缓冲
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	//设置索引缓冲描述
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	//指向临时索引缓冲
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	
	//创建索引缓冲
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	//释放临时内存
	delete[] vertices;
	vertices = NULL;

	delete[] indices;
	indices = NULL;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	//释放顶点缓冲
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}

	//释放索引缓冲
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

	//在 input assemberl 阶段绑定顶点缓冲，以便能够被渲染
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//在 input assemberl 阶段绑定顶点缓冲，以便能够被渲染
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//设置体元语义，渲染三角形列表
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}



