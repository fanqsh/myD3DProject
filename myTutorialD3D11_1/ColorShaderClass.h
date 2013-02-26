#pragma once
#include <d3d10.h>
#include <D3DX10math.h>
#include <d3dx10async.h>
#include <fstream>
using namespace std;

class ColorShaderClass
{
private:
	// 和 shader 中 const buffer 一致，主要用来给这些shader参数赋值
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

public:
	ColorShaderClass(void);
	ColorShaderClass(const ColorShaderClass &);
	~ColorShaderClass(void);

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	bool Render(ID3D10Device*, int, D3DXMATRIX, D3DMATRIX, D3DMATRIX);

private:
	bool InitializeShader(ID3D10Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetSharderParameters(ID3D10Device*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void RenderShader(ID3D10Device*, int);

	ID3D10VertexShader* m_verterShader;
	ID3D10PixelShader* m_pixelShader;
	ID3D10InputLayout* m_layout;
	ID3D10Buffer* m_matrixBuffer;
};