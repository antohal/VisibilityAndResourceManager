////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_Texture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

void ModelClass::generateHeightfieldThreadFunction(ModelClass* self)
{

	float curTime = 0;

	SHeightfield hf;

	while (!self->m_finished)
	{
		curTime += 0.01f;

		self->GenerateHeightfield(hf, curTime);

		hf.ID = self->m_CurID;

		self->m_CurID++;

		self->m_pHeightfieldConverter->AppendTriangulationTask(&hf);
	}
}

void ModelClass::TriangulationCreated(const STriangulation* in_pTriangulation)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_triangulation.ReleaseBuffers();

	m_triangulation = *in_pTriangulation;
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, WCHAR* textureFilename)
{
	bool result;


	m_pHeightfieldConverter = new HeightfieldConverter();
	m_pHeightfieldConverter->Init(device, context, SOFTWARE_MODE /*DIRECT_COMPUTE_MODE*/);

	m_pHeightfieldConverter->RegisterListener(this);

	SHeightfield initialHeightfield;
	GenerateHeightfield(initialHeightfield, 1.5);


	m_pHeightfieldConverter->CreateTriangulationImmediate(&initialHeightfield, &m_triangulation);

	unsigned int * pIndices = new unsigned int[m_triangulation.nIndexCount];
	SVertex* pVertices = new SVertex[m_triangulation.nVertexCount];

	m_triangulation.UnmapBuffers(device, context, pVertices, pIndices);

	delete[] pIndices;
	delete[] pVertices;

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	//m_generateHeightfieldThread = std::thread(generateHeightfieldThreadFunction, this);

	return true;
}


void ModelClass::Shutdown()
{
	m_finished = true;

	//m_generateHeightfieldThread.join();

	delete m_pHeightfieldConverter;

	// Release the model texture.
	ReleaseTexture();


	// Release the model data.
	//ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_triangulation.nIndexCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}



void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(SVertex); 
	offset = 0;

	m_mutex.lock();

	ID3D11Buffer* pVertexBuffer = m_triangulation.pVertexBuffer;
	ID3D11Buffer* pIndexBuffer = m_triangulation.pIndexBuffer;

	if (pVertexBuffer && pIndexBuffer)
	{
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	}

	m_mutex.unlock();

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
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

	out_Heightfield.nCountX = 128;
	out_Heightfield.nCountY = 128;

	out_Heightfield.vecData.resize(out_Heightfield.nCountX * out_Heightfield.nCountY);

	for (unsigned int lx = 0; lx < out_Heightfield.nCountX; lx++)
	{

		for (unsigned int ly = 0; ly < out_Heightfield.nCountY; ly++)
		{
			size_t idx = lx + ly*out_Heightfield.nCountX;

			float k = 1 + 3 * sin(time);

			float cx = k * (float)lx / out_Heightfield.nCountX;
			float cy = k * (float)ly / out_Heightfield.nCountY;

			float val = 0.5f + 0.5f*sin(cx * cy);

			out_Heightfield.vecData[idx] = static_cast<unsigned char>(val * 255);
		}

	}
}
