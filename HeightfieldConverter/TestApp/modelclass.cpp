////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"
#include "vecmath.h"

void ModelClass::TriangulationCreated(const STriangulation* in_pTriangulation)
{
	// ����������� ������ ������ ������������
	m_pHeightfieldConverter->ReleaseTriangulation(&m_triangulation);

	m_triangulation = *in_pTriangulation;
}


bool ModelClass::Initialize(CDirect2DTextBlock* debugTextBlock, ID3D11Device* device, ID3D11DeviceContext* context, WCHAR* pcwszTexture, WCHAR* pcwszNromalMap)
{
	bool result;

	m_pTextBlock = debugTextBlock;

	m_pHeightfieldConverter = new HeightfieldConverter();
	m_pHeightfieldConverter->Init(device, context);

	m_pHeightfieldConverter->RegisterListener(this);

	if (m_pTextBlock)
	{
		m_GeneratedHeightmapsParam = m_pTextBlock->AddParameter(L"����� ��������� ����� ����� (��)");
		m_TriangulationsTimeParam = m_pTextBlock->AddParameter(L"����� ������������ (��)");

		m_pTextBlock->AddTextLine(L"���������� ���� �����: 1024 � 1024");
	}

	// Load the texture for this model.
	result = LoadTextures(device, pcwszTexture, pcwszNromalMap);
	if(!result)
	{
		return false;
	}

	SHeightfield testHeightfield;

	m_pHeightfieldConverter->ReadHeightfieldDataFromTexture(L"TestData/Heightmap.dds", testHeightfield);

	testHeightfield.ID = 0;
	testHeightfield.Config.fMinHeight = 0;
	testHeightfield.Config.fMaxHeight = 1;

	/*testHeightfield.Config.fMinLattitude = 30*D2R;
	testHeightfield.Config.fMaxLattitude = 40*D2R;
	testHeightfield.Config.fMinLongitude = 40*D2R;
	testHeightfield.Config.fMaxLongitude = 50*D2R;*/

	testHeightfield.Config.fMinLattitude = -4;
	testHeightfield.Config.fMaxLattitude = 4;
	testHeightfield.Config.fMinLongitude = -4;
	testHeightfield.Config.fMaxLongitude = 4;

	m_pHeightfieldConverter->CreateTriangulationImmediate(&testHeightfield, &m_triangulation);

	SVertex* pVertices = new SVertex[m_triangulation.nVertexCount];
	unsigned int* pIndices = new unsigned int[m_triangulation.nIndexCount];

	m_pHeightfieldConverter->UnmapTriangulation(&m_triangulation, pVertices, pIndices);


	m_pHeightfieldConverter->ReleaseHeightfield(&testHeightfield);

	//m_generateHeightfieldThread = std::thread(generateHeightfieldThreadFunction, this);

	_beginTime = _previousSecondTime = std::chrono::high_resolution_clock::now();

	return true;
}


void ModelClass::Shutdown()
{
	m_finished = true;

	// release current rendering triangulation buffers
	m_pHeightfieldConverter->ReleaseTriangulation(&m_triangulation);

	// wait for generation thread stops
	//m_generateHeightfieldThread.join();

	delete m_pHeightfieldConverter;

	// Release the model texture.
	ReleaseTexture();

	return;
}


void ModelClass::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if (m_bDemoMode)
	{
		std::chrono::time_point<std::chrono::steady_clock> thisFrameTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> elapsed = thisFrameTime - _beginTime;

		double time = elapsed.count() / 1000.0;

		SHeightfield heightfield;
		GenerateHeightfield(heightfield, (float)time);

		std::chrono::time_point<std::chrono::steady_clock> afterGenerationTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> generationDelta = afterGenerationTime - thisFrameTime;

		m_pHeightfieldConverter->ReleaseTriangulation(&m_triangulation);

		m_pHeightfieldConverter->CreateTriangulationImmediate(&heightfield, &m_triangulation);

		m_pHeightfieldConverter->ReleaseHeightfield(&heightfield);


		std::chrono::time_point<std::chrono::steady_clock> afterTriangulationTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> triangulationDelta = afterTriangulationTime - afterGenerationTime;

		if (m_pTextBlock)
		{
			m_pTextBlock->SetParameterValue(m_GeneratedHeightmapsParam, generationDelta.count());
			m_pTextBlock->SetParameterValue(m_TriangulationsTimeParam, triangulationDelta.count());
		}
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
	const unsigned int c_nWidth = 1024, c_nHeight = 1024;

	out_Heightfield.ID = 0;

	out_Heightfield.Config.fMinLattitude = 30;
	out_Heightfield.Config.fMaxLattitude = 60;

	out_Heightfield.Config.fMinLongitude = 30;
	out_Heightfield.Config.fMaxLongitude = 60;

	out_Heightfield.Config.fMinHeight = 0;
	out_Heightfield.Config.fMaxHeight = 1;

	out_Heightfield.Config.nCountX = c_nWidth;
	out_Heightfield.Config.nCountY = c_nHeight;

	std::vector<unsigned char> vecData;
	vecData.resize(out_Heightfield.Config.nCountX * out_Heightfield.Config.nCountY);

	for (unsigned int lx = 0; lx < out_Heightfield.Config.nCountX; lx++)
	{

		for (unsigned int ly = 0; ly < out_Heightfield.Config.nCountY; ly++)
		{
			size_t idx = lx + ly*out_Heightfield.Config.nCountX;

			float k = 1 + 5 * sin(time);

			float cx = k * (float)lx / out_Heightfield.Config.nCountX;
			float cy = k * (float)ly / out_Heightfield.Config.nCountY;

			float val = 0.5f + 0.5f*sin(cx * cy);

			vecData[idx] = static_cast<unsigned char>(val * 255);
		}
	}

	m_pHeightfieldConverter->ReadHeightfieldDataFromMemory(&vecData[0], c_nWidth, c_nHeight, out_Heightfield);
}
