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


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass : public HeightfieldConverterListener
{

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);
	void Shutdown();
	void Render(ID3D11Device* device, ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();


	virtual void 	TriangulationCreated(const STriangulation* in_pTriangulation) override;


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	//bool LoadModel(char*);
	//void ReleaseModel();

	void GenerateHeightfield(SHeightfield& out_Heightfield, float time);

private:

	static void generateHeightfieldThreadFunction(ModelClass* self);

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	//int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	
	std::mutex				m_mutex;
	STriangulation			m_triangulation;
	bool					m_bNeedToRelockBuffers = false;

	HeightfieldConverter*	m_pHeightfieldConverter = nullptr;

	std::thread				m_generateHeightfieldThread;

	bool					m_finished = false;

	unsigned long			m_CurID = 0;

};

#endif
