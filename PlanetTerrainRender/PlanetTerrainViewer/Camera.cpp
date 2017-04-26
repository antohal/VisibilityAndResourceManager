#include "Camera.h"

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

void CD3DCamera::GetViewMatrix(vm::Matrix4x4df& out_ViewMatrix)
{

}

void CD3DCamera::Update(float deltaTime)
{
	if (_pCameraController)
		_pCameraController->Update(this, deltaTime);
}
