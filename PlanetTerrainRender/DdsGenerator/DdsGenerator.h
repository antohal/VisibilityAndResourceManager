#pragma once

#include <d3d11.h>

#ifndef DDSGENERATOR_EXPORTS
#define DDSGENERATOR_API __declspec(dllimport)
#else
#define DDSGENERATOR_API __declspec(dllexport)
#endif

//************************************************************************************************
// ‘ункци€:		SaveDataToDDS
// Ќазначение:	—охранение массива данных в виде текстуры формата dds
// ¬озвращаемое значение:	в случае успеха возвращает true, в случае неудачи - false
//************************************************************************************************
// ѕараметры:
// ID3D11Device* in_pDevice					-	указатель на устройство D3D11
// ID3D11DeviceContext* in_pDeviceContext	-	указатель на контекст устройства D3D11
// void* in_pData							-	указатель на массив данных
// unsigned int in_nWidth					-	ширина текстуры
// unsigned int in_nHeight					-	высота текстуры
// unsigned int in_nElementSize				-	размер элемента текстуры 
// unsigned int in_nDxgiFormat				-	значение формата, беретс€ из DXGI_FORMAT
// const char* in_pcszFileName				-	им€ файла текстуры
//************************************************************************************************

DDSGENERATOR_API bool SaveDataToDDS(ID3D11Device* in_pDevice, ID3D11DeviceContext* in_pDeviceContext, void* in_pData, unsigned int in_nWidth,
	unsigned int in_nHeight, unsigned int in_nElementSize, unsigned int in_nDxgiFormat, const char* in_pcszFileName);
