#include "stdafx.h"
#include "VisibilityManager.h"

#include "GridManager.h"
#include "Geometry/InFrustum.h"
#include "Camera.h"

#define INITGUID
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
//#include <ddraw.h>
//#include <dxgi.h>

#define TEXTURE_VISIBILITY

using namespace VisibilityManager;

//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#ifdef USE_OPENCL
auto_ptr<COpenCLTaskManager>	g_ptrTaskManager;
#endif


Vector3::Vector3 (const ::D3DXVECTOR3& in_v)
	: x(in_v.x), y(in_v.y), z(in_v.z)
{
}

BoundBox::BoundBox (const ::D3DXVECTOR3& in_vMin, const ::D3DXVECTOR3& in_vMax)
	: vMin(in_vMin), vMax(in_vMax)
{
}

Vector3f ToVec3(const Vector3& v)
{
	return Vector3f(v.x, v.y, v.z);
}

Vector3 FromVec3(const Vector3f& v)
{
	return Vector3(v.x, v.y, v.z);
}

CBoundBox<float> ToBoundBox (const BoundBox& bbox)
{
	return CBoundBox<float>(ToVec3(bbox.vMin), ToVec3(bbox.vMax));
}

struct CVisibilityManager::VisibilityManagerPrivate
{

	CVisibilityManager::VisibilityManagerPrivate ()
		: _pBase3DObjectTree(NULL)
		, _uiScreenWidth(1920)
		, _uiScreenHeight(1080)
		, _uiMinimalObjectSize(7)
		, _fMinimalObjectPixelArea(100)
		, _fMinimalObjectPixelSize(7)
		, _fNearClipPlane(0)
		, _fFarClipPlane(0)
		, _bUpdateTextureVisibility(true)
	{
	}

	auto_ptr<CEngineGrid>	_ptrOkTree;

	struct SObject
	{
		SObject () : _fMaxDistSq(0) {}
		SObject (const CBoundBox<float>& in_bbox, float in_fMaxDist)
			: _bbox(in_bbox), _fMaxDistSq(in_fMaxDist*in_fMaxDist)
		{}

		void UpdateData (const CBoundBox<float>& in_bbox, float in_fMaxDist)
		{
			_bbox = in_bbox;

			if (in_fMaxDist > 0)
				_fMaxDistSq = in_fMaxDist*in_fMaxDist;
		}

		void Init(C3DBaseObject* in_pObject)
		{
			_pObject = in_pObject;
			if (!in_pObject)
				return;

			_bMinimalSizeCheckEnabled = in_pObject->IsMinimalSizeCheckEnabled();

			std::vector<C3DBaseMesh*> vecMeshes;
			in_pObject->GetMeshes(vecMeshes);

			for (C3DBaseMesh* mesh : vecMeshes)
			{
				vector<C3DBaseFaceSet*> vecFaceSets;
				mesh->GetFaceSets(vecFaceSets);

				for (C3DBaseFaceSet* faceset : vecFaceSets)
					_vecFaceSets.push_back(faceset);
			}

			D3DXVECTOR3 *pvMin = nullptr, *pvMax = nullptr;
			in_pObject->GetBoundBox(&pvMin, &pvMax);

			if (pvMin && pvMax)
			{
				_vOriginalBBoxSize.x = pvMax->x - pvMin->x;
				_vOriginalBBoxSize.y = pvMax->y - pvMin->y;
				_vOriginalBBoxSize.z = pvMax->z - pvMin->z;
			}

			UpdateTransform(in_pObject);
		}

		void UpdateTransform(C3DBaseObject* in_pObject)
		{
			if (D3DXMATRIX* mpTransform = in_pObject->GetWorldTransform())
			{
				_mInvTransform.m[0][0] = mpTransform->m[0][0]; _mInvTransform.m[0][1] = mpTransform->m[1][0]; _mInvTransform.m[0][2] = mpTransform->m[2][0];
				_mInvTransform.m[1][0] = mpTransform->m[0][1]; _mInvTransform.m[1][1] = mpTransform->m[1][1]; _mInvTransform.m[1][2] = mpTransform->m[2][1];
				_mInvTransform.m[2][0] = mpTransform->m[0][2]; _mInvTransform.m[2][1] = mpTransform->m[1][2]; _mInvTransform.m[2][2] = mpTransform->m[2][2];
				_mInvTransform.Orthonormalize();
			}
			else
			{
				_mInvTransform.m[0][0] = 1; _mInvTransform.m[0][1] = 0; _mInvTransform.m[0][2] = 0;
				_mInvTransform.m[1][0] = 0; _mInvTransform.m[1][1] = 1; _mInvTransform.m[1][2] = 0;
				_mInvTransform.m[2][0] = 0; _mInvTransform.m[2][1] = 0; _mInvTransform.m[2][2] = 1;
			}
		}

		void InitTextures()
		{
			_bTexturesInited = true;

			if (!_pObject)
				return;

			if (_vecFaceSets.empty())
				return;

			for (C3DBaseFaceSet* faceset : _vecFaceSets)
			{
				if (C3DBaseMaterial* pMaterial = faceset->GetMaterialRef())
				{
					std::vector<C3DBaseTexture*> vecTextures;
					pMaterial->GetTextures(vecTextures);

					for (C3DBaseTexture* texture : vecTextures)
					{
						std::vector<C3DBaseTexture*>& vecTexturesByType = _vecTextures[texture->GetTextureType()];

						if (std::find(vecTexturesByType.begin(), vecTexturesByType.end(), texture) == vecTexturesByType.end())
							vecTexturesByType.push_back(texture);
					}
				}
			}

		}

		float	GetBoundBoxAreaInPixels(const CCamera& in_Camera, const Vector2f& in_vResolution) const;
		float	GetBoundBoxMidSizeInPixels(const CCamera& in_Camera, const Vector2f& in_vResolution) const;
		float	GetCameraDirectionDotProduct(const CCamera& in_Camera, const Vector2f& in_vResolution) const;

		C3DBaseObject*			_pObject = nullptr;

		bool					_bTexturesInited = false;
		bool					_bAlwaysVisible = false;

		CBoundBox<float>		_bbox;
		float					_fMaxDistSq;
		Vector3f				_vOriginalBBoxSize = Vector3f(0, 0, 0);

		bool					_bMinimalSizeCheckEnabled;
		std::vector<C3DBaseTexture*>	_vecTextures[TEXTURE_TYPE_COUNT];
		std::vector<C3DBaseFaceSet*>	_vecFaceSets;

		Matrix3x3f				_mInvTransform;
		bool					_bRegisteredInGrid = false;
	};

	CCamera									_Camera;

	std::set<C3DBaseObject*>				_setAlwaysVisibleObjects;
	std::set<C3DBaseTexture*>				_asetVisibleTextures[TEXTURE_TYPE_COUNT];
	std::map<C3DBaseTexture*, float>		_amapTexturePriority[TEXTURE_TYPE_COUNT];
	std::vector<C3DBaseObject*>				_vecVisibleObjects;
	
#ifndef USE_OPENCL	
	std::map<C3DBaseObject*, SObject>		_mapObjects;
#endif


	C3DBaseObjectManager*					_pBase3DObjectTree;

	unsigned int							_uiScreenWidth;
	unsigned int							_uiScreenHeight;
	unsigned int							_uiMinimalObjectSize;

	float									_fMinimalObjectPixelArea;
	float									_fMinimalObjectPixelSize;

	float									_fNearClipPlane;
	float									_fFarClipPlane;

	bool									_bUpdateTextureVisibility;
	
	struct SObjectAddRequest
	{
		C3DBaseObject*	_Object;
		BoundBox			_BBox; 
		float				_fMaxDistance;
	};

	std::vector<SObjectAddRequest>			_vecObjectAddRequests;

	//float	GetBoundBoxMaxSizeInPixels (const CBoundBox<float>& in_bbox) const;
	//float	GetBoundBoxAreaInPixels(const CBoundBox<float>& in_bbox) const;
	void	SetObjectVisibleOnThisFrame(CVisibilityManager::VisibilityManagerPrivate::SObject&);
	bool	GetTransformedBoundBox (C3DBaseObject* in_pObj, CBoundBox<float>& out_BBox);

	void	SetObjectInternal(C3DBaseObject* in_Object, const BoundBox& in_BBox, float in_fMaxDistance);
};

bool CVisibilityManager::VisibilityManagerPrivate::GetTransformedBoundBox (C3DBaseObject* in_pObj, CBoundBox<float>& out_BBox)
{
	D3DXVECTOR3 *pvMin, *pvMax;
	in_pObj->GetBoundBox(&pvMin, &pvMax);

	if (! pvMin || ! pvMax)
	{
		//assert_debug(false, L"Illegal BoundBox pointers to vMin and vMax D3DXVECTORS!");
		_setAlwaysVisibleObjects.insert(in_pObj);
		CVisibilityManager::VisibilityManagerPrivate::SObject& internalObject = _mapObjects[in_pObj];
		internalObject._bAlwaysVisible = true;

		return false;
	}

	out_BBox.m_vMin = Vector3f(pvMin->x, pvMin->y, pvMin->z);
	out_BBox.m_vMax = Vector3f(pvMax->x, pvMax->y, pvMax->z);


	/*D3DXMATRIX mTest (1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0,
					  0, 0, 0, 1);

	D3DXVECTOR3 vTest (0, 0, 1), vRes;

	D3DXVec3TransformCoord(&vRes, &vTest, &mTest);*/



	// TODO: Enable transform
	D3DXMATRIX* pTransform = in_pObj->GetWorldTransform();
	if (! pTransform)
	{
		//assert_debug(false, L"GetWorldTransform() returned NULL");
		_setAlwaysVisibleObjects.insert(in_pObj);
		CVisibilityManager::VisibilityManagerPrivate::SObject& internalObject = _mapObjects[in_pObj];
		internalObject._bAlwaysVisible = true;

		return false;
	}

	D3DXMATRIX mTransform(*pTransform);

	CBoundBox<float> InitialBBox(Vector3f(pvMin->x, pvMin->y, pvMin->z), Vector3f(pvMax->x, pvMax->y, pvMax->z));

	Vector3f vVertices[8];
	InitialBBox.ComputeVertices(vVertices);

	out_BBox.Init();

	
	for (int i = 0; i < 8; i++)
	{
		D3DXVECTOR3 vInput(vVertices[i].x, vVertices[i].y, vVertices[i].z);
		D3DXVECTOR3 vTransformed = vInput;

		vTransformed.x = vInput.x*mTransform.m[0][0] + vInput.y*mTransform.m[1][0] + vInput.z*mTransform.m[2][0] + 1*mTransform.m[3][0];
		vTransformed.y = vInput.x*mTransform.m[0][1] + vInput.y*mTransform.m[1][1] + vInput.z*mTransform.m[2][1] + 1*mTransform.m[3][1];
		vTransformed.z = vInput.x*mTransform.m[0][2] + vInput.y*mTransform.m[1][2] + vInput.z*mTransform.m[2][2] + 1*mTransform.m[3][2];
		float w = vInput.x*mTransform.m[0][3] + vInput.y*mTransform.m[1][3] + vInput.z*mTransform.m[2][3] + 1*mTransform.m[3][3];

		vTransformed.x /= w;
		vTransformed.y /= w;
		vTransformed.z /= w;

		out_BBox.Update(Vector3f(vTransformed.x, vTransformed.y, vTransformed.z));
	}

	if (!out_BBox.IsValid())
		return false;

	return true;
}

CVisibilityManager::CVisibilityManager (C3DBaseObjectManager* in_pMeshTree, float in_fWorldRadius, float in_fMinCellSize)
{
#ifdef USE_OPENCL
	if (! g_ptrTaskManager.get())
		g_ptrTaskManager = auto_ptr<COpenCLTaskManager>(new COpenCLTaskManager());
#endif

	_private = new VisibilityManagerPrivate;

	if (in_fMinCellSize < 10000)
		in_fMinCellSize = 10000;

	CBoundBox<float> WorldBBox(Vector3f(0), Vector3f(0));

	D3DXVECTOR3 vMinWorld(-in_fWorldRadius, -in_fWorldRadius, -in_fWorldRadius);
	D3DXVECTOR3 vMaxWorld(in_fWorldRadius, in_fWorldRadius, in_fWorldRadius);

	std::vector<C3DBaseObject*> vecObjects;
	in_pMeshTree->GetObjectList(vMinWorld, vMaxWorld, vecObjects);

	//calculate world radius
	for (C3DBaseObject* object : vecObjects)
	{
		CBoundBox<float> BBox;
		if (_private->GetTransformedBoundBox(object, BBox))
			WorldBBox.UpdateByBoundBox(BBox);
	}

	float fWorldRadius = in_fWorldRadius;

	float fRadiusX = MAX(fabsf(WorldBBox.vMax.x), fabsf(WorldBBox.vMin.x));
	float fRadiusY = MAX(fabsf(WorldBBox.vMax.y), fabsf(WorldBBox.vMin.y));
	float fRadiusZ = MAX(fabsf(WorldBBox.vMax.z), fabsf(WorldBBox.vMin.z));

	float fRadius = sqrtf(fRadiusX*fRadiusX + fRadiusY*fRadiusY + fRadiusZ*fRadiusZ);
	if (fRadius > fWorldRadius)
		fWorldRadius = fRadius*1.5f;

	float D = 2 * fWorldRadius;
	BYTE btMaxDepth = 0;

	while (D >= in_fMinCellSize)
	{
		btMaxDepth ++;
		D = D/2;
	}

	_private->_ptrOkTree = auto_ptr<CEngineGrid>(new CEngineGrid(fWorldRadius, btMaxDepth));
	_private->_pBase3DObjectTree = in_pMeshTree;

	SetCamera (Vector3(0), Vector3(1,0,0), Vector3(0,1,0), 75, 75, 1.0, 100.0);

	for (C3DBaseObject* object : vecObjects)
	{
		CBoundBox<float> BBox;
		if (_private->GetTransformedBoundBox(object, BBox))
			SetObject(object, BoundBox(Vector3(BBox.m_vMin.x, BBox.m_vMin.y, BBox.m_vMin.z), Vector3(BBox.m_vMax.x, BBox.m_vMax.y, BBox.m_vMax.z)), 0);
	}

	//UpdateVisibleObjectsSet();
}
float	CVisibilityManager::VisibilityManagerPrivate::SObject::GetCameraDirectionDotProduct(const CCamera& in_Camera, const Vector2f& in_vResolution) const
{
	Vector3f vCenter = _bbox.GetCenter();
	Vector3f vCameraCenter = in_Camera.GetPos();
	Vector3f vDirection = in_Camera.GetDir();
	Vector3f vUp = in_Camera.GetUp();
	Vector3f vLeft = cross(vUp, vDirection);

	Vector3f vDirToObject = Normalize(vCenter - vCameraCenter);

	float fUpProj = dot(vUp, vDirToObject);
	float fLeftProj = dot(vLeft, vDirToObject);
	float fRatio = in_vResolution.x / in_vResolution.y;

	fUpProj *= fRatio;

	float fResult = sqrtf(fUpProj*fUpProj + fLeftProj*fLeftProj);

	return fResult;
}

float	CVisibilityManager::VisibilityManagerPrivate::SObject::GetBoundBoxMidSizeInPixels(const CCamera& in_Camera, const Vector2f& in_vResolution) const
{
	//float fBboxDiameter = _bbox.GetMidSize();

	//Vector3f vDirToObject = _bbox.GetCenter() - in_Camera.GetPos();
	//vDirToObject.Normalize();

	Vector3f vSize = _bbox.GetSize();

	//double a = sqrtf(vSize.x*vSize.x - vSize.x*vDirToObject.x*vSize.x*vDirToObject.x);
	//double b = sqrtf(vSize.y*vSize.y - vSize.y*vDirToObject.y*vSize.y*vDirToObject.y);
	//double c = sqrtf(vSize.z*vSize.z - vSize.z*vDirToObject.z*vSize.z*vDirToObject.z);

	double fBboxDiameter = (vSize.x + vSize.y + vSize.z) / 3;//(a > b && a < c) || (a > c && a < b) ? a : ((b > a && b < c) || (b > c && b < a) ? b : c);
	
	if (in_Camera.IsOrtho())
	{
		double fWidthPixelsPerMeter = in_vResolution.x / in_Camera.GetBoundBox().GetSize().x;
		double fHeightPixelsPerMeter = in_vResolution.y / in_Camera.GetBoundBox().GetSize().y;
	
		double fMaxAspect = max(fWidthPixelsPerMeter, fHeightPixelsPerMeter);
		double fMaxObjectPixels = fBboxDiameter * fMaxAspect;
	
		return (float)fMaxObjectPixels;
	}
	
	Vector3f vObjCenter = _bbox.GetCenter();
	double fDist = Length(vObjCenter - in_Camera.GetPos());
	
	if (IsEqualT<double>(fDist, 0))
		return 10000;
	
	double fMetersOnNearPlane = fBboxDiameter / fDist;
	double fWidthMeters = 2 * in_Camera.GetHorizontalHalfFovTan();
	double fHeightMeters = 2 * in_Camera.GetVerticalHalfFovTan();
	
	double fWidthPixelsPerMeter = in_vResolution.x / fWidthMeters;
	double fHeightPixelsPerMeter = in_vResolution.y / fHeightMeters;
	
	double fMaxAspect = max(fWidthPixelsPerMeter, fHeightPixelsPerMeter);
	double fMaxObjectPixels = fMetersOnNearPlane * fMaxAspect;
	
	return static_cast<float>(fMaxObjectPixels);
}

float	CVisibilityManager::VisibilityManagerPrivate::SObject::GetBoundBoxAreaInPixels(const CCamera& in_Camera, const Vector2f& in_vResolution) const
{
	Vector3f vBBoxSize = _bbox.GetSize();//_vOriginalBBoxSize;
	
	if (in_Camera.IsOrtho())
	{
		float fWidthPixelsPerMeter = in_vResolution.x / in_Camera.GetBoundBox().GetSize().x;
		float fHeightPixelsPerMeter = in_vResolution.y / in_Camera.GetBoundBox().GetSize().y;
	
		float fMaxAspect = max(fWidthPixelsPerMeter, fHeightPixelsPerMeter);
		float fMaxObjectPixels = max(vBBoxSize.x, vBBoxSize.y) * max(vBBoxSize.y, vBBoxSize.z) * fMaxAspect;
	
		return fMaxObjectPixels;
	}
	
	Vector3f vCameraDir = _bbox.GetCenter() - in_Camera.GetPos();
	
	/*D3DXVECTOR3 vInputDir(vCameraDir.x, vCameraDir.y, vCameraDir.z);
	D3DXVECTOR3 vTransformedView;
	
	vTransformedView.x = vInputDir.x*_mInvTransform.m[0][0] + vInputDir.y*_mInvTransform.m[1][0] + vInputDir.z*_mInvTransform.m[2][0];
	vTransformedView.y = vInputDir.x*_mInvTransform.m[0][1] + vInputDir.y*_mInvTransform.m[1][1] + vInputDir.z*_mInvTransform.m[2][1];
	vTransformedView.z = vInputDir.x*_mInvTransform.m[0][2] + vInputDir.y*_mInvTransform.m[1][2] + vInputDir.z*_mInvTransform.m[2][2];
	*/
	
	float fDist = vCameraDir.NormalizeL();
	
	
	float fDotX = fabsf(vCameraDir.x);
	float fDotY = fabsf(vCameraDir.y);
	float fDotZ = fabsf(vCameraDir.z);
	
	float fAreaMeters = fDotX*vBBoxSize.y*vBBoxSize.z + fDotY*vBBoxSize.x*vBBoxSize.z + fDotZ*vBBoxSize.x*vBBoxSize.y;
	
	
	if (IsEqualT<float>(fDist, 0))
		return 1000.f;
	
	float fDistCoeff = 1 / fDist;
	
	float fWidthMeters = 2 * in_Camera.GetHorizontalHalfFovTan();
	float fHeightMeters = 2 * in_Camera.GetVerticalHalfFovTan();
	
	float fWidthPixelsPerMeter = in_vResolution.x / fWidthMeters;
	float fHeightPixelsPerMeter = in_vResolution.y / fHeightMeters;
	
	float fMaxAspect = max(fWidthPixelsPerMeter, fHeightPixelsPerMeter);
	float fMaxObjectPixels = fAreaMeters * fMaxAspect * fDistCoeff ;
	
	return fMaxObjectPixels;
}

void CVisibilityManager::SetResolution (unsigned int in_uiWidth, unsigned int in_uiHeight)
{
	_private->_uiScreenWidth = in_uiWidth;
	_private->_uiScreenHeight = in_uiHeight;
}

void CVisibilityManager::SetMinimalObjectSize (unsigned int in_uiPixels)
{
	_private->_uiMinimalObjectSize = in_uiPixels;
	_private->_fMinimalObjectPixelArea = (float)(in_uiPixels*in_uiPixels);
	_private->_fMinimalObjectPixelSize = (float)in_uiPixels;
}


CVisibilityManager::~ CVisibilityManager ()
{
	delete _private;
}

void CVisibilityManager::SetViewProjection(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	if (this == NULL || in_pmProjection == NULL)
		return;

	Matrix4x4<float> mView, mProj;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			mProj[i][j] = in_pmProjection->m[i][j];
		}

	D3DXMATRIX md3dView;

	D3DXVECTOR3 eye(in_vPos.x, in_vPos.y, in_vPos.z);
	D3DXVECTOR3 at(in_vPos.x + in_vDir.x, in_vPos.y + in_vDir.y, in_vPos.z + in_vDir.z);
	D3DXVECTOR3 up(in_vUp.x, in_vUp.y, in_vUp.z);

	D3DXMatrixLookAtLH(&md3dView, &eye, &at, &up);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			mView[i][j] = md3dView.m[i][j];
		}

	Vector3f vPos = ToVec3(in_vPos);
	Vector3f vDir = ToVec3(in_vDir);
	Vector3f vUp = ToVec3(in_vUp);


	D3DXMATRIX md3dProj = *in_pmProjection;
	D3DXMATRIX mInvProjection;

	D3DXVECTOR3 vCubeEdges[8];
	D3DXVECTOR3 vViewSpaceCubeEdges[8];
	Vector3f vViewSpaceFrustumPoints[8];

	FLOAT fDet = 0;

	float fNearPlane = 99999.f;
	float fFarPlane = 0;

	float fHorizontalFOVDeg = 75;
	float fVerticalFOVDeg = 75;

	if (D3DXMatrixInverse(&mInvProjection, &fDet, &md3dProj) != NULL)
	{

		vCubeEdges[0] = D3DXVECTOR3(1, 1, 0);
		vCubeEdges[1] = D3DXVECTOR3(1, -1, 0);
		vCubeEdges[2] = D3DXVECTOR3(-1, -1, 0);
		vCubeEdges[3] = D3DXVECTOR3(-1, 1, 0);

		vCubeEdges[4] = D3DXVECTOR3(1, 1, 1);
		vCubeEdges[5] = D3DXVECTOR3(1, -1, 1);
		vCubeEdges[6] = D3DXVECTOR3(-1, -1, 1);
		vCubeEdges[7] = D3DXVECTOR3(-1, 1, 1);

		for (int i = 0; i < 8; i++)
		{
			D3DXVec3TransformCoord(&vViewSpaceCubeEdges[i], &vCubeEdges[i], &mInvProjection);

			if (fFarPlane < vViewSpaceCubeEdges[i].z)
				fFarPlane = vViewSpaceCubeEdges[i].z;

			if (fNearPlane > vViewSpaceCubeEdges[i].z)
				fNearPlane = vViewSpaceCubeEdges[i].z;

			vViewSpaceFrustumPoints[i] = Vector3f(vViewSpaceCubeEdges[i].x, vViewSpaceCubeEdges[i].y, vViewSpaceCubeEdges[i].z);
		}


		D3DXVECTOR3 vXZBase1 = vViewSpaceCubeEdges[4] - vViewSpaceCubeEdges[0];
		D3DXVECTOR3 vXZBase2 = vViewSpaceCubeEdges[7] - vViewSpaceCubeEdges[3];

		Vector2f vHorizontalBase1(vXZBase1.x, vXZBase1.z);
		Vector2f vHorizontalBase2(vXZBase2.x, vXZBase2.z);

		float fCosHor = DotProduct(Normalize(vHorizontalBase1), Normalize(vHorizontalBase2));
		fHorizontalFOVDeg = acosf(fCosHor)*R2D;


		D3DXVECTOR3 vYZBase1 = vViewSpaceCubeEdges[4] - vViewSpaceCubeEdges[0];
		D3DXVECTOR3 vYZBase2 = vViewSpaceCubeEdges[5] - vViewSpaceCubeEdges[1];

		Vector2f vVerticalBase1(vYZBase1.y, vYZBase1.z);
		Vector2f vVerticalBase2(vYZBase2.y, vYZBase2.z);

		float fCosVer = DotProduct(Normalize(vVerticalBase1), Normalize(vVerticalBase2));
		fVerticalFOVDeg = acosf(fCosVer)*R2D;

		if (vViewSpaceFrustumPoints[0].z > vViewSpaceFrustumPoints[4].z)
		{
			std::swap(vViewSpaceFrustumPoints[0], vViewSpaceFrustumPoints[4]);
			std::swap(vViewSpaceFrustumPoints[1], vViewSpaceFrustumPoints[5]);
			std::swap(vViewSpaceFrustumPoints[2], vViewSpaceFrustumPoints[6]);
			std::swap(vViewSpaceFrustumPoints[3], vViewSpaceFrustumPoints[7]);
		}

		static bool bLog = false;

		if (!bLog)
		{
			if (FILE * fpLog = fopen("visman.log", "w"))
			{
				for (int i = 0; i < 8; i++)
					fprintf(fpLog, "%f, %f, %f\n", vViewSpaceFrustumPoints[i].x, vViewSpaceFrustumPoints[i].y, vViewSpaceFrustumPoints[i].z);

				fclose(fpLog);
			}
			bLog = true;
		}

		_private->_Camera.CreateFromFrustumPoints(vPos, vDir, vUp, fHorizontalFOVDeg, fVerticalFOVDeg, fNearPlane, fFarPlane, vViewSpaceFrustumPoints);

	}
	else
	{
		_private->_Camera.SetPerspective(vPos, vDir, vUp, 75, 75, 1, 1000);
	}


	_private->_vecVisibleObjects.resize(0);

	for (int i = 0; i < TEXTURE_TYPE_COUNT; i++)
	{
		_private->_asetVisibleTextures[i].clear();
		_private->_amapTexturePriority[i].clear();
	}
}

void CVisibilityManager::SetCamera (const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, 
	float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane)
{
	if (this == NULL)
		return;

	Vector3f vPos = ToVec3(in_vPos);
	Vector3f vDir = ToVec3(in_vDir);
	Vector3f vUp = ToVec3(in_vUp);


	//float fCtgX = cosf(in_fHorizontalFOV*D2R / 2) / sinf(in_fHorizontalFOV*D2R / 2);
	//float fCtgY = cosf(in_fVerticalFOV*D2R / 2) / sinf(in_fVerticalFOV*D2R / 2);
	//float fAspect = fCtgY / fCtgX;

	//D3DXMATRIX mProj;
	//D3DXMatrixPerspectiveFovLH(&mProj, in_fVerticalFOV*D2R, fAspect, in_fNearPlane, in_fFarPlane);

	//SetViewProjection(in_vPos, in_vDir, in_vUp, &mProj);

	_private->_Camera.SetPerspective(vPos, vDir, vUp, in_fHorizontalFOV, in_fVerticalFOV, in_fNearPlane, in_fFarPlane);


	_private->_vecVisibleObjects.resize(0);
	
	for (int i = 0; i < TEXTURE_TYPE_COUNT; i++)
	{
		_private->_asetVisibleTextures[i].clear();
		_private->_amapTexturePriority[i].clear();
	}
}

void CVisibilityManager::SetOrthoCamera (const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, const Vector3& in_vSizes)
{
	_private->_Camera.SetOrtho(ToVec3(in_vPos), ToVec3(in_vDir), ToVec3(in_vUp), ToVec3(in_vSizes));

	_private->_vecVisibleObjects.resize(0);

	for (int i = 0; i < TEXTURE_TYPE_COUNT; i++)
	{
		_private->_asetVisibleTextures[i].clear();
		_private->_amapTexturePriority[i].clear();
	}
}

void CVisibilityManager::VisibilityManagerPrivate::SetObjectInternal(C3DBaseObject* in_Object, const BoundBox& in_BBox, float in_fMaxDistance)
{
#ifndef USE_OPENCL

	CBoundBox<float> bb = ToBoundBox(in_BBox);

	CVisibilityManager::VisibilityManagerPrivate::SObject& internalObject = _mapObjects[in_Object];
	if (!internalObject._bRegisteredInGrid)
	{
		internalObject._fMaxDistSq = in_fMaxDistance*in_fMaxDistance;
		internalObject._bbox = bb;

		_ptrOkTree->RegisterObject(reinterpret_cast<UINT_PTR>(&internalObject), bb);

		internalObject.Init(in_Object);

		internalObject._bRegisteredInGrid = true;
	}
	else
	{
		CBoundBox<float> oldBB = internalObject._bbox;

		if (oldBB.IsDifferent(bb))
			_ptrOkTree->MoveObject(reinterpret_cast<UINT_PTR>(&internalObject), internalObject._bbox, bb);

		internalObject.UpdateData(bb, in_fMaxDistance);
		internalObject.UpdateTransform(in_Object);
	}

#else
	g_ptrTaskManager->UpdateObjectInfo(in_Object);
#endif
}

void CVisibilityManager::SetObject (C3DBaseObject* in_Object, const BoundBox& in_BBox, float in_fMaxDistance)
{
	VisibilityManagerPrivate::SObjectAddRequest addRequest;

	addRequest._BBox = in_BBox;
	addRequest._Object = in_Object;
	addRequest._fMaxDistance = in_fMaxDistance;

	_private->_vecObjectAddRequests.push_back(addRequest);
}

void CVisibilityManager::ObjectMoved (C3DBaseObject* in_pObject)
{
	CBoundBox<float> BBTransformed;
	if (_private->GetTransformedBoundBox(in_pObject, BBTransformed))
	{
		BoundBox bbox(Vector3(BBTransformed.m_vMin.x, BBTransformed.m_vMin.y, BBTransformed.m_vMin.z),
			Vector3(BBTransformed.m_vMax.x, BBTransformed.m_vMax.y, BBTransformed.m_vMax.z));

		this->SetObject(in_pObject, bbox, -1);
	}
}

void CVisibilityManager::SetObjectAlwaysVisible (C3DBaseObject* in_pObject, bool in_bAlwaysVisible)
{
	if (in_bAlwaysVisible)
		_private->_setAlwaysVisibleObjects.insert(in_pObject);
	else
		_private->_setAlwaysVisibleObjects.erase(in_pObject);

	CVisibilityManager::VisibilityManagerPrivate::SObject& internalObject = _private->_mapObjects[in_pObject];
	internalObject._bAlwaysVisible = in_bAlwaysVisible;

}

void CVisibilityManager::SetObjectMaxDistance (C3DBaseObject* in_ObjectId, float in_fMaxDistance)
{
#ifndef USE_OPENCL
	auto itObj = _private->_mapObjects.find(in_ObjectId);
	assert_debug(itObj != _private->_mapObjects.end(), L"visman::SetObjectMaxDistance () : This object is unregistered!");

	if (itObj == _private->_mapObjects.end())
		return;

	itObj->second._fMaxDistSq = in_fMaxDistance*in_fMaxDistance;
#endif
}

void CVisibilityManager::DeleteObject (C3DBaseObject* in_ObjectId)
{
#ifndef USE_OPENCL
	auto itObj = _private->_mapObjects.find(in_ObjectId);
	assert_debug(itObj != _private->_mapObjects.end(), L"visman::DeleteObject () : This object is unregistered!");

	if (itObj == _private->_mapObjects.end())
		return;

	CVisibilityManager::VisibilityManagerPrivate::SObject& obj = itObj->second;

	_private->_ptrOkTree->UnregisterObject(reinterpret_cast<UINT_PTR>(&obj), itObj->second._bbox);
	_private->_mapObjects.erase(itObj);
#else
	g_ptrTaskManager->DeleteObject(in_ObjectId);
#endif

	auto itObjVis = _private->_setAlwaysVisibleObjects.find(in_ObjectId);
	if (itObjVis != _private->_setAlwaysVisibleObjects.end())
		_private->_setAlwaysVisibleObjects.erase(itObjVis);
}

void CVisibilityManager::UpdateTextureVisibility()
{
	if (this == NULL)
		return;

	_private->_bUpdateTextureVisibility = true;
	UpdateVisibleObjectsSet();
}

void CVisibilityManager::UpdateVisibleObjectsSet ()
{
	if (this == NULL)
		return;

	if (!_private->_vecObjectAddRequests.empty())
	{
		for (size_t i = 0; i < _private->_vecObjectAddRequests.size(); i++)
		{
			const VisibilityManagerPrivate::SObjectAddRequest& addRequest = _private->_vecObjectAddRequests[i];
			_private->SetObjectInternal(addRequest._Object, addRequest._BBox, addRequest._fMaxDistance);
		}

		_private->_vecObjectAddRequests.clear();
	}

	_private->_vecVisibleObjects.resize(0);

	if (_private->_bUpdateTextureVisibility)
	{
		for (int i = 0; i < TEXTURE_TYPE_COUNT; i++)
		{
			_private->_asetVisibleTextures[i].clear();
			_private->_amapTexturePriority[i].clear();
		}
	}
	
#ifdef USE_OPENCL
	
	g_ptrTaskManager->CalculateVisibility(_private->_Camera);
	g_ptrTaskManager->UpdateVisibleSets(_private->_setVisibleObjects, _private->_aVisibleObjects);

#else
	
	Vector2f vResolution((float)_private->_uiScreenWidth, (float)_private->_uiScreenHeight);
	const CCamera& camera = _private->_Camera;
	IGridIterator& GI = _private->_ptrOkTree->GetIterator(camera.GetBoundBox(), &camera.GetFrustum());

	_private->_fNearClipPlane = 999999.f;
	_private->_fFarClipPlane = -999999.f;


	for(; !GI.IsEnd(); GI.Next())
	{
		CVisibilityManager::VisibilityManagerPrivate::SObject* pInternalObject = reinterpret_cast<CVisibilityManager::VisibilityManagerPrivate::SObject*>(GI.Get());

		if (pInternalObject->_bAlwaysVisible)
			continue;

		if (!pInternalObject->_bTexturesInited)
			pInternalObject->InitTextures();

		const CBoundBox<float>& bbox = pInternalObject->_bbox;

		if (! _private->_Camera.GetBoundBox().IsIntersectingAnotherBox(bbox))
			continue;

		bool bEnableMinimalSizeCheck = pInternalObject->_bMinimalSizeCheckEnabled;

		if (bEnableMinimalSizeCheck)
		{
			float fSizePixels = pInternalObject->GetBoundBoxMidSizeInPixels(camera, vResolution);

			if (fSizePixels < _private->_fMinimalObjectPixelSize)
				continue;
		}

		bool bInFrustum = true;

		BYTE btCrossFrustumPlaneFlag = GI.GetFrustumFlag();
		bInFrustum = !btCrossFrustumPlaneFlag || IsAABBInFrustum(bbox, _private->_Camera.GetFrustum(), btCrossFrustumPlaneFlag);

		if (! bInFrustum)
			continue;
		
#ifdef TEXTURE_VISIBILITY

		if (_private->_bUpdateTextureVisibility)
		{
			for (int iTextureType = 0; iTextureType < TEXTURE_TYPE_COUNT; iTextureType++)
			{
				for (size_t iTex = 0; iTex < pInternalObject->_vecTextures[iTextureType].size(); iTex++)
				{
					C3DBaseTexture* texture = pInternalObject->_vecTextures[iTextureType][iTex];
					_private->_asetVisibleTextures[iTextureType].insert(texture);

					float& fTexturePriority = _private->_amapTexturePriority[iTextureType][texture];
					float fDot = pInternalObject->GetCameraDirectionDotProduct(_private->_Camera, vResolution);

					if (fDot > fTexturePriority)
						fTexturePriority = fDot;
				}
			}
		}
#endif

		_private->_vecVisibleObjects.push_back(pInternalObject->_pObject);

		pInternalObject->_pObject->SetPotentiallyVisible();

		for (C3DBaseFaceSet* pFaceSet : pInternalObject->_vecFaceSets)
		{
			if (C3DBaseMaterial* material = pFaceSet->GetMaterialRef())
				material->AddVisibleFaceSet(pFaceSet);
		}
	}


#endif


	for (auto itVisObj = _private->_setAlwaysVisibleObjects.begin(); itVisObj != _private->_setAlwaysVisibleObjects.end(); itVisObj++)
	{
		CVisibilityManager::VisibilityManagerPrivate::SObject& internalObject = _private->_mapObjects[(*itVisObj)];
#ifdef TEXTURE_VISIBILITY

		if (_private->_bUpdateTextureVisibility)
		{
			for (int iTextureType = 0; iTextureType < TEXTURE_TYPE_COUNT; iTextureType++)
			{
				for (size_t iTex = 0; iTex < internalObject._vecTextures[iTextureType].size(); iTex++)
				{
					C3DBaseTexture* texture = internalObject._vecTextures[iTextureType][iTex];
					_private->_asetVisibleTextures[iTextureType].insert(texture);

					float& fTexturePriority = _private->_amapTexturePriority[iTextureType][texture];
					float fDot = internalObject.GetCameraDirectionDotProduct(_private->_Camera, vResolution);

					if (fDot > fTexturePriority)
						fTexturePriority = fDot;
				}
			}
		}

#endif

		_private->_vecVisibleObjects.push_back(*itVisObj);

		internalObject._pObject->SetPotentiallyVisible();

		for (C3DBaseFaceSet* pFaceSet : internalObject._vecFaceSets)
		{
			if (C3DBaseMaterial* material = pFaceSet->GetMaterialRef())
				material->AddVisibleFaceSet(pFaceSet);
		}
	}

	_private->_bUpdateTextureVisibility = false;
}

void CVisibilityManager::VisibilityManagerPrivate::SetObjectVisibleOnThisFrame(CVisibilityManager::VisibilityManagerPrivate::SObject& obj)
{
	for (C3DBaseFaceSet* pFaceSet : obj._vecFaceSets)
	{
		if (C3DBaseMaterial* material = pFaceSet->GetMaterialRef())
			material->AddVisibleFaceSet(pFaceSet);
	}
}

size_t CVisibilityManager::GetVisibleObjectsCount() const
{
	return _private->_vecVisibleObjects.size();
}

C3DBaseObject* CVisibilityManager::GetVisibleObjectPtr(size_t index) const
{
	if (index >= _private->_vecVisibleObjects.size())
		return NULL;

	return _private->_vecVisibleObjects[index];
}

bool CVisibilityManager::IsTextureVisible(C3DBaseTexture* texture) const
{
	bool bRes = _private->_asetVisibleTextures[texture->GetTextureType()].find(texture) != _private->_asetVisibleTextures[texture->GetTextureType()].end();
	return bRes;
}

float CVisibilityManager::GetTexturePriority(C3DBaseTexture* texture) const
{
	if (_private->_asetVisibleTextures[texture->GetTextureType()].find(texture) == _private->_asetVisibleTextures[texture->GetTextureType()].end())
		return -2;

	return _private->_amapTexturePriority[texture->GetTextureType()][texture];
}

// Получить текущие параметры камеры
void CVisibilityManager::GetCameraParameters(CameraDesc& out_parameters) const
{
	out_parameters.vPos = FromVec3(_private->_Camera.GetPos());
	out_parameters.vDir = FromVec3(_private->_Camera.GetDir());
	out_parameters.vUp = FromVec3(_private->_Camera.GetUp());

	out_parameters.horizontalFovTan = _private->_Camera.GetHorizontalHalfFovTan();
	out_parameters.verticalFovTan = _private->_Camera.GetVerticalHalfFovTan();
}

// Получить набор объектов из ориентированного бокса
void CVisibilityManager::GetObjectsFromOrientedBox(const OrientedBox& box, std::vector<C3DBaseObject*>& out_objects) const
{


}


float CVisibilityManager::GetNearClipPlane () const
{
	return _private->_fNearClipPlane;
}

float CVisibilityManager::GetFarClipPlane () const
{
	return _private->_fFarClipPlane;
}

bool CVisibilityManager::IsObjectVisible (C3DBaseObject* in_pObject) const
{
	if (_private->_setAlwaysVisibleObjects.find(in_pObject) != _private->_setAlwaysVisibleObjects.end())
		return true;

#ifdef USE_OPENCL
	return g_ptrTaskManager->IsObjectVisible(in_pObject);
#else
	return std::find(_private->_vecVisibleObjects.begin(), _private->_vecVisibleObjects.end(), in_pObject) != _private->_vecVisibleObjects.end();
#endif
}
