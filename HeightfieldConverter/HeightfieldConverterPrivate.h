#pragma once

#include "HeightfieldConverter.h"

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	// инициализаци€
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode);

	// —оздать ресурсы - нужно об€зательно вызвать после создани€ D3D11Device
	void	CreateResources();

	// ќсвободить ресурсы - вызываетс€ в случае освобождени€ D3D11Device (например при изменении размоеров окна)
	void	ReleaseResources();

	// —оздать триангул€цию немедленно и дождатьс€ готовности
	bool	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// добавить/удалить listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// добавить задачу на триангул€цию, котора€ будет выполн€тьс€ асинхронно с помощью DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);


private:


};