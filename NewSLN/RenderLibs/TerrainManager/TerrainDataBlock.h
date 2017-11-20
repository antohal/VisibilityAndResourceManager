#pragma once

#include "TerrainDataManager.h"
#include "TerrainDataManagerImpl.h"

#include <string>
#include <vector>

#include <atlbase.h>

class CTerrainBlockDesc::CTerrainBlockDescImplementation
{
public:

	CTerrainBlockDescImplementation(CTerrainBlockDesc* in_pHolder);
	~CTerrainBlockDescImplementation();

	// �������������
	void									Init(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner, float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude, CTerrainBlockDesc* in_pParent);

	const STerrainBlockParams*				GetParams() const { return &_params; }

	// �������� ��� ����� ��������
	const wchar_t*							GetTextureFileName() const;

	// �������� ��� ����� ����� �����
	const wchar_t*							GetHeightmapFileName() const;

	// �������� ��������� �� ������������ ���� (���������� null, ���� �������� ��������)
	const CTerrainBlockDesc*				GetParentBlockData() const;

	// �������� ���������� �������� ������
	unsigned int							GetChildBlockDataCount() const;

	// �������� ��������� �� �������� ����
	const CTerrainBlockDesc*				GetChildBlockData(unsigned int id) const;

	// ��������� �������� ����� �� ��������� ����������
	void									LoadChildsFromDirectory(const std::wstring& in_wsDirectory);

	// ������������� �������� ���� ��������� �������
	void									GenerateChilds(const wchar_t* in_pcwszDirectoryName, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth, const std::vector<std::wstring>& vecTextures, const std::vector<std::wstring>& vecHeightmaps);


	void									ReadDataStructure(const std::wstring in_wsParentDir, const LodInfoStruct * in_pLodInfoArray, unsigned int in_uiMaxDepth);

	unsigned int							Depth() const { return _params.uiDepth; }

	// �������� ���������� [�����������]
	static CTerrainBlockDesc*				CreateTerrainBlockDataInstance(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner,
		float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude, CTerrainBlockDesc* in_pParent);

	size_t									GetMemoryUsage() const;

	const CTerrainBlockDesc*				GetNeighbour(unsigned int id) const
	{
		if (id >= 8)
			return nullptr;

		return _neighbours[id];
	}

private:

	STerrainBlockParams						_params;

	std::wstring							_wsTextureFileName;
	std::wstring							_wsHeightmapFileName;

	CTerrainBlockDesc*						_pParentBlock = nullptr;
	std::vector<CTerrainBlockDesc*>			_vecChildBlocks;

	CTerrainBlockDesc*						_neighbours[8];

	CTerrainDataManager::CTerrainDataManagerImplementation*		_pOwner = nullptr;
	CTerrainBlockDesc*						_pHolder = nullptr;

	friend class CTerrainDataManager::CTerrainDataManagerImplementation;
};