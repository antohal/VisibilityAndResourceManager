#include "HeightfieldConverter.h"
#include "HeightfieldConverterPrivate.h"
#include "Log.h"

HeightfieldConverter::HeightfieldConverter()
{
	LogInit("HeightfieldConverter.log");
	LogEnable(true);

	_private = new HeightfieldConverterPrivate;
}

HeightfieldConverter::~HeightfieldConverter()
{
	delete _private;
}

// инициализация
void HeightfieldConverter::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcszComputeShaderFile)
{
	_private->Init(in_pD3DDevice11, in_pDeviceContext, in_pcszComputeShaderFile);
}

void HeightfieldConverter::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, ID3DX11Effect* in_pEffect)
{
	_private->Init(in_pD3DDevice11, in_pDeviceContext, in_pEffect);
}

void HeightfieldConverter::SetHeightScale(float in_fHeightScale)
{
	_private->SetHeightScale(in_fHeightScale);
}

float HeightfieldConverter::GetHeightScale() const
{
	return _private->GetHeightScale();
}

void HeightfieldConverter::SetNormalDivisionAngles(float in_fAngle1InDeg, float in_fAngle2InDeg)
{
	_private->SetNormalDivisionAngles(in_fAngle1InDeg, in_fAngle2InDeg);
}

void HeightfieldConverter::ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords)
{
	_private->ComputeTriangulationCoords(in_Coords, out_TriangulationCoords);
}

// Создать триангуляцию немедленно и дождаться готовности
void HeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, STriangulation* out_pTriangulation, const SHeightfield** in_ppNeighbours)
{
	_private->CreateTriangulationImmediate(in_pHeightfield, in_fLongitudeCutCoeff, in_fLattitudeCutCoeff,  out_pTriangulation, in_ppNeighbours);
}

// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
void HeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback in_Callback)
{
	_private->AppendTriangulationTask(in_pHeightfield, in_fLongitudeCutCoeff, in_fLattitudeCutCoeff, param, in_Callback);
}

void HeightfieldConverter::UpdateTasks()
{
	_private->UpdateTasks();
}

// Задать глобальный коэффициент масштаба.
// По умолчанию все расчеты ведуться в привязке к эллипсоиду Земли в системе координат WGS-84 в метрах
void HeightfieldConverter::SetWorldScale(float in_fScale)
{
	_private->SetWorldScale(in_fScale);
}

float HeightfieldConverter::GetWorldScale() const
{
	return _private->GetWorldScale();
}

// Считать данные карты высот из текстуры
void HeightfieldConverter::ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield, unsigned short in_usCompressionRatio)
{
	_private->ReadHeightfieldDataFromTexture(in_pcwszTextureFileName, out_Heightfield, in_usCompressionRatio);
}

void HeightfieldConverter::ReadHeightfieldDataFromMemory(const unsigned char* in_pData, unsigned int in_nWidth, unsigned int in_nHeight, SHeightfield& out_Heightfield)
{

}

void HeightfieldConverter::LockDeviceContext()
{
	_private->LockDeviceContext();
}

void HeightfieldConverter::UnlockDeviceContext()
{
	_private->UnlockDeviceContext();
}

// Освободить буферы триангуляции
void HeightfieldConverter::ReleaseTriangulation(STriangulation* triangulation)
{
	_private->ReleaseTriangulation(triangulation);
}

void HeightfieldConverter::ReleaseHeightfield(SHeightfield* heightfield)
{
	_private->ReleaseHeightfield(heightfield);
}

// Получить буферы вершин и индексов в памяти
bool HeightfieldConverter::UnmapTriangulation(STriangulation* triangulation, SVertex* out_pVertexes, unsigned int* out_pIndices)
{
	return _private->UnmapTriangulation(triangulation, out_pVertexes, out_pIndices);
}
