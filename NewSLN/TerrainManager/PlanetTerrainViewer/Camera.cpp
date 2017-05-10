#include "Camera.h"
#include "D3DX10.h"

void CD3DCamera::Set(const vm::Vector3df& in_vPos, const vm::Vector3df& in_vUp, const vm::Vector3df& in_vDir)
{
	_vPosition = in_vPos;
	_vUp = in_vUp;
	_vDirection = in_vDir;
}

void CD3DCamera::SetController(CD3DCameraController* in_pController)
{
	_pCameraController = in_pController;
}

const vm::Vector3df& CD3DCamera::GetPos() const
{
	return _vPosition;
}

const vm::Vector3df& CD3DCamera::GetUp() const
{
	return _vUp;
}

const vm::Vector3df& CD3DCamera::GetDir() const
{
	return _vDirection;
}

void CD3DCamera::GetViewMatrix(D3DXMATRIX& out_ViewMatrix) const
{
	D3DXVECTOR3 up, position, lookAt;

	up = D3DXVECTOR3((float)_vUp[0], (float)_vUp[1], (float)_vUp[2]);
	position = D3DXVECTOR3((float)_vPosition[0], (float)_vPosition[1], (float)_vPosition[2]);
	lookAt = D3DXVECTOR3((float)(_vPosition + _vDirection)[0], (float)(_vPosition + _vDirection)[1], (float)(_vPosition + _vDirection)[2]);

	D3DXMatrixLookAtLH(&out_ViewMatrix, &position, &lookAt, &up);
}

void CD3DCamera::Update(float deltaTime)
{
	if (_pCameraController)
		_pCameraController->Update(this, deltaTime);
}
