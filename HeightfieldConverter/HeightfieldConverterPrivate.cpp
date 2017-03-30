#include "HeightfieldConverterPrivate.h"

// инициализаци€
void HeightfieldConverter::HeightfieldConverterPrivate::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode)
{

}

// —оздать триангул€цию немедленно и дождатьс€ готовности
bool HeightfieldConverter::HeightfieldConverterPrivate::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	return true;
}

// добавить/удалить listener
void HeightfieldConverter::HeightfieldConverterPrivate::RegisterListener(HeightfieldConverterListener*)
{

}

void HeightfieldConverter::HeightfieldConverterPrivate::UnregisterListener(HeightfieldConverterListener*)
{

}

// добавить задачу на триангул€цию, котора€ будет выполн€тьс€ асинхронно с помощью DirectCompute
void HeightfieldConverter::HeightfieldConverterPrivate::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{

}

// —оздать ресурсы - нужно об€зательно вызвать после создани€ D3D11Device
void HeightfieldConverter::HeightfieldConverterPrivate::CreateResources()
{

}

// ќсвободить ресурсы - вызываетс€ в случае освобождени€ D3D11Device (например при изменении размоеров окна)
void HeightfieldConverter::HeightfieldConverterPrivate::ReleaseResources()
{

}
