#pragma once

#include <vector>

#include "ResourceManagerLink.h"

// predeclarations
struct D3DXMATRIX;
struct D3DXVECTOR3;

// ��� ������������� �������������� ResourceManager ������������ ���������� �������
// ������������ ���� ������ ����������� �������� �� ������� �������:
// C3DBaseObject, C3DBaseMesh, C3DBaseFaceSet, C3DBaseMaterial, C3DBaseTechnique, C3DBaseTexture.
// � ����� ���������� ���������, ����������� ������� ��������� � ������������ �� �� ������� �������:
// C3DBaseManager - ��� ���� ��������, ������� ����� ���� ���������/���������, ����� ��������
// C3DBaseObjectManager - ��� �������� (��������, ��������� ������� �������� ������������ � ������� ���������� ���������)

// ����������� ���� ��������
enum E3DResourceType
{
	C3DRESOURCE_OBJECT,
	C3DRESOURCE_MESH,
	C3DRESOURCE_MATERIAL,
	C3DRESOURCE_FACESET,
	C3DRESOURCE_TECHNIQUE,
	C3DRESOURCE_TEXTURE
};

class C3DBaseManager;

// ������� ������
class RESOURCEMANAGER_API C3DBaseResource
{
public:

	// �������� ��� �������
	virtual E3DResourceType	GetType() const = 0;

	// �������� ���������� ������ ������� ������� �� ������ ������
	virtual int GetVisibleRefCount() const { return _visibleRefCount; }

	// �������� �������� �������
	virtual void GetChildResources(std::vector<C3DBaseResource*>& out_vecChildResources) const = 0;

	// �������� ��������� �� ��������, ����������� ������ ��������
	// ����� ���� NULL. � ����� ������, ������ �� �������� �����������
	virtual C3DBaseManager*	GetManager() const = 0;

private:

	int	_visibleRefCount = 0;
	friend class CResourceManager;
};

// ������� �������� ����������� ��������
class C3DBaseManager
{
public:

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource*) = 0;

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource*) = 0;
};

class C3DBaseMesh;

// ������� ����� ������� � �����
class RESOURCEMANAGER_API C3DBaseObject : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_OBJECT; }

	// ��� 3D ������� ������ ����� ���������� �����-����. ������, ���� ������ - �����, � �� ���, ��
	// ����� ������ ���������� �������� � out_vBBMin � out_vBBMax.
	virtual void GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) = 0;

	// �������� ������� �������������
	virtual D3DXMATRIX* GetWorldTransform() = 0;

	// ������� ������ ����������: ��������-�� �������� ������� ������� �� ������
	virtual bool IsMinimalSizeCheckEnabled() const = 0;

	// ������� ���������� ����� ����� ������� �������
	virtual void	GetMeshes(std::vector<C3DBaseMesh*>& out_vecMeshes) const = 0;

protected:

	// ��� ������� ����������, ���������� ������ VisibilityManager � ������, ����� ������ ���������� �������.
	// ���������� ��������� �������� ��������� ���� �������� ��������
	// ������, ������������ ����� �������������� � � �������� ������, ���� ����� ������������ ������ �������.
	// ��� ���� ����������� ������� C3DBaseResource::SetVisible � ������ ���������������� �������.
	virtual void SetVisible();

	friend class CVisibilityManager;
};

// ������� ����� ��������� ��������
class C3DBaseObjectManager : public C3DBaseManager
{
public:

	// �������� ������ �������� �� ��������� �����-�����
	virtual void GetObjectList(const D3DXVECTOR3& bboxMin, const D3DXVECTOR3& bboxMax, std::vector<C3DBaseObject*>& out_vecObjects) = 0;
};

class C3DBaseFaceSet;

// ������� ����� ����
class C3DBaseMesh : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MESH; }

	// �������� ������ ����-�����, ������������ � ����
	virtual void	GetFaceSets(std::vector<C3DBaseFaceSet*>& out_vecFaceSets) const = 0;
};

class C3DBaseMaterial;

// ������� ����� ����-����
class C3DBaseFaceSet : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_FACESET; }

	// �������� ������ �� ��������
	virtual C3DBaseMaterial* GetMaterialRef() = 0;
};

class C3DBaseTechnique;
class C3DBaseTexture;

// ������� ��������
class C3DBaseMaterial : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_MATERIAL; }

	// ������� ������������ � �������� ���������� [��� �������������� �������� ������� � Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*) = 0;

	// �������� ����� ������
	virtual void	GetTechniques(std::vector<C3DBaseTechnique*>& out_vecTechniques) const = 0;

	// �������� ������ �������
	virtual void	GetTextures(std::vector<C3DBaseTexture*>& out_vecTextures) const = 0;
};

// ������� �������
class C3DBaseTechnique : public C3DBaseResource
{
public:
	
	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TECHNIQUE; }

};

enum ETextureType
{
	TEXTURE_2D = 0,
	TEXTURE_TYPE_CUBE,
	TEXTURE_TYPE_VOLUME,
	TEXTURE_TYPE_FX,

	TEXTURE_TYPE_COUNT
};


class C3DBaseTexture : public C3DBaseResource
{
public:

	// ��� ������� ����� ��������.
	virtual E3DResourceType	GetType() const { return C3DRESOURCE_TEXTURE; }

	// ������� ��� ��������
	virtual ETextureType	GetTextureType() const = 0;
};
