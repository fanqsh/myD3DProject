#include "CamerClass.h"

CamerClass::CamerClass(void)
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	_cameraType = AIRCRAFI;

	_pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

}

CamerClass::CamerClass(const CamerClass &)
{
}

CamerClass::~CamerClass(void)
{
}

void CamerClass::SetPosition(float x, float y, float z)
{
	_pos.x = x;
	_pos.y = y;
	_pos.z = z;
	
	return;
}

void CamerClass::SetPosition(D3DXVECTOR3 *pos)
{
	_pos = *pos;
}

void CamerClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	
	return;
}

D3DXVECTOR3 CamerClass::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}

void CamerClass::SetCameraType(CamerType cameraType)
{
	_cameraType = cameraType;
}

void CamerClass::GetViewMatrix(D3DXMATRIX* V)
{
	//����view�ֲ�����ϵ��������ı˴�����
	D3DXVec3Normalize(&_look, &_look);
	//look X right
	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	// ����view����:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	(*V)(0,0) = _right.x; (*V)(0, 1) = _up.x; (*V)(0, 2) = _look.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) = _right.y; (*V)(1, 1) = _up.y; (*V)(1, 2) = _look.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) = _right.z; (*V)(2, 1) = _up.z; (*V)(2, 2) = _look.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;

	return;
}

void CamerClass::GetLook(D3DXVECTOR3 *look)
{
	*look = _look;
}

void CamerClass::GetUp(D3DXVECTOR3 *up)
{
	*up = _up;
}

void CamerClass::GetRight(D3DXVECTOR3 *right)
{
	*right = _right;
}

void CamerClass::GetPosition(D3DXVECTOR3 *pos)
{
	*pos = _pos;
}

//ɨ�ӣ����ֹ۲췽�򲻱䣬 ��right�����һ��ƽ�Ƶ���һ��
void CamerClass::strafe(float units)
{
	//����x��z���ƶ�
	if (_cameraType == LANOBJECT)
		_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;

	if (_cameraType == AIRCRAFI)
		_pos += _right * units;
}

//����ģʽ��������ָ����up������ƶ�
void CamerClass::fly(float units)
{
	//����y���ƶ�
	if (_cameraType == LANOBJECT)
		_pos.y += units;

	if (_cameraType == AIRCRAFI)
		_pos += _up * units;
}

//���ߣ���������۲췽���ƶ�
void CamerClass::walk(float units)
{
	//����x,z���ƶ�,���ַ�ʽ��ʹ��y��߶Ȳ��䣬x z�ᰴ��������
	if (_cameraType == LANOBJECT)
		_pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;

	if (_cameraType == AIRCRAFI)
		_pos += _look * units;
}

void CamerClass::pich(float angle)
{
	D3DXMATRIX T;
	
	//����LANOBJECT , �����ƣ�0 1 0����ת
	if (_cameraType == LANOBJECT)
		D3DXMatrixRotationY(&T, angle);

	//����AIRCRAFT, Χ��up������ת
	if (_cameraType == AIRCRAFI)
		D3DXMatrixRotationAxis(&T, &_up, angle);

	//Χ��up����y��,��תright��look
	D3DXVec3TransformCoord(&_right, &_right, &T);
	D3DXVec3TransformCoord(&_up, &_up, &T);
}

void CamerClass::yaw(float angle)
{
	D3DXMATRIX T;

	//��LANDOBJECT,��������(0,1,0)��ת��
	if( _cameraType == LANOBJECT )
		D3DXMatrixRotationY(&T, angle);

	//����aircraft,����up������ת
	if( _cameraType == AIRCRAFI )
		D3DXMatrixRotationAxis(&T, &_up, angle);

	// ����up����y�ᣬ��תright��look
	D3DXVec3TransformCoord(&_right,&_right, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);

}

void CamerClass::roll(float angle)
{
	//ֻ��ARICRAFTģʽ����roll��ת
	if (_cameraType == AIRCRAFI)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_look, angle);

		//��look����,��תup��right
		D3DXVec3TransformCoord(&_right, &_right, &T);
		D3DXVec3TransformCoord(&_up, &_up, &T);
	}
}
