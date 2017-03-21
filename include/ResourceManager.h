#pragma once

#include <string>

#include "ResourceManagerLink.h"

class CVisibilityManager;
class C3DBaseObjectManager;
class C3DBaseTexture;
class CDirect2DTextBlock;

// ����� ��������� ��������.
// ������ ������� ���� ������ �������������� � ������������� ����������,
// ������� ��� ��������� ��������� �� ���� ���������� ������������ ������� GetResourceManager()

class RESOURCEMANAGER_API CResourceManager
{
public:

	CResourceManager();
	~CResourceManager();

	// ����������������, ������ MeshTree
	void Init(C3DBaseObjectManager*);

	// ���������� ���������� ���������: SetPredictionFOV(45, 45), SetInvisibleUnloadTime(0)
	void SetupDebugParameters();

	// ������� �����, ������ �������, ������ ���� ��������� (����� ������� ��������� ������ ������ ����)
	// ��� ���� VisibilityManager ���������� �� ������ 
	// [����������� ��������, �� ��������� = 10�]
	void SetInvisibleUnloadTime(float time);

	// ���������� ����� ������������ ��������� �������� ������
	// [����������� ��������, �� ��������� = 10]
	// (��������, ��� �������� �������� 10 �/�, ��������������� �������� ������������ �� 100 � � ������� �������� ������)
	void SetSpeedVisibilityPredictionTime(float predictionTime);

	//  ���������� ����� ������������ �������� �������� ������
	// [����������� ��������, �� ��������� = 10]
	// (�������� ��� ������� �������� �������� 10 ����/�, ��������������� �������� �������������� �� 100 ����)
	void SetRotationRateVisibilityPredictionTime(float rateMultiplier);

	// ���������� �������� ���������� ������� �������� ������ [�� ��������� 0.25 �]
	void SetRotationRateAverageInterval(float intervalSec);

	// ���������� ������� ��������� ��� ������������ ���������
	// �� ��������� ������� �������� �� ���������� VisibilityManager
	void SetPredictionFOV(float horizontalFovDeg, float verticalFovDeg);

	// �������� VisibilityManager � ���������. ���������� ����� � ���� ������ in_pVisibilityManager ��� ��������� ���������������
	// [���������� ���� ��� �� ����� �������������]
	void AddVisibilityManager(CVisibilityManager* in_pVisibilityManager);

	// �������� ��������� �������� 
	// [����� �������� � ��������� ������, �� ������������ � ����� �������, �� � ����� ������ � ������������ VisibilityManager]
	void Update(float deltaTime);

	// �������� ������� ��������� �������� ��� ���������
	float GetTexturePriority(C3DBaseTexture*);

	// �������� ��� � ���� [�������� �� ��������� TRUE]
	void EnableLog(bool enable = true);

	// ��������� ��� � ����
	void DisableLog();

	// �������� ��������� ���������� ���������� � ��������� ����
	void EnableDebugTextRender(CDirect2DTextBlock*);

	// ��������� ��������� ���������� ���������� � ��������� ����
	void DisableDebugTextRender();

private:

	struct SResourceManagerPrivate;
	SResourceManagerPrivate* _private;

	friend class C3DBaseResource;
	friend class C3DBaseObject;
};
