#pragma once

#include <utility>

#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

// максимальная глубина дерева
#define MAX_TERRAIN_TREE_DEPTH	20

// индексы террейн блока на текущем уровне
struct STerrainBlockIndex
{
	unsigned char	ucLongitudeIndex = 255;
	unsigned char	ucLattitudeIndex = 255;
};

// параметры блока Земли
struct STerrainBlockParams
{
	// минимальная и максимальная долгота
	float				fMinLongitude = 0;
	float				fMaxLongitude = 0;

	// минимальная и максимальная широта
	float				fMinLattitude = 0;
	float				fMaxLattitude = 0;

	// коэффициенты обрезки
	float				fLongitudeСutCoeff = 1;
	float				fLattitudeCutCoeff = 1;

	// уровень глубины блока
	unsigned int		uiDepth = 0;

	// положение в дереве (индекс по долготе и широте по каждому уровню глубины). Для уровня глубины больше текущего будут значения по умолчанию = 255
	STerrainBlockIndex	aTreePosition[MAX_TERRAIN_TREE_DEPTH];
};

#pragma pack(push, 4)

// Структуры, считываемые из файла, описывающие лоды Земли
struct DataBaseVersionInfo
{
	unsigned char Major;
	unsigned char Minor;
};


struct DataBaseInfo : public DataBaseVersionInfo
{
	unsigned char Reserved;
	unsigned char LodCount; // кол-во лодов
	int DeltaY;				// кол-во доп пикселей по X для выравнивани¤ текстуры
	int DeltaX;				//  кол-во доп пикселей по Y для выравнивани¤ текстуры
};

struct DataBaseInfo_Ver_1_3 : public DataBaseInfo
{
	// в случае, если версия меньше 1.3, то эти значения вычисляются по DeltaY, DeltaX
	double MaxLon = 0;
	double MinLat = 0;
};

struct LodInfoStruct_Ver_1_1
{
	short Width;			// ширины тексутры
	short Height;			// высота тексутры
	short CountY;			// кол-во текстур по X
	short CountX;			// кол-во текстур по Y

	// ver 1.1

	unsigned int	TextureFormat = 0;				// 4 байта

	unsigned int	TextureScaleAlgorithm = 0;		// 4 байта
	bool			TextureIsGauss = false;				// 1 байта

	short			AltWidth = 0;
	short			AltHeight = 0;

	unsigned int	AltFormat = 0;					// 4 байта
	unsigned int	AltScaleAlgorithm = 0;			// 4 байта
	bool			AltIsGauss = 0;					// 1 байта
	bool			HasBorder = 0;					// 1 байта
};

struct LodInfoStruct_Ver_1_2
{
	short Width;			// ширины тексутры
	short Height;			// высота тексутры
	short CountY;			// кол-во текстур по X
	short CountX;			// кол-во текстур по Y

							// ver 1.1

	unsigned int	TextureFormat = 0;				// 4 байта

	unsigned int	TextureScaleAlgorithm = 0;		// 4 байта
	bool			TextureIsGauss = false;				// 1 байта

	short			AltWidth = 0;
	short			AltHeight = 0;

	unsigned int	AltFormat = 0;					// 4 байта
	unsigned int	AltScaleAlgorithm = 0;			// 4 байта
	bool			AltIsGauss = 0;					// 1 байта
	bool			HasBorder = 0;					// 1 байта

	// ver 1.2
	unsigned char	Border = 0;
	unsigned char	AltBorder = 0;
};

#pragma pack(pop)
