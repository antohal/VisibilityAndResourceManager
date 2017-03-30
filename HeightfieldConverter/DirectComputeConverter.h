#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include "AbstractConverter.h"

class DirectComputeHeightfieldConverter : public IAbstractHeightfieldConverter
{
public:

	DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext);

	//@{ IAbstractHeightfieldConverter

	// ������� ������������ ���������� � ��������� ����������
	virtual void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) override;

	// ��������/������� listener
	virtual void	RegisterListener(HeightfieldConverterListener*) override;
	virtual void 	UnregisterListener(HeightfieldConverterListener*) override;

	// �������� ������ �� ������������, ������� ����� ����������� ����������
	virtual void	AppendTriangulationTask(const SHeightfield* in_pHeightfield) override;

	//@}

private:

	CComPtr<ID3D11Device>           m_ptrD3DDevice;
	CComPtr<ID3D11ComputeShader>	m_ptrComputeShader;

};
