#include "SoftwareConverter.h"
#include "vecmath.h"
#include <chrono>

using namespace std::chrono_literals;

SoftwareHeightfieldConverter::SoftwareHeightfieldConverter(ID3D11Device* in_pD3DDevice11)
{
	_triangulationThread = std::thread(triangulationThreadFunc, this);

	_ptrD3DDevice11 = in_pD3DDevice11;
}

SoftwareHeightfieldConverter::~SoftwareHeightfieldConverter()
{
	setThreadFinished();
	_triangulationThread.join();
}

// Создать триангуляцию немедленно и дождаться готовности
void SoftwareHeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	STriangulationTask task(this, *in_pHeightfield);
	task.createTriangulation();
	*out_pTriangulation = task._triangulation;
}

// добавить/удалить listener
void SoftwareHeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
	_setListeners.insert(in_pListener);
}

void SoftwareHeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
	_setListeners.erase(in_pListener);
}

void SoftwareHeightfieldConverter::triangulationThreadFunc(SoftwareHeightfieldConverter* self)
{
	while (!self->threadFinished())
	{
		if (!self->processTriangulations())
			std::this_thread::sleep_for(1ns);
	}
}

bool SoftwareHeightfieldConverter::processTriangulations()
{
	_triangulationsMutex.lock();

	if (_setTriangulationTasks.empty())
	{
		_triangulationsMutex.unlock();
		return false;
	}

	STriangulationTask* task = *_setTriangulationTasks.begin();

	_setTriangulationTasks.erase(task);

	_triangulationsMutex.unlock();

	task->createTriangulation();

	_finishedTasksMutex.lock();
	_lstFinishedTasks.push_back(task);
	_finishedTasksMutex.unlock();


	return true;
}

// добавить задачу на триангуляцию, которая будет выполняться асинхронно
void SoftwareHeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	std::lock_guard<std::mutex> lock(_triangulationsMutex);
	//_qTriangulationTasks.push(STriangulationTask(this, *in_pHeightfield));

	_setTriangulationTasks.insert(new STriangulationTask(this, *in_pHeightfield));
}

void SoftwareHeightfieldConverter::UpdateTasks()
{
	_finishedTasksMutex.lock();

	for (STriangulationTask* task : _lstFinishedTasks)
	{
		for (HeightfieldConverterListener* listener : _setListeners)
			listener->TriangulationCreated(&task->_triangulation);

		delete task;
	}

	_lstFinishedTasks.clear();

	_finishedTasksMutex.unlock();

}

void SoftwareHeightfieldConverter::STriangulationTask::createTriangulation()
{
	_triangulation.ID = _heightfield.ID;

	if (_heightfield.nCountX <= 1 || _heightfield.nCountY <= 1)
	{
		// TODO: log

		return;
	}

	for (unsigned int ly = 0; ly < _heightfield.nCountY - 1; ly++)
	{
		for (unsigned int lx = 0; lx < _heightfield.nCountX - 1; lx++)
		{
			unsigned int i0 = ly*_heightfield.nCountX + lx;
			unsigned int i1 = ly*_heightfield.nCountX + lx + 1;
			unsigned int i2 = (ly + 1)*_heightfield.nCountX + lx;
			unsigned int i3 = (ly + 1)*_heightfield.nCountX + lx + 1;

			_vecIndexData.push_back(i3);
			_vecIndexData.push_back(i1);
			_vecIndexData.push_back(i0);

			_vecIndexData.push_back(i2);
			_vecIndexData.push_back(i3);
			_vecIndexData.push_back(i0);
		}
	}

	float dx = _heightfield.fSizeX / (_heightfield.nCountX - 1);
	float dy = _heightfield.fSizeY / (_heightfield.nCountY - 1);

	_vecVertexData.reserve(_heightfield.nCountX * _heightfield.nCountY);

	for (unsigned int i = 0; i < _heightfield.vecData.size(); i++)
	{
		unsigned int ly = i / _heightfield.nCountX;
		unsigned int lx = i - ly * _heightfield.nCountX;
		
		SVertex vtx;
		vtx.position.y = _heightfield.fMinHeight + (_heightfield.fMaxHeight - _heightfield.fMinHeight) * ((float)_heightfield.vecData[i] / 255.f);
		vtx.position.x = lx * dx - _heightfield.fSizeX * 0.5;
		vtx.position.z = ly * dy - _heightfield.fSizeX * 0.5;

		vtx.texture.x = (float)lx / (_heightfield.nCountX - 1);
		vtx.texture.y = (float)ly / (_heightfield.nCountY - 1);

		vtx.normal.x = 0;
		vtx.normal.y = 1;
		vtx.normal.z = 0;

		vtx.binormal.x = 1;
		vtx.binormal.y = 0;
		vtx.binormal.z = 0;

		_vecVertexData.push_back(vtx);
	}

	_triangulation.nIndexCount = _vecIndexData.size();
	_triangulation.nVertexCount = _vecVertexData.size();

	createBuffers();
}


void SoftwareHeightfieldConverter::STriangulationTask::createBuffers()
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	if (_vecIndexData.size() == 0)
		return;

	if (_vecVertexData.size() == 0)
		return;


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SVertex) * _vecVertexData.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &_vecVertexData[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = _owner->_ptrD3DDevice11->CreateBuffer(&vertexBufferDesc, &vertexData, &_triangulation.pVertexBuffer);
	if (FAILED(result))
	{
		return;
	}


	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _vecIndexData.size();
	indexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = &_vecIndexData[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = _owner->_ptrD3DDevice11->CreateBuffer(&indexBufferDesc, &indexData, &_triangulation.pIndexBuffer);
	if (FAILED(result))
	{
		return;
	}
}
