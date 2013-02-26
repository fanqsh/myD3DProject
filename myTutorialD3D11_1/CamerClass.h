#pragma once
#include <D3DX10math.h>

class CamerClass
{
public:
	enum CamerType {LANOBJECT, AIRCRAFI};
public:
	CamerClass(void);
	CamerClass(const CamerClass &);
	~CamerClass(void);

	void strafe(float units);	//左右
	void fly(float units);		//上下
	void walk(float units);		//前后

	void pich(float angle);		//旋转view坐标系right向量
	void yaw(float angle);		//旋转up向量
	void roll(float angle);		//旋转look向量

	void GetRight(D3DXVECTOR3 *right);
	void GetUp(D3DXVECTOR3 *up);
	void GetLook(D3DXVECTOR3 *look);

	void GetPosition(D3DXVECTOR3 *pos);
	void SetPosition(D3DXVECTOR3 *pos);
	void SetPosition(float, float, float);

	D3DXVECTOR3 GetRotation();
	void SetRotation(float, float, float);

	void Render();
	void GetViewMatrix(D3DXMATRIX*);
	void SetCameraType(CamerType cameraType);

private:
	float m_positionX, m_positionY, m_positionZ; //摄像机所在的世界坐标系中位置
	float m_rotationX, m_rotationY, m_rotationZ; //摄像机的欧拉旋转角度

	D3DXMATRIX m_viewMatrix;

	CamerType _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
};
