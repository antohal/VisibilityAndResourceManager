////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
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

	m_triangulation = *in_pTriangulation;
	m_bNeedToRelockBuffers = true;
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, WCHAR* textureFilename)
{
	bool result;


	m_pHeightfieldConverter = new HeightfieldConverter();
	m_pHeightfieldConverter->Init(device, context, SOFTWARE_MODE);

	m_pHeightfieldConverter->RegisterListener(this);

	SHeightfield initialHeightfield;
	GenerateHeightfield(initialHeightfield, 1.5);


	m_pHeightfieldConverter->CreateTriangulationImmediate(&initialHeightfield, &m_triangulation);

	// Load in the model data,
	/*result = LoadModel(modelFilename);
	if(!result)
	{
		return false;
	}*/

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	m_generateHeightfieldThread = std::thread(generateHeightfieldThreadFunction, this);

	return true;
}


void ModelClass::Shutdown()
{
	m_finished = true;

	m_generateHeightfieldThread.join();

	delete m_pHeightfieldConverter;

	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	//ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_mutex.lock();
	if (m_bNeedToRelockBuffers)
	{
		ShutdownBuffers();
		InitializeBuffers(device);

		m_bNeedToRelockBuffers = false;
	}
	m_mutex.unlock();

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_triangulation.vecIndexData.size();
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	if (m_triangulation.vecIndexData.size() == 0)
		return false;

	if (m_triangulation.vecVertexData.size() == 0)
		return false;


	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(SVertex) * m_triangulation.vecVertexData.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = &m_triangulation.vecVertexData[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}


	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_triangulation.vecIndexData.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = &m_triangulation.vecIndexData[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(SVertex); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

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

	out_Heightfield.fSizeX = 6;
	out_Heightfield.fSizeY = 6;
	out_Heightfield.fMinHeight = 0;
	out_Heightfield.fMaxHeight = 1;

	out_Heightfield.nCountX = 256;
	out_Heightfield.nCountY = 256;

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

//
//bool ModelClass::LoadModel(char* filename)
//{
//	ifstream fin;
//	char input;
//	int i;
//
//
//	// Open the model file.
//	fin.open(filename);
//	
//	// If it could not open the file then exit.
//	if(fin.fail())
//	{
//		return false;
//	}
//
//	// Read up to the value of vertex count.
//	fin.get(input);
//	while(input != ':')
//	{
//		fin.get(input);
//	}
//
//	// Read in the vertex count.
//	fin >> m_vertexCount;
//
//	// Set the number of indices to be the same as the vertex count.
//	m_indexCount = m_vertexCount;
//
//	// Create the model using the vertex count that was read in.
//	m_model = new ModelType[m_vertexCount];
//	if(!m_model)
//	{
//		return false;
//	}
//
//	// Read up to the beginning of the data.
//	fin.get(input);
//	while(input != ':')
//	{
//		fin.get(input);
//	}
//	fin.get(input);
//	fin.get(input);
//
//	// Read in the vertex data.
//	for(i=0; i<m_vertexCount; i++)
//	{
//		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
//		fin >> m_model[i].tu >> m_model[i].tv;
//		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
//	}
//
//	// Close the model file.
//	fin.close();
//
//	return true;
//}
//
//
//void ModelClass::ReleaseModel()
//{
//	if(m_model)
//	{
//		delete [] m_model;
//		m_model = 0;
//	}
//
//	return;
//}