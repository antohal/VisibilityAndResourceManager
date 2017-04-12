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
void HeightfieldConverter::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext)
{
	_private->Init(in_pD3DDevice11, in_pDeviceContext);
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

void HeightfieldConverter::UpdateTasks()
{
	_private->UpdateTasks();
}

// ������ ���������� ����������� ��������.
// �� ��������� ��� ������� �������� � �������� � ���������� ����� � ������� ��������� WGS-84 � ������
void HeightfieldConverter::SetWorldScale(float in_fScale)
{
	_private->SetWorldScale(in_fScale);
}

// ������� ������ ����� ����� �� ��������
void HeightfieldConverter::ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield)
{
	_private->ReadHeightfieldDataFromTexture(in_pcwszTextureFileName, out_Heightfield);
}

void HeightfieldConverter::ReadHeightfieldDataFromMemory(const unsigned char* in_pData, unsigned int in_nWidth, unsigned int in_nHeight, SHeightfield& out_Heightfield)
{

}

// ���������� ������ ������������
void HeightfieldConverter::ReleaseTriangulation(STriangulation* triangulation)
{
	_private->ReleaseTriangulation(triangulation);
}

void HeightfieldConverter::ReleaseHeightfield(SHeightfield* heightfield)
{
	_private->ReleaseHeightfield(heightfield);
}

// �������� ������ ������ � �������� � ������
void HeightfieldConverter::UnmapTriangulation(STriangulation* triangulation, SVertex* out_pVertexes, unsigned int* out_pIndices)
{
	_private->UnmapTriangulation(triangulation, out_pVertexes, out_pIndices);
}
