#pragma once

#include <functional>
#include <chrono>
#include <thread>
#include <map>
#include <mutex>
#include <string>
#include <list>

#include <d3d11.h>

class CTextureLoadQueue
{
public:
	typedef std::function<void(size_t, ID3D11ShaderResourceView*)> FinishedLoadHandler;
	typedef std::function<float(size_t)> SortQueueHandler;

	CTextureLoadQueue(ID3D11Device* device, const FinishedLoadHandler& finishedHandler, const SortQueueHandler& sortHandler);
	~CTextureLoadQueue();

	//@{ This functions must be called from one thread
	void AddToLoad(size_t ID, const std::wstring& wsTextureFileName);
	void Remove(size_t ID);
	void Process();
	void Sort();
	//@}

	int	 CountInQueue() const;

private:

	void StartTextureLoadThreadFunc();
	bool UpdateTextureLoad();

	struct SLoadRequest
	{
		size_t			_object = -1;
		std::wstring	_textureName;
		float			_sortValue = 0;

		SLoadRequest() {}
		SLoadRequest(size_t obj, const std::wstring& wsTextureFileName) : _object(obj), _textureName(wsTextureFileName) {}
	};

	struct SAsyncTexture
	{
		int		_refCount = 0;
		bool	_loaded = false;
		ID3D11ShaderResourceView*	_pResView = nullptr;
	};

	ID3D11Device*				_device = nullptr;
	FinishedLoadHandler			_finishHandler;
	SortQueueHandler			_sortHandler;

	std::thread*				_pTextureLoadThread = nullptr;
	bool						_bTexturesThreadFinished = false;

	mutable std::mutex			_queueMutex;
	mutable std::mutex			_justLoadedMutex;

	std::list<SLoadRequest>		_lstLoadQueue;
	std::map<size_t, SAsyncTexture>	_mapAsyncTextures;
	std::list<std::pair<size_t, ID3D11ShaderResourceView*>> _lstJustLoaded;
};
