#pragma once

#include <d3d11.h>
#include <vector>

#ifndef HEIGHTFIELD_CONVERTER_EXPORTS
#define HEIGHFIELD_CONVERTER_API __declspec(dllimport)
#else
#define HEIGHFIELD_CONVERTER_API __declspec(dllexport)
#endif

// структура карты высот
struct SHeightfield
{
	unsigned long long	ID;						// идентификатор

	float fMinHeight;							// минимальная высота (соответствующая значению 0 в данных)
	float fMaxHeight;							// максимальная высота (соответствующая значению 255 в данных)
	float fSizeX;								// размер по X
	float fSizeY;								// размер по Y
	
	unsigned int nCountX;						// количество точек по X
	unsigned int nCountY;						// количество точек по Y
	std::vector<unsigned char>	vecData;		// массив данных [количество байт: nCountX*nCountY]
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
	unsigned long long	ID;						// идентификатор (совпадает с соответствующим Heightfield)

	std::vector<SVertex>		vecVertexData;	// массив вершин
	std::vector<unsigned int>	vecIndexData;	// массив индексов
};

// Класс-обработчик событий триангуляции (аналог callback)
class HeightfieldConverterListener
{
public:

	// Функция вызывается, когда триангуляция готова
	virtual void 	TriangulationCreated(const STriangulation* in_pTriangulation) = 0;
};

enum EHeightfieldConverterMode
{
	SOFTWARE_MODE,
	DIRECT_COMPUTE_MODE
};

// Главный класс-триангулятор карт высот
class HEIGHFIELD_CONVERTER_API HeightfieldConverter
{
public:

	HeightfieldConverter();
	~HeightfieldConverter();
	
	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode);

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// добавить/удалить listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

private:

	struct HeightfieldConverterPrivate;
	HeightfieldConverterPrivate*	_private = nullptr;
};
