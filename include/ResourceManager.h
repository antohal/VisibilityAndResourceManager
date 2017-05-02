#pragma once

#include <string>

#include "ResourceManagerLink.h"

//@{ ���������������

class C3DBaseObjectManager;
class C3DBaseTexture;

// �� VisibilityManager.h
struct Vector3;
class CVisibilityManager;

// �� D2DTextRenderer
class CDirect2DTextBlock;
struct _D3DMATRIX;
typedef struct _D3DMATRIX D3DMATRIX;
//@}

// ����� ��������� ��������.
// ������ ������� ���� ������ �������������� � ������������� ����������,
// ������� ��� ��������� ��������� �� ���� ���������� ������������ ������� GetResourceManager()

class RESOURCEMANAGER_API CResourceManager
{
public:

	CResourceManager();
	~CResourceManager();

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

	// ���������� ��������� ������ �� ������� �����
	// ���� �� �������� ��� �������, �� ��������� ������ ��� ��������������� ��������� �������� ����� ������� ��
	// VisibilityManager-��, ����������� � ������� ResourceManager.
	void SetViewProjection(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);

	// ���������� ��������� ������ �� ������� ����� [������� �������������� ������������������]
	void SetCamera(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane);

	// �������� VisibilityManager � ���������. ���������� ����� � ���� ������ in_pVisibilityManager ��� ��������� ���������������
	// [���������� ���� ��� �� ����� �������������]
	void AddVisibilityManager(CVisibilityManager* in_pVisibilityManager);

	void RemoveVisibilityManager(CVisibilityManager* in_pVisibilityManager);

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
