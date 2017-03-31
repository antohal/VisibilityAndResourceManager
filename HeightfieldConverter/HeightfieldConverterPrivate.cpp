#include "HeightfieldConverterPrivate.h"

#include "SoftwareConverter.h"
#include "DirectComputeConverter.h"

HeightfieldConverter::HeightfieldConverterPrivate::~HeightfieldConverterPrivate()
{
	if (_pAbstractConverter)
		delete _pAbstractConverter;
}

// �������������
void HeightfieldConverter::HeightfieldConverterPrivate::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode)
{
	switch (in_Mode)
	{
	case SOFTWARE_MODE:
		_pAbstractConverter = new SoftwareHeightfieldConverter();
		break;
	}
}

// ������� ������������ ���������� � ��������� ����������
void HeightfieldConverter::HeightfieldConverterPrivate::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	if (_pAbstractConverter)
		_pAbstractConverter->CreateTriangulationImmediate(in_pHeightfield, out_pTriangulation);
}

// ��������/������� listener
void HeightfieldConverter::HeightfieldConverterPrivate::RegisterListener(HeightfieldConverterListener* listener)
{
	if (_pAbstractConverter)
		_pAbstractConverter->RegisterListener(listener);
}

void HeightfieldConverter::HeightfieldConverterPrivate::UnregisterListener(HeightfieldConverterListener* listener)
{
	if (_pAbstractConverter)
		_pAbstractConverter->UnregisterListener(listener);
}

// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
void HeightfieldConverter::HeightfieldConverterPrivate::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	if (_pAbstractConverter)
		_pAbstractConverter->AppendTriangulationTask(in_pHeightfield);
}
