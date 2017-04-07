////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


void ModelClass::TriangulationCreated(const STriangulation* in_pTriangulation)
{
	// освобождаем буферы старой триангул€ции
	m_triangulation.ReleaseBuffers();

	m_triangulation = *in_pTriangulation;
}

bool ModelClass::Initialize(CDirect2DTextBlock* debugTextBlock, ID3D11Device* device, ID3D11DeviceContext* context, WCHAR* pcwszTexture, WCHAR* pcwszNromalMap)
{
	bool result;

	m_pTextBlock = debugTextBlock;

	m_pHeightfieldConverter = new HeightfieldConverter();
	//m_pHeightfieldConverter->Init(device, context, SOFTWARE_MODE);
	m_pHeightfieldConverter->Init(device, context, DIRECT_COMPUTE_MODE);

	m_pHeightfieldConverter->RegisterListener(this);

	if (m_pTextBlock)
	{
		m_GeneratedHeightmapsParam = m_pTextBlock->AddParameter(L"¬рем€ генерации карты высот (мс)");
		m_TriangulationsTimeParam = m_pTextBlock->AddParameter(L"¬рем€ триангул€ции (мс)");

		m_pTextBlock->AddTextLine(L"–азрешение карт высот: 1024 х 1024");
	}

	// Load the texture for this model.
	result = LoadTextures(device, pcwszTexture, pcwszNromalMap);
	if(!result)
	{
		return false;
	}

	//m_generateHeightfieldThread = std::thread(generateHeightfieldThreadFunction, this);

	_beginTime = _previousSecondTime = std::chrono::high_resolution_clock::now();

	return true;
}


void ModelClass::Shutdown()
{
	m_finished = true;

	// release current rendering triangulation buffers
	m_triangulation.ReleaseBuffers();

	// wait for generation thread stops
	//m_generateHeightfieldThread.join();

	delete m_pHeightfieldConverter;

	// Release the model texture.
	ReleaseTexture();

	return;
}


void ModelClass::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{

	std::chrono::time_point<std::chrono::steady_clock> thisFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = thisFrameTime - _beginTime;
	
	double time = elapsed.count() / 1000.0;

	SHeightfield initialHeightfield;
	GenerateHeightfield(initialHeightfield, (float)time);

	std::chrono::time_point<std::chrono::steady_clock> afterGenerationTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> generationDelta = afterGenerationTime - thisFrameTime;

	m_triangulation.ReleaseBuffers();
	m_pHeightfieldConverter->CreateTriangulationImmediate(&initialHeightfield, &m_triangulation);


	std::chrono::time_point<std::chrono::steady_clock> afterTriangulationTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> triangulationDelta = afterTriangulationTime - afterGenerationTime;

	if (m_pTextBlock)
	{
		m_pTextBlock->SetParameterValue(m_GeneratedHeightmapsParam, generationDelta.count());
		m_pTextBlock->SetParameterValue(m_TriangulationsTimeParam, triangulationDelta.count());
	}


	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_triangulation.nIndexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_pTexture->GetTexture();
}

ID3D11ShaderResourceView* ModelClass::GetNormalMap()
{
	return m_pNormalMap->GetTexture();
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(SVertex); 
	offset = 0;


	ID3D11Buffer* pVertexBuffer = m_triangulation.pVertexBuffer;
	ID3D11Buffer* pIndexBuffer = m_triangulation.pIndexBuffer;

	if (pVertexBuffer && pIndexBuffer)
	{
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	}

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* filename, WCHAR* normalmap)
{
	bool result;


	// Create the texture object.
	m_pTexture = new TextureClass;
	
	// Initialize the texture object.
	result = m_pTexture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	m_pNormalMap = new TextureClass;
	m_pNormalMap->Initialize(device, normalmap);

	return true;
}


void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_pTexture)
	{
		m_pTexture->Shutdown();
		delete m_pTexture;
		m_pTexture = 0;
	}

	if (m_pNormalMap)
	{
		m_pNormalMap->Shutdown();
		delete m_pNormalMap;
		m_pNormalMap = 0;
	}

	return;
}

void ModelClass::GenerateHeightfield(SHeightfield & out_Heightfield, float time)
{
	out_Heightfield.ID = 0;

	out_Heightfield.fSizeX = 8;
	out_Heightfield.fSizeY = 8;
	out_Heightfield.fMinHeight = 0;
	out_Heightfield.fMaxHeight = 1;

	out_Heightfield.nCountX = 1024;
	out_Heightfield.nCountY = 1024;

	out_Heightfield.vecData.resize(out_Heightfield.nCountX * out_Heightfield.nCountY);

	for (unsigned int lx = 0; lx < out_Heightfield.nCountX; lx++)
	{

		for (unsigned int ly = 0; ly < out_Heightfield.nCountY; ly++)
		{
			size_t idx = lx + ly*out_Heightfield.nCountX;

			float k = 1 + 5 * sin(time);

			float cx = k * (float)lx / out_Heightfield.nCountX;
			float cy = k * (float)ly / out_Heightfield.nCountY;

			float val = 0.5f + 0.5f*sin(cx * cy);

			out_Heightfield.vecData[idx] = static_cast<unsigned char>(val * 255);
		}

	}
}
