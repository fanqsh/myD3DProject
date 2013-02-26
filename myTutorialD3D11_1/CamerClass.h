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

	void strafe(float units);	//����
	void fly(float units);		//����
	void walk(float units);		//ǰ��

	void pich(float angle);		//��תview����ϵright����
	void yaw(float angle);		//��תup����
	void roll(float angle);		//��תlook����

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
	float m_positionX, m_positionY, m_positionZ; //��������ڵ���������ϵ��λ��
	float m_rotationX, m_rotationY, m_rotationZ; //�������ŷ����ת�Ƕ�

	D3DXMATRIX m_viewMatrix;

	CamerType _cameraType;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
};
