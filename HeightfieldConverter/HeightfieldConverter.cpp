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
void HeightfieldConverter::Init(ID3D11Device* in_pD3DDevice11)
{
	_private->Init(in_pD3DDevice11);
}

void HeightfieldConverter::CreateResources()
{
	_private->CreateResources();
}

void HeightfieldConverter::ReleaseResources()
{
	_private->ReleaseResources();
}

// ������� ������������ ���������� � ��������� ����������
bool HeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	return _private->CreateTriangulationImmediate(in_pHeightfield, out_pTriangulation);
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
