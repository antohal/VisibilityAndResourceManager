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

	// ������� �����, ������ �������, ������ ���� ��������� (����� ������� ��������� ������ ������ ����)
	// ��� ���� VisibilityManager ���������� �� ������ 
	// [����������� ��������, �� ��������� = 25�]
	void SetInvisibleUnloadTime(float time);

	// ���������� ��������� �������� ����������� ������, � ������ ��������
	// ��������� ������� ����������� �������� ����������� � ������� �������� ������
	// [����������� ��������, �� ��������� = 10]
	// (��������, ��� �������� �������� 10 �/�, ��������� ����������� �� 100 �)
	void SetSpeedPotentialVisibilityMultiplier(float speedMultiplier);

	// ���������� ��������� ������� �������� �������� ������, � ������
	// �������� ��������� ������� ����������� �������� ��������������
	// � ������� �������� ������
	// [����������� ��������, �� ��������� = 5]
	// (�������� ��� ������� �������� �������� 10 ����/�, ����� ���� �������������� �� 50 ����)
	void SetRotationRatePotentialVisibilityMultiplier(float rateMultiplier);

	// �������� VisibilityManager � ���������. ���������� ����� � ���� ������ in_pVisibilityManager ��� ��������� ���������������
	// [���������� �� ����� �������������]
	void AddVisibilityManager(CVisibilityManager* in_pVisibilityManager);

	// �������� ��������� �������� 
	// [����� �������� � ��������� ������, �� ������������ � ����� �������]
	void Update(float deltaTime);

	// �������� ������� ��������� �������� ��� ���������
	float GetTexturePriority(C3DBaseTexture*);

	// �������� ��� � ���� [�������� �� ��������� TRUE]
	void EnableLeg(bool enable = true);

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
