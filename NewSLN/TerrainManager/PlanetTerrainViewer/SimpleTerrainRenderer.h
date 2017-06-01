#pragma once

#include "Scene.h"
#include "TerrainObjectManager.h"
#include "HeightfieldConverter.h"

#include <map>

class CSimpleTerrainRenderer;

class CSimpleTerrainRenderObject
{
protected:

	CSimpleTerrainRenderObject(CSimpleTerrainRenderer*, const CTerrainBlockDesc*);


private:

	SHeightfield					_heightfield;
	STriangulation					_triangulation;

	const CTerrainBlockDesc*		_pTerrainBlockData = nullptr;
	
	std::wstring					_wsTextureFileName;
	ID3D11ShaderResourceView*		_pTextureSRV = nullptr;

	CSimpleTerrainRenderer*			_owner = nullptr;

	friend class CSimpleTerrainRenderer;
};

class CSimpleTerrainRenderer : public CD3DSceneRenderer
{
public:

	enum class PSRenderingMode
	{
		STANDARD = 0,
		SHOW_NORMALS,
	};

	void							Init(CTerrainObjectManager* in_pTerrainObjectManager);

	CSimpleTerrainRenderObject*		CreateObject(TerrainObjectID);
	void							DeleteObject(TerrainObjectID);

	//@{ CD3DSceneRenderer

	virtual int						Render(CD3DGraphicsContext* in_pContext) override;

	//@}

	void							SetRenderingMode(PSRenderingMode mode) { _RenderingMode = mode; }
	PSRenderingMode					GetRenderingMode() const { return _RenderingMode; }

	void							SetLightParameters(const vm::Vector3df& in_vDirection, const vm::Vector3df& in_vDiffuse);


private:

	PSRenderingMode					_RenderingMode = PSRenderingMode::STANDARD;

	vm::Vector3df					_vLightDirection = vm::Vector3df(-1, -1, 0);
	vm::Vector3df					_vLightDiffuse = vm::Vector3df(1, 1, 1);

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

	ID3D11VertexShader*				_pVertexShader = nullptr;
	ID3D11PixelShader*				_pPixelShader = nullptr;
	ID3D11InputLayout*				_pInputLayout = nullptr;
	ID3D11SamplerState*				_pSampleState = nullptr;
	ID3D11Buffer*					_pMatrixBuffer = nullptr;
	ID3D11Buffer*					_pLightBuffer = nullptr;

	//@}

	CTerrainObjectManager*			_pTerrainObjectManager = nullptr;

	std::map<TerrainObjectID, CSimpleTerrainRenderObject*>	_mapTerrainRenderObjects;
};