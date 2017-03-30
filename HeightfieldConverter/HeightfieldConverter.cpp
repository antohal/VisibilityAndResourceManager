#include "HeightfieldConverter.h"
#include "HeightfieldConverterPrivate.h"

HeightfieldConverter::HeightfieldConverter()
{
	_private = new HeightfieldConverterPrivate;
}

HeightfieldConverter::~HeightfieldConverter()
{
	delete _private;
}

// �������������
void HeightfieldConverter::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode)
{
	_private->Init(in_pD3DDevice11, in_pDeviceContext, in_Mode);
}

// ������� ������������ ���������� � ��������� ����������
void HeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	_private->CreateTriangulationImmediate(in_pHeightfield, out_pTriangulation);
}

// ��������/������� listener
void HeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
	_private->RegisterListener(in_pListener);
}

void HeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
	_private->UnregisterListener(in_pListener);
}

// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
void HeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	_private->AppendTriangulationTask(in_pHeightfield);
}
