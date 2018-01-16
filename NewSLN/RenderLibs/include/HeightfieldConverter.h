#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <D3DX11.h>
#include <d3dx11effect.h>

#ifndef HEIGHTFIELD_CONVERTER_EXPORTS
#define HEIGHFIELD_CONVERTER_API __declspec(dllimport)
#else
#define HEIGHFIELD_CONVERTER_API __declspec(dllexport)
#endif

// структура карты высот
struct SHeightfield
{
	// Координаты карты высот
	struct SCoordinates
	{
		float					fMinLattitude;				// минимальная широта (рад)
		float					fMaxLattitude;				// максимальная широта (рад)

		float					fMinLongitude;				// минимальная долгота (рад)
		float					fMaxLongitude;				// максимальная долгота (рад)
	};

	// Конфигурация карты высот
	struct SConfig
	{
		SCoordinates			Coords;

		unsigned int			nCountX = 256;				// количество точек по долготе
		unsigned int			nCountY = 256;				// количество точек по широте
	};							

	unsigned long long			ID = 0;						// идентификатор
	SConfig						Config;						// конфигурация

	float						fLongitudeCutCoeff = 1.f;	
	float						fLattitudeCutCoeff = 1.f;
	
	ID3D11ShaderResourceView*	pTextureSRV = nullptr;		// текстура с данными [формат текстуры - float]
};

// структура вершины
struct SVertex
{
	D3DXVECTOR3					position;
	D3DXVECTOR3					normal;
	D3DXVECTOR2					texture;
	D3DXVECTOR3					tangent;
};

// информация о координатах триангулированной области
// [можно заранее ее получить из HeightfieldConverter с помощью вызова ComputeTriangulationCoords]
struct STriangulationCoordsInfo
{
	double						vPosition[3];				// Точка начала координат триангуляции (лежит на поверхности эллипсоида)
	double						vXAxis[3];					// Координаты оси X (направлена на север вдоль поверхности эллипсоида)
	double						vYAxis[3];					// Координаты оси Y (направлена по нормали к поверхности эллипсоида)
	double						vZAxis[3];					// Координаты оси Z (направлена на восток вдоль поверхности эллипсоида)

	double						vBoundBoxMinimum[3];		// Минимум баунд-бокса
	double						vBoundBoxMaximum[3];		// Максимум баунд-бокса
};

// структура триангуляции, построенной по карте высот, содержащая буферы вершин и индексов
struct STriangulation
{
	unsigned long long			ID;							// идентификатор (совпадает с соответствующим Heightfield)

	ID3D11Buffer*				pVertexBuffer = nullptr;	// Буффер вершин
	ID3D11Buffer*				pIndexBuffer = nullptr;		// Буффер индексов

	unsigned int				nVertexCount = 0;			// количество вершин
	unsigned int				nIndexCount = 0;			// количество индексов

	unsigned int				nCountLat = 0;				// количество вершин по широте
	unsigned int				nCountLong = 0;				// количество вершин по долготе

	STriangulationCoordsInfo	Info;
};

// Колбэк-функция, сигнализирующая о завершении триангуляции
typedef void (*TriangulationTaskCompleteCallback)(void* param, STriangulation* in_pCompletedTriangulation);

// Главный класс-триангулятор карт высот
class HEIGHFIELD_CONVERTER_API HeightfieldConverter
{
public:

	HeightfieldConverter();
	~HeightfieldConverter();
	
	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcszComputeShaderFile);

	// Инициализировать через Эффект. Подразумевается, что техника носит название "HeightfieldConverter"
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, ID3DX11Effect* in_pEffect);

	// Задать глобальный коэффициент масштаба.
	// По умолчанию все расчеты ведуться в привязке к эллипсоиду Земли в системе координат WGS-84 в метрах
	void	SetWorldScale(float in_fScale);

	// получить глобальный масштаб
	float	GetWorldScale() const;

	// Задать множитель высоты, считываемых из текстур карт высот
	void	SetHeightScale(float in_fHeightScale);

	// Запросить масштабный коэффициент на высоту
	float	GetHeightScale() const;

	// Задать предельный углы при которых происходит интерполяция и разделение нормалей [по умолчанию 30 и 60 градусов]
	void	SetNormalDivisionAngles(float in_fAngleMinInDeg, float in_fAngleMaxInDeg);

	// Рассчитать координаты триангулированной области
	void	ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords);

	// Считать данные карты высот из текстуры
	// in_usCompressionRatio - степень сжатия текстуры (и соответственно результирующей триангуляции) Может принимать значения степени двойки - 1, 2, 4, 8, 16, ...
	void	ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield, unsigned short in_usCompressionRatio);

	// Считать данные карты высот из памяти
	void	ReadHeightfieldDataFromMemory(const unsigned char* in_pData, unsigned int in_nWidth, unsigned int in_nHeight, SHeightfield& out_Heightfield);

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, STriangulation* out_pTriangulation, const SHeightfield** in_ppNeighbours = nullptr);

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback in_Callback);

	// обработать поставленные задачи [можно вызвать в отдельном потоке]
	void	UpdateTasks();

	// Освободить карту высот
	void	ReleaseHeightfield(SHeightfield*);

	// Освободить буферы триангуляции
	void	ReleaseTriangulation(STriangulation*);

	// Получить буферы вершин и индексов в памяти
	bool	UnmapTriangulation(STriangulation*, SVertex* out_pVertexes, unsigned int* out_pIndices);

	// В случае, если триангуляции создаются в другом потоке, необходимо блокировать контекст во время других обращений к нему
	void	LockDeviceContext();

	void	UnlockDeviceContext();

private:

	struct HeightfieldConverterPrivate;
	HeightfieldConverterPrivate*	_private = nullptr;

	friend class DirectComputeHeightfieldConverter;
};
