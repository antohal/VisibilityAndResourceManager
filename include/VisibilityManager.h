#pragma once

#include "3DEngineInterface.h"

//#include <d3d9types.h>


#ifdef VisibilityManager_EXPORTS
#define VISIBILITYMANAGER_API __declspec(dllexport)
#else
#define VISIBILITYMANAGER_API __declspec(dllimport)
#endif

struct D3DXVECTOR3;
struct _D3DMATRIX;
typedef struct _D3DMATRIX D3DMATRIX;

//! \struct Vector3
//! \date 2012/5/22
//! \brief ���������� ��������� ��� ������������� 3� �������
struct VISIBILITYMANAGER_API Vector3
{
	float x, y, z;

	Vector3 () {}
	Vector3 (float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	Vector3 (float _a) : x(_a), y(_a), z(_a) {}
	Vector3 (const D3DXVECTOR3&);
};

//! \struct BoundBox
//! \date 2012/5/22
//! \brief ���������� ��������� ��� ������������� ��������������� ������, ������������ �� ���� ������� ������� ���������
struct VISIBILITYMANAGER_API BoundBox
{
	//! ����������� ����� ��������������� ������
	Vector3 vMin;
	//! ������������ ����� ��������������� ������
	Vector3 vMax;

	BoundBox () {}
	BoundBox (const Vector3& in_vMin, const Vector3& in_vMax) : vMin(in_vMin), vMax(in_vMax) {}
	BoundBox (const D3DXVECTOR3& in_vMin, const D3DXVECTOR3& in_vMax);
};

struct CameraDesc
{
	Vector3 vPos;
	Vector3 vDir;
	Vector3 vUp;

	float	verticalFov = 0;
	float	horizontalFov = 0;

	float	nearPlane = 0;
	float	farPlane = 0;
};

struct OrientedBox
{
	Vector3 vPos;
	Vector3 vX;
	Vector3 vY;

	BoundBox boundBox;
};

class IVisibilityManagerPrivateInterface;

// ������ ��� ��������� ���������.
// ����� �������������� ��� ���������� �������������� �������� ���������, ���������� ����������� ����� ������� ��������
// ��������� �� �������� ����������� ������� �������, ����� ���� �������� �����������
class IVisibilityManagerPlugin
{
public:

	virtual bool IsObjectVisible(C3DBaseObject* in_pObject) const = 0;
	virtual void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection) = 0;
};

class VISIBILITYMANAGER_API CVisibilityManager
{
public:

	CVisibilityManager (C3DBaseObjectManager* in_pMeshTree, float in_fWorldRadius, float in_fMinCellSize);
	~ CVisibilityManager ();

	// �������� ������ ����
	float GetWorldRadius() const;

	// �������� ����������� ������ ������
	float GetMinCellSize() const;

	// ���������� ���������� ������
	void SetResolution (unsigned int in_uiWidth, unsigned int in_uiHeight);

	// �������� ���������� ������
	void GetResolution(unsigned int& out_uiWidth, unsigned int& out_uiHeight);

	// ���������� ����������� ������ �������, ������� �� ������
	void SetMinimalObjectSize (unsigned int in_uiPixels);

	// �������� ����������� ������ �������
	unsigned int GetMinimalObjectSize() const;

	// �������� ������� ���������� ���������
	float GetNearClipPlane () const;

	// �������� ������� ���������� ���������
	float GetFarClipPlane () const;

	// ���������� ������������� ����� (���������� ������ � C3DBaseMaterial::AddVisibleFaceSet)
	void SetEye(unsigned int in_uiEye);

	void SetViewProjection(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);

	void SetCamera (const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, 
		float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane);

	void SetOrthoCamera (const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, const Vector3& in_vSizes);

	void SetObject (C3DBaseObject* in_pObject, const BoundBox& in_BBox, float in_fMaxDistance);

	void ObjectMoved (C3DBaseObject* in_pObject);

	void SetObjectAlwaysVisible (C3DBaseObject* in_pObject, bool in_bAlwaysVisible);

	void SetObjectMaxDistance (C3DBaseObject* in_Object, float in_fMaxDistance);
	void DeleteObject (C3DBaseObject* in_pObject);
	void UpdateVisibleObjectsSet ();
	void UpdateTextureVisibility();
	bool IsObjectVisible (C3DBaseObject* in_pObject) const;
	
	//const CPtrArray& GetVisibleObjects () const;
	size_t GetVisibleObjectsCount() const;
	C3DBaseObject* GetVisibleObjectPtr(size_t index) const;


	bool IsTextureVisible(C3DBaseTexture*) const;


	// GetTexturePriority: ���������� ������������� ��������, ���� ������ � ��������� ����� (��� ���� ��������, ��� ������ �� ��� ����������� ������).
	// ��� ���� ����������� ����������� ������ ������.
	// ���� ������� ���������� - 2, �� �������� ��������.
	float GetTexturePriority(C3DBaseTexture*) const;

	// �������� ������� ��������� ������
	void GetCameraParameters(CameraDesc& out_parameters) const;

	// �������� ��� � ���� [�������� �� ��������� TRUE]
	void EnableLog(bool enable = true);

	// ��������� ��� � ����
	void DisableLog();


	// ���������� ������
	void InstallPlugin(IVisibilityManagerPlugin*);

	// ������� ������
	void UninstallPlugin(IVisibilityManagerPlugin*);

protected:

	void SetPredictionModeEnabled(bool enabled);

	IVisibilityManagerPrivateInterface*	GetPrivateInterface() const;

private:
	struct VisibilityManagerPrivate;
	VisibilityManagerPrivate* _private;

	friend class CResourceManager;
};
