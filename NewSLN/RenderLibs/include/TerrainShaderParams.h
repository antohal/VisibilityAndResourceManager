#pragma once

struct SGlobalTerrainShaderParams
{
	//Коэффициенты разбиений
	// N - количество LOD-ов; 
	// чем больше уровень, тем выше детализация; 
	// второй индекс отвечает за ось, по которой производится разбиение;
	// partitionCoefficients[k+1] содержит коэффициенты по двум осям, во сколько раз объект уровня k+1 меньше, чем объект уровня k
	// 	(т.е., во сколько раз сетка объекта уровня k+1 чаще, чем сетка объекта уровня k)
	// partitionCoefficients[0] содержит неиспользуемую информацию о разбиении всей карты на объекты 
	// Разбиения по X должны быть делителями числа nCountX-1
	// Разбиения по Y должны быть делителями числа nCountY-1
	unsigned int	aPartitionCoefficients[20][2];

	// Количество вершин по X и Y  для каждого уровня
	unsigned int	aVertexCounts[20][2];

	//20 - это максимальное количество уровней детализации. Реальное количество уровней передается в переменной levelsCount
	unsigned int	uiLevelsCount = 0;
};

struct STerrainBlockShaderParams
{
	float			fMinLattitude = 0;				// минимальная широта
	float			fMaxLattitude = 0;				// максимальная широта

	float			fMinLongitude = 0;				// минимальная долгота
	float			fMaxLongitude = 0;				// максимальная долгота

	unsigned int	nCountX = 0;					// количество точек по долготе (может не совпадать с разрешением текстуры)
	unsigned int	nCountY = 0;					// количество точек по широте (может не совпадать с разрешением текстуры)

	float			fLongitudeCoeff = 0;			// максимальная текстурная координата по долготе
	float			fLattitudeCoeff = 0;			// максимальная текстурная координата по широте

	float			fWorldScale = 1;				// Масштаб мира
	float			fHeightScale = 1;				// Масштаб высоты

	// Коэффициенты обрезки текстурных координат для соседних блоков
	float			fNorthBlockLongCoeff = 1;
	float			fNorthBlockLatCoeff = 1;

	float			fNorthEastBlockLongCoeff = 1;
	float			fNorthEastBlockLatCoeff = 1;

	float			fEastBlockLongCoeff = 1;
	float			fEastBlockLatCoeff = 1;

	float			fSouthEastBlockLongCoeff = 1;
	float			fSouthEastBlockLatCoeff = 1;

	float			fSouthBlockLongCoeff = 1;
	float			fSouthBlockLatCoeff = 1;

	float			fSouthWestBlockLongCoeff = 1;
	float			fSouthWestBlockLatCoeff = 1;

	float			fWestBlockLongCoeff = 1;
	float			fWestBlockLatCoeff = 1;

	float			fNorthWestBlockLongCoeff = 1;
	float			fNorthWestBlockLatCoeff = 1;

	// 0	-	мин/макс долгота/широта северного соседнего блока
	float			fNorthMinLat = 0;
	float			fNorthMaxLat = 0;

	float			fNorthMinLong = 0;
	float			fNorthMaxLong = 0;

	// 1	-	мин/макс долгота/широта северо-восточного соседнего блока
	float			fNorthEastMinLat = 0;
	float			fNorthEastMaxLat = 0;

	float			fNorthEastMinLong = 0;
	float			fNorthEastMaxLong = 0;

	//2		-	мин/макс долгота/широта восточного соседнего блока

	float			fEastMinLat = 0;
	float			fEastMaxLat = 0;

	float			fEastMinLong = 0;
	float			fEastMaxLong = 0;

	//3		-	мин/макс долгота/широта юго-восточного соседнего блока
	float			fSouthEastMinLat = 0;
	float			fSouthEastMaxLat = 0;

	float			fSouthEastMinLong = 0;
	float			fSouthEastMaxLong = 0;

	//4		-	мин/макс долгота/широта южного соседнего блока
	float			fSouthMinLat = 0;
	float			fSouthMaxLat = 0;

	float			fSouthMinLong = 0;
	float			fSouthMaxLong = 0;

	//5		-	мин/макс долгота/широта юго-западного соседнего блока
	float			fSouthWestMinLat = 0;
	float			fSouthWestMaxLat = 0;

	float			fSouthWestMinLong = 0;
	float			fSouthWestMaxLong = 0;

	//6		-	мин/макс долгота/широта западного соседнего блока
	float			fWestMinLat = 0;
	float			fWestMaxLat = 0;

	float			fWestMinLong = 0;
	float			fWestMaxLong = 0;

	//7		-	мин/макс долгота/широта северо-западного соседнего блока
	float			fNorthWestMinLat = 0;
	float			fNorthWestMaxLat = 0;

	float			fNorthWestMinLong = 0;
	float			fNorthWestMaxLong = 0;


	//Уровни детализации для текущего и соседних объектов terrain-а
	//Индексация границ:
	// 0 - северная граница
	// 1 - западная граница
	// 2 - южная граница
	// 3 - восточная граница
	unsigned int	uiCurrentLOD = 0;
	unsigned int	uiAdjacentLOD[4];

	// Текстура карты высот данного блока. Формат - R32F
	ID3D11ShaderResourceView*	pHeightfield = nullptr;

	// Текстуры карты высот для соседних блоков
	ID3D11ShaderResourceView*	pNorthNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pNorthEastNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pEastNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pSouthEastNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pSouthNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pSouthWestNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pWestNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pNorthWestNeighbourHeightfield = nullptr;

	// буфер вершин для данного блока
	ID3D11Buffer*				pVertexBuffer = nullptr;
};
