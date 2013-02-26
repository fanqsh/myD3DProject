#pragma once
#include <windows.h>
#include "D3DClass.h"
#include "CamerClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"

/////////////
// GLOBALS //
/////////////

const bool FULL_SCREEN = false; //�Ƿ�ȫ��
const bool VSYNC_ENABLED = true; //�Ƿ�ֱͬ��
const float SCREEN_DEPTH = 1000.0f; //��ȣ�Զ��
const float SCREEN_NEAR = 0.1f; //��ȣ�����

class GraphicsClass
{
public:
	CamerClass* m_Camer;

public:
	GraphicsClass(void);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass(void);
	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
	
	D3DClass* m_D3D;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;
};