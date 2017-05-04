#pragma once

#include <string>
#include <list>
#include <vector>

#include "Scene.h"

#include "C3DBaseObject.h"
#include "C3DBaseFaceSet.h"
#include "C3DBaseMaterial.h"
#include "C3DBaseObjectManager.h"

#include "TerrainDataManager.h"
#include "HeightfieldConverter.h"

#include "D3DX10.h"
#include <d3d11.h>

class CD3DStaticTerrainRenderer;
class CD3DStaticTerrainMaterial;
class CD3DStaticTerrainFaceset;
class HeightfieldConverter;
class CDirect3DSystem;

class CD3DStaticTerrainObject : public C3DBaseObject
{
public:

	CD3DStaticTerrainObject(CD3DStaticTerrainRenderer* in_pOwner);

	void	SetFaceset(CD3DStaticTerrainFaceset*);

	//@{ C3DBaseObject

	// Функция вычисления расстояния до объекта
	virtual float GetDistance(const D3DXVECTOR3* in_pvPointFrom) const;

	// Получить минимальное расстояние видимости до объекта
	virtual float GetMinimalVisibleDistance() const;

	// Получить максимальное расстояние видимости до объекта
	virtual float GetMaximalVisibleDistance() const;

	// Получить родительский объект-лод
	virtual C3DBaseObject*	GetParentLODObject();

	// Получить количество дочерних объектов-лодов
	virtual unsigned int	GetNumChildLODObjects();

	// Получить дочерний лод-объект
	virtual C3DBaseObject*	GetChildLODObject(unsigned int id);

	// Все 3D объекты должны будут возвращать Баунд-Бокс. Причем, если объект - точка, а не меш, то
	// пусть вернет одинаковые значения в out_vBBMin и out_vBBMax.
	virtual void							GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) override;

	// Получить матрицу трансформации
	virtual D3DXMATRIX*						GetWorldTransform() override;

	// Функция должна возвращать: включена-ли проверка размера объекта на экране
	virtual bool							IsMinimalSizeCheckEnabled() const override { return false; };

	// Функция возврящает количество мешей данного объекта
	virtual size_t							GetMeshesCount() const override { return 0; }

	// Функция возвращает конкретный меш объекта по его идентификатору
	virtual C3DBaseMesh*					GetMeshById(size_t id) const { return nullptr; }

	// получить список фейссетов
	virtual size_t							GetFaceSetsCount() const { return 1; }
	virtual C3DBaseFaceSet*					GetFaceSetById(size_t id) const;

	virtual C3DBaseManager*					GetManager() const { return nullptr; }

	//@}

private:

	CD3DStaticTerrainFaceset*				_pFaceset = nullptr;

	D3DXMATRIX								_mTransform;
	D3DXVECTOR3								_vBBoxMin;
	D3DXVECTOR3								_vBBoxMax;

	CD3DStaticTerrainRenderer*				_owner = nullptr;
};

class CD3DStaticTerrainFaceset : public C3DBaseFaceSet
{
public:

	CD3DStaticTerrainFaceset(CD3DStaticTerrainRenderer* in_pOwner, CD3DStaticTerrainObject* in_pObject, CD3DStaticTerrainMaterial* in_pMaterial, const CTerrainBlockData* in_pTerrainBlockData);

	//@{ C3DBaseFaceSet

	// получить ссылку на материал
	virtual C3DBaseMaterial*				GetMaterialRef() override;

	virtual C3DBaseManager*					GetManager() const;

	const CTerrainBlockData*				GetTerrainBlockData() const;

	//@}

	const STriangulation&					GetTriangulation() const;
	STriangulation&							GetTriangulation();

	void	Load();
	void	Unload();

	void	SetIndexAndVertexBuffers(CD3DGraphicsContext* in_pContext);
	unsigned int GetIndexCount() const;

protected:

	void	ComputeTriangulationCoords();

private:

	SHeightfield							_heightfield;
	STriangulation							_triangulation;

	CD3DStaticTerrainMaterial*				_pMaterialRef = nullptr;
	CD3DStaticTerrainObject*				_pTerrainObject = nullptr;
	const CTerrainBlockData*				_pTerrainBlockData = nullptr;

	CD3DStaticTerrainRenderer*				_owner = nullptr;

	friend class CD3DStaticTerrainObject;
};

class CD3DStaticTerrainMaterial : public C3DBaseMaterial
{
public:

	CD3DStaticTerrainMaterial(CD3DStaticTerrainRenderer* in_pOwner, const std::wstring& in_wsFileName);

	int	Render(CD3DGraphicsContext* in_pContext);

	// Функция используется в процессе рендеринга [для взаимодействия процесса рендера с Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*, unsigned int in_uiEyeId) override;

	// Получить набор техник
	virtual size_t	GetTechniquesCount() const override { return 0; }
	virtual C3DBaseTechnique*	GetTechniqueById(size_t id) const override { return nullptr; }

	// получить список текстур
	virtual size_t	GetTexturesCount() const override { return 0; }
	virtual C3DBaseTexture*	GetTextureById(size_t id) const override { return nullptr; }

	virtual C3DBaseManager*					GetManager() const;

	void	Load();
	void	Unload();

private:

	CD3DStaticTerrainRenderer*				_pOwner = nullptr;

	std::wstring							_wsTextureFileName;

	std::set<CD3DStaticTerrainFaceset*>		_setVisibleFacesets;

	ID3D11ShaderResourceView*				_pTextureSRV = nullptr;
};



class CD3DStaticTerrainRenderer : public C3DBaseTerrainObjectManager, public CD3DSceneRenderer
{
public:

	enum PSRenderingMode
	{
		STANDARD = 0,
		SHOW_NORMALS,
	};

	CD3DStaticTerrainRenderer();
	~CD3DStaticTerrainRenderer();

	void			Init(CDirect3DSystem* in_pSystem, float in_fWorldScale);

	void			LoadPlanet(const wchar_t* in_pcwszDirectory);

	void			SetLightParameters(const vm::Vector3df& in_vDirection, const vm::Vector3df& in_vDiffuse);

	void			AddVisibleMaterial(CD3DStaticTerrainMaterial*);

	void			SetRenderingMode(PSRenderingMode mode) { _RenderingMode = mode; }
	PSRenderingMode	GetRenderingMode() const { return _RenderingMode; }

	//@{ C3DBaseTerrainObjectManager
	virtual const CTerrainBlockData* GetTerrainDataForObject(C3DBaseObject* pObject) const override;
	virtual const CTerrainBlockData* GetRootTerrainData() const override;
	//@}

	//@{ C3DBaseManager

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource* in_pResource) override;

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource* in_pResource) override;

	//@}

	HeightfieldConverter*	GetHeightfieldConverter() { return _pHeightfieldConverter; }

protected:

	//@{ C3DBaseObjectManager

	// получить список объектов
	virtual size_t GetObjectsCount() const override;
	virtual C3DBaseObject*	GetObjectByIndex(size_t id) const override;

	//@}

	//@{ CD3DSceneRenderer

	virtual int		Render(CD3DGraphicsContext* in_pContext) override;

	virtual float	GetWorldRadius() const;
	virtual float	GetMinCellSize() const;

	//@}

	void			DrawIndexedByShader(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, unsigned int indexCount);

private:

	void			CreateObjects();
	void			CreateObjectsRecursive(const CTerrainBlockData* in_pData);
	void			CreateObject(const CTerrainBlockData* in_pData);

	bool			InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	void			OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename);
	bool			SetShaderParameters(CD3DGraphicsContext* in_pContext, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);
	void			FinalizeShader();

	CTerrainBlockData*						_pPlanetTerrainData = nullptr;
	CTerrainDataManager*					_pTerrainDataManager = nullptr;

	HeightfieldConverter*					_pHeightfieldConverter = nullptr;

	std::set<CD3DStaticTerrainMaterial*>	_setVisibleMaterials;
	std::vector<CD3DStaticTerrainObject*>	_vecTerrainObjects;
	std::list<CD3DStaticTerrainMaterial*>	_lstMaterials;
	std::list<CD3DStaticTerrainFaceset*>	_lstFacesets;
	std::map<C3DBaseObject*, const CTerrainBlockData*>	_mapTerrainDataBlocks;

	vm::Vector3df							_vLightDirection = vm::Vector3df(-1, -1, 0);
	vm::Vector3df							_vLightDiffuse = vm::Vector3df(1, 1, 1);

	PSRenderingMode							_RenderingMode = STANDARD;

	//@{ Rendering fields

	struct MatrixBufferType
	{
		D3DXMATRIX view;
		D3DXMATRIX projection;

		vm::Vector4df	vCamPos;
		vm::Vector4df	vAxisX;
		vm::Vector4df	vAxisY;
		vm::Vector4df	vAxisZ;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		unsigned int mode;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

	ID3D11VertexShader*						_pVertexShader = nullptr;
	ID3D11PixelShader*						_pPixelShader = nullptr;
	ID3D11InputLayout*						_pInputLayout = nullptr;
	ID3D11SamplerState*						_pSampleState = nullptr;
	ID3D11Buffer*							_pMatrixBuffer = nullptr;
	ID3D11Buffer*							_pLightBuffer = nullptr;

	//@}

	friend class CD3DStaticTerrainMaterial;
	friend class CD3DStaticTerrainFaceset;
};