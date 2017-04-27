#pragma once

#include <d3d11.h>

#ifndef DDSGENERATOR_EXPORTS
#define DDSGENERATOR_API __declspec(dllimport)
#else
#define DDSGENERATOR_API __declspec(dllexport)
#endif

//************************************************************************************************
// �������:		SaveDataToDDS
// ����������:	���������� ������� ������ � ���� �������� ������� dds
// ������������ ��������:	� ������ ������ ���������� true, � ������ ������� - false
//************************************************************************************************
// ���������:
// ID3D11Device* in_pDevice					-	��������� �� ���������� D3D11
// ID3D11DeviceContext* in_pDeviceContext	-	��������� �� �������� ���������� D3D11
// void* in_pData							-	��������� �� ������ ������
// unsigned int in_nWidth					-	������ ��������
// unsigned int in_nHeight					-	������ ��������
// unsigned int in_nElementSize				-	������ �������� �������� 
// unsigned int in_nDxgiFormat				-	�������� �������, ������� �� DXGI_FORMAT
// const char* in_pcszFileName				-	��� ����� ��������
//************************************************************************************************

DDSGENERATOR_API bool SaveDataToDDS(ID3D11Device* in_pDevice, ID3D11DeviceContext* in_pDeviceContext, void* in_pData, unsigned int in_nWidth,
	unsigned int in_nHeight, unsigned int in_nElementSize, unsigned int in_nDxgiFormat, const char* in_pcszFileName);
