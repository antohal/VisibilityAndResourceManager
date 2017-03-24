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


	CResourceManager*			_owner = nullptr;
	C3DBaseObjectManager*		_objectManager = nullptr;

	float						_invisibleUnloadTime = 10.f;
	float						_boundBoxExtensionSpeedMultiplier = 10.f;
	float						_rotationRatePredictorMultiplier = 10.f;

	float						_horizontalFovDeg = 0;
	float						_verticalFovDeg = 0;

	void	SetPredictionFOV(float horizontalFovDeg, float verticalFovDeg)
	{
		_horizontalFovDeg = horizontalFovDeg;
		_verticalFovDeg = verticalFovDeg;
	}


	struct VisibilityPredictor
	{
		CameraDesc				_cameraParams;

		bool					_initialized = false;

		vm::Vector3df			_cameraPos;
		vm::Vector3df			_cameraDir;
		vm::Quaterniondf		_cameraOrient;
		
		vm::Quaterniondf		_predictedQuat;
		vm::Vector3df			_predictedPos;
		vm::Vector3df			_predictedDir;

		vm::Vector3df			_cameraVelocity;
		vm::Vector3df			_cameraAngularVelocity;

		std::list<vm::Vector3df>	_angularVelocityBuffer;
	};

	vector<CVisibilityManager*>		_visibilityManagers;
	map<CVisibilityManager*, VisibilityPredictor>	_mapVisibilityPredictors;

	map<C3DBaseObject*, float>		_objectVisibilityTimers;

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

	void predictMovement(CVisibilityManager* in_pVisibilityManager, float deltaTime, std::vector<CollectObjectsData>& out_vecCollectObjectsData)
	{
		VisibilityPredictor& predictor = _mapVisibilityPredictors[in_pVisibilityManager];
		
		in_pVisibilityManager->GetCameraParameters(predictor._cameraParams);

		vm::Vector3df vCameraPos = vm::Vector3df(predictor._cameraParams.vPos.x, predictor._cameraParams.vPos.y, predictor._cameraParams.vPos.z);
		vm::Vector3df vCameraDir = vm::Vector3df(predictor._cameraParams.vDir.x, predictor._cameraParams.vDir.y, predictor._cameraParams.vDir.z);
		vm::Vector3df vCameraUp = vm::Vector3df(predictor._cameraParams.vUp.x, predictor._cameraParams.vUp.y, predictor._cameraParams.vUp.z);
		vm::Vector3df vCameraLeft = vm::cross(vCameraUp, vCameraDir);

		vm::Matrix3x3df mMatrixTransform = vm::Matrix3x3df(vCameraDir, vCameraLeft, vCameraUp);

		vm::Quaterniondf qCameraOrientation(mMatrixTransform);

		float horizontalFovDeg = predictor._cameraParams.horizontalFov;
		float verticalFovDeg = predictor._cameraParams.verticalFov;

		if (_horizontalFovDeg > 0)
			horizontalFovDeg = _horizontalFovDeg;

		if (_verticalFovDeg > 0)
			verticalFovDeg = _verticalFovDeg;

		Vector3D<float> vNormalPos(vCameraPos[0], vCameraPos[1], vCameraPos[2]);
		Vector3D<float> vNormalDir(vCameraDir[0], vCameraDir[1], vCameraDir[2]);
		Vector3D<float> vNormalUp(vCameraUp[0], vCameraUp[1], vCameraUp[2]);

		if (predictor._initialized)
		{
			predictor._cameraVelocity = (vCameraPos - predictor._cameraPos) / deltaTime;
			
			//@{ calculate angular velocity
			vm::Quaterniondf cameraOrientDot = (1.0 / deltaTime) * (qCameraOrientation - predictor._cameraOrient);

			vm::Quaterniondf qW = 2.0 * (cameraOrientDot * qCameraOrientation.GetInverse());

			vm::Vector3df angVel(qW[1], qW[2], qW[3]);

			predictor._angularVelocityBuffer.push_back(angVel);

			size_t MAX_VEL_BUF = 5;
			
			if (deltaTime > 0)
				MAX_VEL_BUF = (size_t)(_cameraRateInterval / deltaTime);

			if (MAX_VEL_BUF < 5)
				MAX_VEL_BUF = 5;

			if (MAX_VEL_BUF > 100)
				MAX_VEL_BUF = 100;

			while (predictor._angularVelocityBuffer.size() > MAX_VEL_BUF)
			{
				predictor._angularVelocityBuffer.pop_front();
			}

			predictor._cameraAngularVelocity.Zero();
			for (const vm::Vector3df& vBufferedAngVel : predictor._angularVelocityBuffer)
			{
				predictor._cameraAngularVelocity += vBufferedAngVel;
			}

			predictor._cameraAngularVelocity *= 1.0 / MAX_VEL_BUF;
			//@}

			//@{ predict

			//@{ ограничить величину угловой скорости
			if (vm::length(predictor._cameraAngularVelocity)*_rotationRatePredictorMultiplier > M_PI)
				predictor._cameraAngularVelocity = vm::normalize(predictor._cameraAngularVelocity) * M_PI / _rotationRatePredictorMultiplier;
			//@}

			vm::Quaterniondf qRotation(0, predictor._cameraAngularVelocity[0], predictor._cameraAngularVelocity[1], predictor._cameraAngularVelocity[2]);
			vm::Quaterniondf qDOrientation = (qRotation*qCameraOrientation)*0.5;

			predictor._predictedQuat = qCameraOrientation + ((double)_rotationRatePredictorMultiplier * deltaTime) * (qDOrientation);
			predictor._predictedQuat.Normalize();

			predictor._predictedPos = vCameraPos + _boundBoxExtensionSpeedMultiplier * deltaTime * predictor._cameraVelocity;

			//@}

			//@{

			vm::Matrix3x3df mCameraOrient = predictor._predictedQuat.ToSO3Operator();
			predictor._predictedDir = mCameraOrient[0];

			Vector3D<float> vPredictedPos(predictor._predictedPos[0], predictor._predictedPos[1], predictor._predictedPos[2]);
			Vector3D<float> vPredictedDir(predictor._predictedDir[0], predictor._predictedDir[1], predictor._predictedDir[2]);
			Vector3D<float> vPredictedUp(mCameraOrient[2][0], mCameraOrient[2][1], mCameraOrient[2][2]);

			out_vecCollectObjectsData.push_back(CollectObjectsData(vPredictedPos, vPredictedDir, vPredictedUp, predictor._cameraParams.nearPlane, predictor._cameraParams.farPlane,
				horizontalFovDeg, verticalFovDeg));


			//@}

			if (vm::length(predictor._predictedPos - vCameraPos) > 0.1f)
			{
				vm::Vector3df vVelocityDirection = vm::normalize(predictor._predictedPos - vCameraPos);

				Vector3D<float> vMiddleDir = Vector3D<float>(vVelocityDirection[0], vVelocityDirection[1], vVelocityDirection[2]);
				Matrix3x3<float> mCameraMiddle = GetMatrixFromForwardDirection(vMiddleDir, vNormalUp);

				Vector3D<float> vMiddleUp = mCameraMiddle.line3;

				out_vecCollectObjectsData.push_back(CollectObjectsData(vNormalPos, vMiddleDir, vMiddleUp, predictor._cameraParams.nearPlane, length(predictor._predictedPos - vCameraPos),
					horizontalFovDeg, verticalFovDeg));

				out_vecCollectObjectsData.push_back(CollectObjectsData(vPredictedPos, vNormalDir, vNormalUp, predictor._cameraParams.nearPlane, predictor._cameraParams.farPlane,
					horizontalFovDeg, verticalFovDeg));
			}
		}

		out_vecCollectObjectsData.push_back(CollectObjectsData(vNormalPos, vNormalDir, vNormalUp, predictor._cameraParams.nearPlane, predictor._cameraParams.farPlane,
			horizontalFovDeg, verticalFovDeg));

		predictor._cameraPos = vCameraPos;
		predictor._cameraDir = vCameraDir;
		predictor._cameraOrient = qCameraOrientation;

		predictor._initialized = true;
	}

	void updatePredictorDebugText()
	{
		if (_mapVisibilityPredictors.empty())
			return;

		if (!_textBlock)
			return;

		const VisibilityPredictor& firstPredictor = _mapVisibilityPredictors.begin()->second;

		_textBlock->UpdateFormattedTextLine(_paramCameraVelocity, firstPredictor._cameraVelocity[0], firstPredictor._cameraVelocity[1], firstPredictor._cameraVelocity[2]);
		_textBlock->UpdateFormattedTextLine(_paramCameraAngularVelocity, R2D*firstPredictor._cameraAngularVelocity[0], R2D*firstPredictor._cameraAngularVelocity[1], R2D*firstPredictor._cameraAngularVelocity[2]);
		
		_textBlock->UpdateFormattedTextLine(_paramCameraPos, firstPredictor._cameraPos[0], firstPredictor._cameraPos[1], firstPredictor._cameraPos[2]);
		_textBlock->UpdateFormattedTextLine(_paramPredictedPos, firstPredictor._predictedPos[0], firstPredictor._predictedPos[1], firstPredictor._predictedPos[2]);

		_textBlock->UpdateFormattedTextLine(_paramPredictedDirection, firstPredictor._predictedDir[0], firstPredictor._predictedDir[1], firstPredictor._predictedDir[2]);
		_textBlock->UpdateFormattedTextLine(_paramCameraDirection, firstPredictor._cameraDir[0], firstPredictor._cameraDir[1], firstPredictor._cameraDir[2]);
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
	_pos = pos;

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

	for (CVisibilityManager* visMan : _private->_visibilityManagers)
	{
		std::vector<CollectObjectsData> vecCollectObjectsData;

		_private->predictMovement(visMan, deltaTime, vecCollectObjectsData);

		visMan->GetPrivateInterface()->MarkPotentiallyVisibleObjects(vecCollectObjectsData);
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
