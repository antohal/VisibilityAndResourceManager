#include <Windows.h>

#include "ResourceManager.h"
#include "VisibilityManager.h"
#include "Log.h"
#include "Debug.h"

#include "Common.h"
#include "vecmath.h"

#include "Geometry/BoundBox.h"
#include "Geometry/Plane.h"
#include "Geometry/InFrustum.h"

#include "PrivateInterface.h"

#include <vector>
#include <map>
#include <list>

#include "D2DBaseTypes.h"
#include "CDirect2DTextRenderer.h"

#include <D3DX10math.h>
#include <math.h>

#define USE_DEBUG_INFO


using namespace std;

namespace {
	CResourceManager* g_ResourceManager = nullptr;
}

CResourceManager* GetResourceManager()
{
	return g_ResourceManager;
}

struct CResourceManager::SResourceManagerPrivate
{
	SResourceManagerPrivate(CResourceManager* owner) : _owner(owner)
	{}

	struct ViewProjection
	{
		Vector3D<float>			vPos, vDir, vUp;
		D3DXMATRIX				mProjection;
	};

	struct VisibilityPredictor
	{
		CameraDesc					_cameraParams;

		bool						_initialized = false;

		vm::Vector3df				_cameraPos;
		vm::Vector3df				_cameraDir;
		vm::Quaterniondf			_cameraOrient;

		vm::Quaterniondf			_predictedQuat;
		vm::Vector3df				_predictedPos;
		vm::Vector3df				_predictedDir;

		vm::Vector3df				_cameraVelocity;
		vm::Vector3df				_cameraAngularVelocity;

		std::list<vm::Vector3df>	_angularVelocityBuffer;
	};

	CResourceManager*			_owner = nullptr;
	C3DBaseObjectManager*		_objectManager = nullptr;

	float						_invisibleUnloadTime = 10.f;
	float						_boundBoxExtensionSpeedMultiplier = 10.f;
	float						_rotationRatePredictorMultiplier = 10.f;

	float						_horizontalFovDeg = 0;
	float						_verticalFovDeg = 0;


	ViewProjection				_viewProjection;
	bool						_bViewProjectionInitialized = false;


	VisibilityPredictor			_predictor;

	vector<CVisibilityManager*>	_visibilityManagers;

	map<C3DBaseObject*, float>	_objectVisibilityTimers;

	CDirect2DTextBlock*			_textBlock = nullptr;


	float						_cameraRateInterval = 0.25;

	UINT						_paramPotentiallyVisibleResources = -1;
	UINT						_paramPotentiallyVisibleObjects = -1;
	UINT						_paramPotentiallyVisibleMeshes = -1;
	UINT						_paramPotentiallyVisibleFacesets = -1;
	UINT						_paramPotentiallyVisibleMaterials = -1;
	UINT						_paramPotentiallyVisibleTextures = -1;

	UINT						_paramCameraVelocity = -1;
	UINT						_paramCameraAngularVelocity = -1;
	UINT						_paramCameraDirection = -1;

	UINT						_paramCameraPos = -1;
	UINT						_paramPredictedPos = -1;
	UINT						_paramPredictedDirection = -1;

	unsigned int				_potentiallyVisibleResources = 0;

#ifdef USE_DEBUG_INFO
	std::map<E3DResourceType, int>	_mapPotentiallyVisibleByType;
#endif

	void SetPredictionFOV(float horizontalFovDeg, float verticalFovDeg)
	{
		_horizontalFovDeg = horizontalFovDeg;
		_verticalFovDeg = verticalFovDeg;
	}

	void incrementVisibilityRefCountRecursive(C3DBaseResource* resource)
	{
		if (!resource)
			return;

		if (resource->_visibleRefCount == 0)
		{
			_potentiallyVisibleResources++;

#ifdef USE_DEBUG_INFO
			_mapPotentiallyVisibleByType[resource->GetType()] ++;
#endif

			if (resource->GetManager())
			{
				resource->GetManager()->RequestLoadResource(resource);
				LogMessage("CResourceManager: resource with type %d is became visible, requesting load", resource->GetType());
			}
		}

		resource->_visibleRefCount++;
		//LogMessage("CResourceManager: resource with type %d incremented refcounter %d", resource->GetType(), resource->_visibleRefCount);

		for (size_t iChildResource = 0; iChildResource < resource->GetChildResourceCount(); iChildResource++)
		{
			C3DBaseResource* childResource = resource->GetChildResourceById(iChildResource);
			incrementVisibilityRefCountRecursive(childResource);
		}
	}

	void decrementVisibilityRefCountRecursive(C3DBaseResource* resource)
	{
		if (!resource)
			return;

		// TODO: insert assert here
		if (resource->_visibleRefCount > 0)
		{
			resource->_visibleRefCount--;
			//LogMessage("CResourceManager: resource with type %d decremented refcounter %d", resource->GetType(), resource->_visibleRefCount);

			if (resource->_visibleRefCount == 0)
			{
				_potentiallyVisibleResources--;

#ifdef USE_DEBUG_INFO
				_mapPotentiallyVisibleByType[resource->GetType()] --;
#endif

				if (resource->GetManager())
				{
					resource->GetManager()->RequestUnloadResource(resource);
					LogMessage("CResourceManager: resource with type %d no longer referenced, unloading", resource->GetType());
				}
			}
		} 

		for (size_t iChildResource = 0; iChildResource < resource->GetChildResourceCount(); iChildResource++)
		{
			C3DBaseResource* childResource = resource->GetChildResourceById(iChildResource);
			decrementVisibilityRefCountRecursive(childResource);
		}
	}

	void UpdatePredictorCameraParameters()
	{
		if (!_bViewProjectionInitialized)
		{
			LogMessage("CResourceManager::UpdatePredictorCameraParameters - cannot init camera params, SetViewProjection not called.");
			return;
		}

		_predictor._cameraParams.vPos = Vector3(_viewProjection.vPos.x, _viewProjection.vPos.y, _viewProjection.vPos.z);
		_predictor._cameraParams.vDir = Vector3(_viewProjection.vDir.x, _viewProjection.vDir.y, _viewProjection.vDir.z);
		_predictor._cameraParams.vUp = Vector3(_viewProjection.vUp.x, _viewProjection.vUp.y, _viewProjection.vUp.z);

		const D3DXMATRIX& mProjection = _viewProjection.mProjection;

		_predictor._cameraParams.verticalFov = static_cast<float>(D2R) * 2.f * atanf(1.f / mProjection(1, 1));

		float coeff = mProjection(1, 1) * sinf(0.5f * static_cast<float>(D2R) * _predictor._cameraParams.verticalFov) / mProjection(0, 0);
		_predictor._cameraParams.horizontalFov = static_cast<float>(D2R) * 2.f * asinf(coeff);

		_predictor._cameraParams.farPlane = mProjection(3, 2) / (1 - mProjection(2, 2));
		_predictor._cameraParams.nearPlane = _predictor._cameraParams.farPlane * (1 - 1 / mProjection(2, 2));
	}

	void predictMovement(float deltaTime, std::vector<CollectObjectsData>& out_vecCollectObjectsData)
	{
		bool cameraParametersInitialized = false;

		if (_bViewProjectionInitialized)
		{
			UpdatePredictorCameraParameters();

			cameraParametersInitialized = true;
		}
		else
		{
			if (!_visibilityManagers.empty())
			{
				_visibilityManagers[0]->GetCameraParameters(_predictor._cameraParams);

				cameraParametersInitialized = true;
			}
		}

		if (!cameraParametersInitialized)
		{
			LogMessage("CResourceManager::predictMovement - cannot predict movement, because of empty visibility managers and no SetViewProjection");
			return;
		}


		vm::Vector3df vCameraPos = vm::Vector3df(_predictor._cameraParams.vPos.x, _predictor._cameraParams.vPos.y, _predictor._cameraParams.vPos.z);
		vm::Vector3df vCameraDir = vm::Vector3df(_predictor._cameraParams.vDir.x, _predictor._cameraParams.vDir.y, _predictor._cameraParams.vDir.z);
		vm::Vector3df vCameraUp = vm::Vector3df(_predictor._cameraParams.vUp.x, _predictor._cameraParams.vUp.y, _predictor._cameraParams.vUp.z);
		vm::Vector3df vCameraLeft = vm::cross(vCameraUp, vCameraDir);

		vm::Matrix3x3df mMatrixTransform = vm::Matrix3x3df(vCameraDir, vCameraLeft, vCameraUp);

		vm::Quaterniondf qCameraOrientation(mMatrixTransform);

		float horizontalFovDeg = _predictor._cameraParams.horizontalFov;
		float verticalFovDeg = _predictor._cameraParams.verticalFov;

		if (_horizontalFovDeg > 0)
			horizontalFovDeg = _horizontalFovDeg;

		if (_verticalFovDeg > 0)
			verticalFovDeg = _verticalFovDeg;

		Vector3D<float> vNormalPos((float)vCameraPos[0], (float)vCameraPos[1], (float)vCameraPos[2]);
		Vector3D<float> vNormalDir((float)vCameraDir[0], (float)vCameraDir[1], (float)vCameraDir[2]);
		Vector3D<float> vNormalUp((float)vCameraUp[0], (float)vCameraUp[1], (float)vCameraUp[2]);

		if (_predictor._initialized)
		{
			_predictor._cameraVelocity = (vCameraPos - _predictor._cameraPos) / deltaTime;
			
			//@{ calculate angular velocity
			vm::Quaterniondf cameraOrientDot = (1.0 / deltaTime) * (qCameraOrientation - _predictor._cameraOrient);

			vm::Quaterniondf qW = 2.0 * (cameraOrientDot * qCameraOrientation.GetInverse());

			vm::Vector3df angVel(qW[1], qW[2], qW[3]);

			_predictor._angularVelocityBuffer.push_back(angVel);

			size_t MAX_VEL_BUF = 5;
			
			if (deltaTime > 0)
				MAX_VEL_BUF = (size_t)(_cameraRateInterval / deltaTime);

			if (MAX_VEL_BUF < 5)
				MAX_VEL_BUF = 5;

			if (MAX_VEL_BUF > 100)
				MAX_VEL_BUF = 100;

			while (_predictor._angularVelocityBuffer.size() > MAX_VEL_BUF)
			{
				_predictor._angularVelocityBuffer.pop_front();
			}

			_predictor._cameraAngularVelocity.Zero();
			for (const vm::Vector3df& vBufferedAngVel : _predictor._angularVelocityBuffer)
			{
				_predictor._cameraAngularVelocity += vBufferedAngVel;
			}

			_predictor._cameraAngularVelocity *= 1.0 / MAX_VEL_BUF;
			//@}

			//@{ predict

			//@{ ограничить величину угловой скорости
			if (vm::length(_predictor._cameraAngularVelocity)*_rotationRatePredictorMultiplier > M_PI * 0.5)
				_predictor._cameraAngularVelocity = vm::normalize(_predictor._cameraAngularVelocity) * M_PI * 0.5 / _rotationRatePredictorMultiplier;
			//@}

			vm::Quaterniondf qRotation(0, _predictor._cameraAngularVelocity[0], _predictor._cameraAngularVelocity[1], _predictor._cameraAngularVelocity[2]);
			vm::Quaterniondf qDOrientation = (qRotation*qCameraOrientation)*0.5;

			_predictor._predictedQuat = qCameraOrientation + ((double)_rotationRatePredictorMultiplier * deltaTime) * (qDOrientation);
			_predictor._predictedQuat.Normalize();

			_predictor._predictedPos = vCameraPos + _boundBoxExtensionSpeedMultiplier * deltaTime * _predictor._cameraVelocity;

			//@}

			//@{

			vm::Matrix3x3df mCameraOrient = _predictor._predictedQuat.ToSO3Operator();
			_predictor._predictedDir = mCameraOrient[0];

			Vector3D<float> vPredictedPos((float)_predictor._predictedPos[0], (float)_predictor._predictedPos[1], (float)_predictor._predictedPos[2]);
			Vector3D<float> vPredictedDir((float)_predictor._predictedDir[0], (float)_predictor._predictedDir[1], (float)_predictor._predictedDir[2]);
			Vector3D<float> vPredictedUp((float)mCameraOrient[2][0], (float)mCameraOrient[2][1], (float)mCameraOrient[2][2]);

			out_vecCollectObjectsData.push_back(CollectObjectsData(vPredictedPos, vPredictedDir, vPredictedUp, _predictor._cameraParams.nearPlane, _predictor._cameraParams.farPlane,
				horizontalFovDeg, verticalFovDeg));


			//@}

			if (vm::length(_predictor._predictedPos - vCameraPos) > 0.1f)
			{
				vm::Vector3df vVelocityDirection = vm::normalize(_predictor._predictedPos - vCameraPos);

				Vector3D<float> vMiddleDir = Vector3D<float>((float)vVelocityDirection[0], (float)vVelocityDirection[1], (float)vVelocityDirection[2]);
				Matrix3x3<float> mCameraMiddle = GetMatrixFromForwardDirection(vMiddleDir, vNormalUp);

				Vector3D<float> vMiddleUp = mCameraMiddle.line3;

				out_vecCollectObjectsData.push_back(CollectObjectsData(vNormalPos, vMiddleDir, vMiddleUp, _predictor._cameraParams.nearPlane, (float)length(_predictor._predictedPos - vCameraPos),
					horizontalFovDeg, verticalFovDeg));

				out_vecCollectObjectsData.push_back(CollectObjectsData(vPredictedPos, vNormalDir, vNormalUp, _predictor._cameraParams.nearPlane, _predictor._cameraParams.farPlane,
					horizontalFovDeg, verticalFovDeg));
			}
		}

		out_vecCollectObjectsData.push_back(CollectObjectsData(vNormalPos, vNormalDir, vNormalUp, _predictor._cameraParams.nearPlane, _predictor._cameraParams.farPlane,
			horizontalFovDeg, verticalFovDeg));

		_predictor._cameraPos = vCameraPos;
		_predictor._cameraDir = vCameraDir;
		_predictor._cameraOrient = qCameraOrientation;

		_predictor._initialized = true;
	}

	void updatePredictorDebugText()
	{
		if (!_textBlock)
			return;

		_textBlock->UpdateFormattedTextLine(_paramCameraVelocity, _predictor._cameraVelocity[0], _predictor._cameraVelocity[1], _predictor._cameraVelocity[2]);
		_textBlock->UpdateFormattedTextLine(_paramCameraAngularVelocity, R2D*_predictor._cameraAngularVelocity[0], R2D*_predictor._cameraAngularVelocity[1], R2D*_predictor._cameraAngularVelocity[2]);
		
		_textBlock->UpdateFormattedTextLine(_paramCameraPos, _predictor._cameraPos[0], _predictor._cameraPos[1], _predictor._cameraPos[2]);
		_textBlock->UpdateFormattedTextLine(_paramPredictedPos, _predictor._predictedPos[0], _predictor._predictedPos[1], _predictor._predictedPos[2]);

		_textBlock->UpdateFormattedTextLine(_paramPredictedDirection, _predictor._predictedDir[0], _predictor._predictedDir[1], _predictor._predictedDir[2]);
		_textBlock->UpdateFormattedTextLine(_paramCameraDirection, _predictor._cameraDir[0], _predictor._cameraDir[1], _predictor._cameraDir[2]);
	}
};

void C3DBaseObject::SetPotentiallyVisible()
{
	if (!GetResourceManager())
		return;

	map<C3DBaseObject*, float>& objectVisibilityTimers = GetResourceManager()->_private->_objectVisibilityTimers;

	// Если объект стал виден впервые, или после долгого перерыва, то обновить счетчики видимости
	if (objectVisibilityTimers.find(this) == objectVisibilityTimers.end())
	{
		GetResourceManager()->_private->incrementVisibilityRefCountRecursive(this);
	}

	GetResourceManager()->_private->_objectVisibilityTimers[this] = GetResourceManager()->_private->_invisibleUnloadTime;

	//LogMessage("CResourceManager: object %d is potentially visible", reinterpret_cast<UINT_PTR>(this));
}


CResourceManager::CResourceManager()
{
	_private = new SResourceManagerPrivate(this);
	g_ResourceManager = this;

	LogInit("resman.log");
	LogEnable(true);

	LogMessage("CResourceManager: created.");
}

CResourceManager::~CResourceManager()
{
	LogMessage("CResourceManager: destructed.");

	delete _private;
}

void CResourceManager::Init(C3DBaseObjectManager* objectManager)
{
	_private->_objectManager = objectManager;

	//SetupDebugParameters();

	LogMessage("CResourceManager: inited OK.");
}

void CResourceManager::SetupDebugParameters()
{
	SetPredictionFOV(45, 45);
	SetInvisibleUnloadTime(0.5);
}

void CResourceManager::SetPredictionFOV(float horizontalFovDeg, float verticalFovDeg)
{
	_private->SetPredictionFOV(horizontalFovDeg, verticalFovDeg);
}

void CResourceManager::SetViewProjection(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection)
{
	_private->_viewProjection.vPos = Vector3D<float>(in_vPos.x, in_vPos.y, in_vPos.z);
	_private->_viewProjection.vDir = Vector3D<float>(in_vDir.x, in_vDir.y, in_vDir.z);
	_private->_viewProjection.vUp = Vector3D<float>(in_vUp.x, in_vUp.y, in_vUp.z);

	_private->_viewProjection.mProjection = *in_pmProjection;

	_private->_bViewProjectionInitialized = true;
}

void CResourceManager::SetCamera(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, float in_fHorizontalFOV, float in_fVerticalFOV, float in_fNearPlane, float in_fFarPlane)
{
	_private->_viewProjection.vPos = Vector3D<float>(in_vPos.x, in_vPos.y, in_vPos.z);
	_private->_viewProjection.vDir = Vector3D<float>(in_vDir.x, in_vDir.y, in_vDir.z);
	_private->_viewProjection.vUp = Vector3D<float>(in_vUp.x, in_vUp.y, in_vUp.z);

	float aspect = sinf(0.5f * in_fHorizontalFOV * static_cast<float>(D2R) / sinf(0.5f * in_fVerticalFOV * static_cast<float>(D2R)));

	D3DXMatrixPerspectiveFovLH(&_private->_viewProjection.mProjection, in_fVerticalFOV * static_cast<float>(D2R), aspect, in_fNearPlane, in_fFarPlane);

	_private->_bViewProjectionInitialized = true;
}

void CResourceManager::SetInvisibleUnloadTime(float time)
{
	LogMessage("CResourceManager: InvisibleUnloadTime set to %f.", time);

	if (time < 0.5)
		time = 0.5;

	_private->_invisibleUnloadTime = time;
}

void CResourceManager::SetRotationRateAverageInterval(float intervalSec)
{
	_private->_cameraRateInterval = intervalSec;
}

void CResourceManager::SetSpeedVisibilityPredictionTime(float speedMultiplier)
{
	LogMessage("CResourceManager: SpeedPotentialVisibilityMultiplier set to %f.", speedMultiplier);

	_private->_boundBoxExtensionSpeedMultiplier = speedMultiplier;
}

void CResourceManager::SetRotationRateVisibilityPredictionTime(float rateMultiplier)
{
	LogMessage("CResourceManager: RotationRatePotentialVisibilityMultiplier set to %f.", rateMultiplier);

	_private->_rotationRatePredictorMultiplier = rateMultiplier;
}

void CResourceManager::AddVisibilityManager(CVisibilityManager* visibilityManager)
{
	_private->_visibilityManagers.push_back(visibilityManager);

	LogMessage("CResourceManager: added visibility manager %d", reinterpret_cast<UINT_PTR>(visibilityManager));
}

CollectObjectsData::CollectObjectsData(const Vector3D<float>& pos, const Vector3D<float>& dir, const Vector3D<float>& up,
	float nearPlane, float farPlane, float horizontalFov, float verticalFov)
{
	Matrix3x3f mOrientation = GetMatrixFromForwardDirection<float>(Normalize(dir), Normalize(up));

	SFOV_Tan HorizontalFOV(horizontalFov);
	SFOV_Tan VerticalFOV(verticalFov);

	UpdateFrustumPlanes(mOrientation, pos, farPlane, nearPlane,
		HorizontalFOV.GetTanFOVAngle(), VerticalFOV.GetTanFOVAngle(),
		_boundBox, _frustum);
}

void CResourceManager::Update(float deltaTime)
{
	if (deltaTime > 1)
		deltaTime = 1;

	if (deltaTime == 0)
	{
		// TODO: log error here
		LogMessage("CResourceManager ERROR: zero delta time!");
		return;
	}

	if (!_private->_visibilityManagers.empty())
	{
		/* Set camera test !
		CameraDesc camParams;
		(*_private->_visibilityManagers.begin())->GetCameraParameters(camParams);

		SetCamera(camParams.vPos, camParams.vDir, camParams.vUp, camParams.horizontalFov, camParams.verticalFov, camParams.nearPlane, camParams.farPlane);
		*/


		std::vector<CollectObjectsData> vecCollectObjectsData;
		_private->predictMovement( deltaTime, vecCollectObjectsData);
		(*_private->_visibilityManagers.begin())->GetPrivateInterface()->MarkPotentiallyVisibleObjects(vecCollectObjectsData);
	}

	map<C3DBaseObject*, float>& objectVisibilityTimers = _private->_objectVisibilityTimers;

	for (auto it = objectVisibilityTimers.begin(); it != objectVisibilityTimers.end();)
	{
		float& timer = it->second;

		timer -= deltaTime;

		if (timer <= 0)
		{
			_private->decrementVisibilityRefCountRecursive(it->first);
			it = objectVisibilityTimers.erase(it);
		}
		else
			it++;
	}

	if (_private->_textBlock)
	{
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleResources, (float) _private->_potentiallyVisibleResources);

#ifdef USE_DEBUG_INFO
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleObjects, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_OBJECT]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleMeshes, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_MESH]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleFacesets, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_FACESET]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleMaterials, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_MATERIAL]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleTextures, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_TEXTURE]);

		_private->updatePredictorDebugText();

#endif

	}
}

float CResourceManager::GetTexturePriority(C3DBaseTexture* texture)
{
	float texturePriority = -2;

	for (CVisibilityManager* visibilityManager : _private->_visibilityManagers)
	{
		float localTexturePriority = visibilityManager->GetTexturePriority(texture);

		if (localTexturePriority > 0)
		{
			if (texturePriority < 0 || localTexturePriority < texturePriority)
				texturePriority = localTexturePriority;
		}
	}

	return texturePriority;
}

void CResourceManager::EnableLog(bool enable/* = true*/)
{
	LogMessage("CResourceManager: Log state is now %d.", (int) enable);

	LogEnable(enable);
}

// Выключить лог в файл
void CResourceManager::DisableLog()
{
	LogMessage("CResourceManager: Log disabled.");

	LogEnable(false);
}

// Включить рендеринг отладочной информации в текстовый блок
void CResourceManager::EnableDebugTextRender(CDirect2DTextBlock* textBlock)
{
	if (_private->_textBlock)
	{
		LogMessage("CResourceManager::EnableDebugTextRender: text block already assigned, disable first!");
		return;
	}

	if (!textBlock)
	{
		LogMessage("CResourceManager::EnableDebugTextRender: input textBlock is NULL!");
		return;
	}

	_private->_textBlock = textBlock;

	_private->_paramPotentiallyVisibleResources =  textBlock->AddParameter(L"Количество потенциально видимых ресурсов");

	_private->_paramPotentiallyVisibleObjects = textBlock->AddParameter(L"Объектов");
	_private->_paramPotentiallyVisibleMeshes = textBlock->AddParameter(L"Мешей");
	_private->_paramPotentiallyVisibleFacesets = textBlock->AddParameter(L"Фейссетов");
	_private->_paramPotentiallyVisibleMaterials = textBlock->AddParameter(L"Материалов");
	_private->_paramPotentiallyVisibleTextures = textBlock->AddParameter(L"Текстур");

	textBlock->AddTextLine(L"_____________________________");

	_private->_paramCameraVelocity = textBlock->AddFormattedTextLine(L"Линейная скорость камеры:\n[%.3lf,\t%.3lf, \t%.3lf]");
	_private->_paramCameraAngularVelocity = textBlock->AddFormattedTextLine(L"Угловая скорость камеры (град/сек):\n[%.3lf,\t%.3lf, \t%.3lf]");

	_private->_paramCameraPos = textBlock->AddFormattedTextLine(L"Позиция камеры:\n[%.3lf,\t%.3lf, \t%.3lf]");
	_private->_paramPredictedPos = textBlock->AddFormattedTextLine(L"Предсказанная позиция камеры:\n[%.3lf,\t%.3lf, \t%.3lf]");

	_private->_paramCameraDirection = textBlock->AddFormattedTextLine(L"Направление камеры:\n[%.3lf,\t%.3lf, \t%.3lf]");
	_private->_paramPredictedDirection = textBlock->AddFormattedTextLine(L"Предсказанное Направление камеры:\n[%.3lf,\t%.3lf, \t%.3lf]");

}

// Выключить рендеринг отладочной информации в текстовый блок
void CResourceManager::DisableDebugTextRender()
{
	_private->_textBlock = nullptr;
}
