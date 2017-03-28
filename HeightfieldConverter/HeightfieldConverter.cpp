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

// инициализация
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

// Создать триангуляцию немедленно и дождаться готовности
bool HeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	return _private->CreateTriangulationImmediate(in_pHeightfield, out_pTriangulation);
}

// добавить/удалить listener
void HeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
	_private->RegisterListener(in_pListener);
}

void HeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
	_private->UnregisterListener(in_pListener);
}

// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
void HeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	_private->AppendTriangulationTask(in_pHeightfield);
}
