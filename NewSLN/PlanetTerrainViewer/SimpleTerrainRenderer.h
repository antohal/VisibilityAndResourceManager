#pragma once

#include "Scene.h"
#include "TerrainManager.h"
#include "HeightfieldConverter.h"

#include <map>
#include <mutex>
#include <thread>
#include <d3dx11effect.h>

class CSimpleTerrainRenderer;

class CSimpleTerrainRenderObject
{
protected:

	// ������� ������ ��� ������� ����� �� ����������
	CSimpleTerrainRenderObject(CSimpleTerrainRenderer* in_pRenderer, const STerrainBlockParams* in_pParams, TerrainObjectID ID);

	~CSimpleTerrainRenderObject();

	void SetIndexAndVertexBuffers(CD3DGraphicsContext* in_pContext);
	unsigned int GetIndexCount() const;

	ID3D11ShaderResourceView* GetTextureResourceView() { return _pTextureSRV; }

	
private:

	TerrainObjectID					_ID;

	std::wstring					_wsTextureFileName;

	std::thread*					_pLoadThread = nullptr;
	ID3D11ShaderResourceView*		_pTextureSRV = nullptr;

	CSimpleTerrainRenderer*			_owner = nullptr;

	friend class CSimpleTerrainRenderer;
};

class CSimpleTerrainRenderer : public CD3DSceneRenderer
{
public:

	enum PSRenderingMode
	{
		STANDARD = 0,
		SHOW_NORMALS,
		GREY,
		WITHOUT_LIGHTING
	};

	~CSimpleTerrainRenderer();

	void							Init(CTerrainManager* in_pTerrainManager, float in_fWorldScale);

	CSimpleTerrainRenderObject*		CreateObject(TerrainObjectID);
	void							DeleteObject(TerrainObjectID);

	void							AddObjectToRenderQueue(TerrainObjectID);

	void							SetDebugTextBlock(CDirect2DTextBlock* block);

	//@{ CD3DSceneRenderer

	virtual int						Render(CD3DGraphicsContext* in_pContext) override;
	virtual int						GetVisibleObjectsCount() const override;
	//@}

	void							SetRenderingMode(PSRenderingMode mode) { _RenderingMode = mode; }
	PSRenderingMode					GetRenderingMode() const { return _RenderingMode; }

	void							SetLightParameters(const vm::Vector3df& in_vDirection, const vm::Vector3df& in_vDiffuse);
	
	CTerrainManager*				GetTerrainManager() { return _pTerrainManager; };

	HeightfieldConverter*			GetHeightfieldConverter() { return _pHeightfieldConverter; }

	void							StartUpdateTriangulationsThread();


	virtual void					LockDeviceContext() override;
	virtual void					UnlockDeviceContext() override;


	void							LockLoadMutex();
	void							UnlockLoadMutex();

private:

	bool InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename);
	void FinalizeShader();

	bool SetShaderParameters(CD3DGraphicsContext* in_pContext, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);
	void DrawIndexedByShader(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, unsigned int indexCount);

	PSRenderingMode					_RenderingMode = PSRenderingMode::STANDARD;

	vm::Vector3df					_vLightDirection = vm::Vector3df(-1, -1, 0);
	vm::Vector3df					_vLightDiffuse = vm::Vector3df(1, 1, 1);

	//@{ Rendering fields

	struct MatrixBufferType
	{
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		unsigned int mode;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

	ID3D11VertexShader*				_pVertexShader = nullptr;
	ID3D11PixelShader*				_pPixelShader = nullptr;
	ID3D11InputLayout*				_pInputLayout = nullptr;
	ID3D11SamplerState*				_pSampleState = nullptr;
	ID3D11Buffer*					_pMatrixBuffer = nullptr;
	ID3D11Buffer*					_pLightBuffer = nullptr;

	ID3DX11Effect*					_pD3DXEffect = nullptr;

	//@}

	CTerrainManager*				_pTerrainManager = nullptr;

	HeightfieldConverter*			_pHeightfieldConverter = nullptr;

	CDirect2DTextBlock*				_pTextBlock = nullptr;

	UINT							_uiTriangulationsCountParam = -1;
	UINT							_uiHeightfieldsCountParam = -1;

	std::thread*					_pTriangulationsThread = nullptr;
	bool							_bTriangulationThreadFinished = false;

	std::map<TerrainObjectID, CSimpleTerrainRenderObject*>	_mapTerrainRenderObjects;

	int								_visibleObjsCount = 0;
	std::list<TerrainObjectID>		_lstRenderQueue;

	std::mutex						_loadMutex;
};