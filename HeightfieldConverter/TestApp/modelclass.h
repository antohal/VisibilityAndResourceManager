////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "HeightfieldConverter.h"

#include "CDirect2DTextRenderer.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass : public HeightfieldConverterListener
{

public:

	bool Initialize(CDirect2DTextBlock* debugTextBlock, ID3D11Device*, ID3D11DeviceContext*, WCHAR* pcwszTexture, WCHAR* pcwszNromalMap);
	void Shutdown();
	void Render(ID3D11Device* device, ID3D11DeviceContext*);

	void SetDemoMode(bool in_bDemoMode)
	{
		m_bDemoMode = in_bDemoMode;
	}

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetNormalMap();


	virtual void 	TriangulationCreated(const STriangulation* in_pTriangulation) override;


private:

	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTexture();

	void GenerateHeightfield(SHeightfield& out_Heightfield, float time);

private:

	TextureClass*			m_pTexture = nullptr;
	TextureClass*			m_pNormalMap = nullptr;

	CDirect2DTextBlock*		m_pTextBlock = nullptr;
	UINT					m_GeneratedHeightmapsParam = -1;
	UINT					m_TriangulationsTimeParam = -1;

	std::chrono::time_point<std::chrono::steady_clock>  _beginTime;
	std::chrono::time_point<std::chrono::steady_clock>	_previousSecondTime;
	
	STriangulation			m_triangulation;

	HeightfieldConverter*	m_pHeightfieldConverter = nullptr;

	std::thread				m_generateHeightfieldThread;

	bool					m_finished = false;
	bool					m_firstRender = true;

	bool					m_bDemoMode = false;

	unsigned long			m_CurID = 0;
};

#endif
