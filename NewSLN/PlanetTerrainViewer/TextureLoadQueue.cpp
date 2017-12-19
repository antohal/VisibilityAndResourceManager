#include "TextureLoadQueue.h"
#include "Log.h"

#include <d3dx11async.h>
#include <algorithm>

CTextureLoadQueue::CTextureLoadQueue(ID3D11Device* device, const FinishedLoadHandler& finishedHandler)
	:	_device (device), _handler(finishedHandler)
{
	StartTextureLoadThreadFunc();
}

CTextureLoadQueue::~CTextureLoadQueue()
{
	// delete texture load thread
	if (_pTextureLoadThread)
	{
		_bTexturesThreadFinished = true;

		if (_pTextureLoadThread->joinable() && _pTextureLoadThread->native_handle())
		{
			_pTextureLoadThread->join();
		}

		delete _pTextureLoadThread;

		_pTextureLoadThread = nullptr;
	}
}

void CTextureLoadQueue::AddToLoad(size_t ID, const std::wstring& wsTextureFileName)
{
	auto it = _mapAsyncTextures.find(ID);

	if (it != _mapAsyncTextures.end())
	{
		SAsyncTexture& asyncTex = it->second;

		asyncTex._refCount++;

		if (asyncTex._loaded)
		{
			_handler(ID, asyncTex._pResView);
		}
	}
	else
	{
		SAsyncTexture& asyncTex = _mapAsyncTextures[ID];
		asyncTex._refCount = 1;

		std::lock_guard<std::mutex> lg(_queueMutex);

		auto itQueue = std::find_if(_lstLoadQueue.begin(), _lstLoadQueue.end(), [&](const SLoadRequest& lr) -> bool { return lr._object == ID; });
		if (itQueue == _lstLoadQueue.end())
			_lstLoadQueue.push_back(SLoadRequest(ID, wsTextureFileName));
	}
}

int	 CTextureLoadQueue::CountInQueue() const
{
	std::lock_guard<std::mutex> lg(_queueMutex);
	return (int)_lstLoadQueue.size();
}

void CTextureLoadQueue::Remove(size_t ID)
{
	auto it = _mapAsyncTextures.find(ID);

	if (it != _mapAsyncTextures.end())
	{
		SAsyncTexture& asyncTex = it->second;
		asyncTex._refCount--;

		if (asyncTex._refCount <= 0)
		{
			if (asyncTex._loaded && asyncTex._pResView)
			{
				asyncTex._pResView->Release();
			}

			_mapAsyncTextures.erase(it);


			std::lock_guard<std::mutex> lg(_queueMutex);

			auto itQueue = std::find_if(_lstLoadQueue.begin(), _lstLoadQueue.end(), [&](const SLoadRequest& lr) -> bool { return lr._object == ID; });
			if (itQueue != _lstLoadQueue.end())
			{
				_lstLoadQueue.erase(itQueue);
			}
		}
	}
	else
	{
		LogMessage("Error: CTextureLoadQueue::Remove, it == _mapAsyncTextures.end() - illegal");
	}
}

void CTextureLoadQueue::Process()
{
	{
		std::lock_guard<std::mutex> lg(_justLoadedMutex);

		for (auto pt : _lstJustLoaded)
		{
			SAsyncTexture& asyncTex = _mapAsyncTextures[pt.first];

			asyncTex._loaded = true;
			asyncTex._pResView = pt.second;

			_handler(pt.first, asyncTex._pResView);
		}

		_lstJustLoaded.clear();
	}
}

void CTextureLoadQueue::StartTextureLoadThreadFunc()
{
	_pTextureLoadThread = new std::thread([this]() {

		while (!_bTexturesThreadFinished)
		{
			if (!UpdateTextureLoad())
			{
				std::chrono::milliseconds dur(1);
				std::this_thread::sleep_for(dur);
			}
		}

	});
}

bool CTextureLoadQueue::UpdateTextureLoad()
{
	SLoadRequest currentLoadRequest;

	{
		std::lock_guard<std::mutex> lg(_queueMutex);

		if (!_lstLoadQueue.empty())
		{
			currentLoadRequest = _lstLoadQueue.front();
			_lstLoadQueue.pop_front();
		}
	}

	if (currentLoadRequest._object == -1)
		return false;

	LogMessage("Loading texture '%ls'", currentLoadRequest._textureName.c_str());


	ID3D11ShaderResourceView* pResourceViewTex = nullptr;
	D3DX11CreateShaderResourceViewFromFileW(_device, currentLoadRequest._textureName.c_str(), NULL, NULL, &pResourceViewTex, NULL);
	
#ifdef _DEBUG
	//@{ TODO: DEBUG
	std::chrono::milliseconds dur(50);
	std::this_thread::sleep_for(dur);
	//@}
#endif

	{
		std::lock_guard<std::mutex> lg(_justLoadedMutex);

		std::pair<size_t, ID3D11ShaderResourceView*> ptLoadedTex;
		ptLoadedTex.first = currentLoadRequest._object;
		ptLoadedTex.second = pResourceViewTex;

		_lstJustLoaded.push_back(ptLoadedTex);
	}


	return true;
}
