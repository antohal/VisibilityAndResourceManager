#pragma once

#include "vecmath.h"

#include <D3DX10math.h>

class CCamera
{
public:


	void					Set(const vm::Vector3df& in_vPos, const vm::Vector3df& in_vUp, const vm::Vector3df& in_vDir);

	const vm::Vector3df&	GetPos() const;
	const vm::Vector3df&	GetUp() const;
	const vm::Vector3df&	GetDir() const;

	void					GetViewMatrix(D3DXMATRIX& out_ViewMatrix);

private:

	vm::Vector3df			_vPosition = vm::Vector3df(0);
	vm::Vector3df			_vUp = vm::Vector3df(0, 1, 0);
	vm::Vector3df			_vDirection = vm::Vector3df(0, 0, 1);
};