#include "HeightfieldConverterPrivate.h"

// �������������
void HeightfieldConverter::HeightfieldConverterPrivate::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode)
{

}

// ������� ������������ ���������� � ��������� ����������
bool HeightfieldConverter::HeightfieldConverterPrivate::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	return true;
}

// ��������/������� listener
void HeightfieldConverter::HeightfieldConverterPrivate::RegisterListener(HeightfieldConverterListener*)
{

}

void HeightfieldConverter::HeightfieldConverterPrivate::UnregisterListener(HeightfieldConverterListener*)
{

}

// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
void HeightfieldConverter::HeightfieldConverterPrivate::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{

}

// ������� ������� - ����� ����������� ������� ����� �������� D3D11Device
void HeightfieldConverter::HeightfieldConverterPrivate::CreateResources()
{

}

// ���������� ������� - ���������� � ������ ������������ D3D11Device (�������� ��� ��������� ��������� ����)
void HeightfieldConverter::HeightfieldConverterPrivate::ReleaseResources()
{

}
