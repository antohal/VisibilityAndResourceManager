#pragma once

#include <d3d11.h>

#ifndef HEIGHTFIELD_CONVERTER_EXPORTS
#define HEIGHFIELD_CONVERTER_API __declspec(dllimport)
#else
#define HEIGHFIELD_CONVERTER_API __declspec(dllexport)
#endif

// структура карты высот
struct SHeightfield
{
	float fMinHeight;	// минимальная высота (соответствующая значению 0 в данных)
	float fMaxHeight;	// максимальная высота (соответствующая значению 255 в данных)
	float fSizeX;		// размер по X
	float fSizeY;		// размер по Y
	
	unsigned int nCountX;	// количество точек по X
	unsigned int nCountY;	// количество точек по Y
	const unsigned char*	pcuszData;	// массив данных [количество байт: nCountX*nCountY]
};

// структура вершины
struct SVertex
{
 	float x, y, z;
};

// структура триангуляции, построенной по карте высот
// её легко превратить в буфер вершин и индексов
struct STriangulation
{
	SVertex* 		pVertexData;	// массив вершин
	unsigned int 	nVertexCount;	// количество вершин

	unsigned int*	pIndexData;		// массив индексов
	unsigned int 	nIndexCount;	// количество индексов
};

// Класс-обработчик событий триангуляции (аналог callback)
class HeightfieldConverterListener
{
public:

	// Функция вызывается, когда триангуляция готова
	virtual void 	TriangulationCreated(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) = 0;
};

// Главный класс-триангулятор карт высот
class HEIGHFIELD_CONVERTER_API HeightfieldConverter
{
public:

	HeightfieldConverter();
	~HeightfieldConverter();
	
	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11);

	// Создать ресурсы - нужно обязательно вызвать после создания D3D11Device
	void	CreateResources();

	// Освободить ресурсы - вызывается в случае освобождения D3D11Device (например при изменении размоеров окна)
	void	ReleaseResources();

	// Создать триангуляцию немедленно и дождаться готовности
	bool	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// добавить/удалить listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

private:

	struct HeightfieldConverterPrivate;
	HeightfieldConverterPrivate*	_private = nullptr;
};
