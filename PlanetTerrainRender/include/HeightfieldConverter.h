#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#ifndef HEIGHTFIELD_CONVERTER_EXPORTS
#define HEIGHFIELD_CONVERTER_API __declspec(dllimport)
#else
#define HEIGHFIELD_CONVERTER_API __declspec(dllexport)
#endif

// структура карты высот
struct SHeightfield
{
	// конфигурация карты высот
	struct SHeightfieldConfig
	{
		float fMinLattitude;					// минимальная широта (рад)
		float fMaxLattitude;					// максимальная широта (рад)

		float fMinLongitude;					// минимальная долгота (рад)
		float fMaxLongitude;					// максимальная долгота (рад)

		float fMinHeight;						// минимальная высота над уровнем эллипсоида WGS-84, метры (соответствующая значению 0 в данных)
		float fMaxHeight;						// максимальная высота над уровнем эллипсоида WGS-84, метры (соответствующая значению 255 в данных)

		unsigned int nCountX;					// количество точек по долготе
		unsigned int nCountY;					// количество точек по широте

		unsigned int nChannel = 0;				// номер канала в текстуре
	};							

	unsigned long long				ID;			// идентификатор
	SHeightfieldConfig				Config;		// конфигурация
	
	ID3D11ShaderResourceView*		pTextureSRV = nullptr;	// текстура с данными
};

// структура вершины
struct SVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR2 texture;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 binormal;
};

// структура триангуляции, построенной по карте высот, содержащая буферы вершин и индексов
struct STriangulation
{
	unsigned long long	ID;							// идентификатор (совпадает с соответствующим Heightfield)

	ID3D11Buffer*		pVertexBuffer = nullptr;	// Буффер вершин
	ID3D11Buffer*		pIndexBuffer = nullptr;		// Буффер индексов

	unsigned int		nVertexCount = 0;			// количество вершин
	unsigned int		nIndexCount = 0;			// количество индексов

	double				vPosition[3];				// Точка начала координат триангуляции (лежит на поверхности эллипсоида)
	double				vXAxis[3];					// Координаты оси X (направлена на север вдоль поверхности эллипсоида)
	double				vYAxis[3];					// Координаты оси Y (направлена по нормали к поверхности эллипсоида)
	double				vZAxis[3];					// Координаты оси Z (направлена на восток вдоль поверхности эллипсоида)

	double				vBoundBoxMinimum[3];		// Минимум баунд-бокса
	double				vBoundBoxMaximum[3];		// Максимум баунд-бокса
};

// Класс-обработчик событий триангуляции (аналог callback)
class HeightfieldConverterListener
{
public:

	// Функция вызывается, когда триангуляция готова
	virtual void 	TriangulationCreated(const STriangulation* in_pTriangulation) = 0;
};

// Главный класс-триангулятор карт высот
class HEIGHFIELD_CONVERTER_API HeightfieldConverter
{
public:

	HeightfieldConverter();
	~HeightfieldConverter();
	
	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext);

	// Задать глобальный коэффициент масштаба.
	// По умолчанию все расчеты ведуться в привязке к эллипсоиду Земли в системе координат WGS-84 в метрах
	void	SetWorldScale(float in_fScale);

	// Считать данные карты высот из текстуры
	void	ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield);

	// Считать данные карты высот из памяти
	void	ReadHeightfieldDataFromMemory(const unsigned char* in_pData, unsigned int in_nWidth, unsigned int in_nHeight, SHeightfield& out_Heightfield);

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// добавить/удалить listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

	// обработать поставленные задачи
	void	UpdateTasks();

	// Освободить карту высот
	void	ReleaseHeightfield(SHeightfield*);

	// Освободить буферы триангуляции
	void	ReleaseTriangulation(STriangulation*);

	// Получить буферы вершин и индексов в памяти
	void	UnmapTriangulation(STriangulation*, SVertex* out_pVertexes, unsigned int* out_pIndices);

private:

	struct HeightfieldConverterPrivate;
	HeightfieldConverterPrivate*	_private = nullptr;

	friend class DirectComputeHeightfieldConverter;
};
