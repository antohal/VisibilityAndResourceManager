#pragma once

#include "vecmath.h"

class CD3DCamera;
struct D3DXMATRIX;

class CD3DCameraController
{
public:

	virtual void			Update(CD3DCamera* in_pCamera, float deltaTime) = 0;

};

class CD3DCamera
{
public:


	void					Set(const vm::Vector3df& in_vPos, const vm::Vector3df& in_vUp, const vm::Vector3df& in_vDir);
	void					SetController(CD3DCameraController* in_pController);

	const vm::Vector3df&	GetPos() const;
	const vm::Vector3df&	GetUp() const;
	const vm::Vector3df&	GetDir() const;

	void					Update(float deltaTime);

	void					GetViewMatrix(D3DXMATRIX& out_ViewMatrix) const;

private:

	vm::Vector3df			_vPosition = vm::Vector3df(0.0);
	vm::Vector3df			_vUp = vm::Vector3df(0, 1, 0);
	vm::Vector3df			_vDirection = vm::Vector3df(0, 0, 1);

	CD3DCameraController*	_pCameraController = nullptr;
};