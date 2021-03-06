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
	void									Init(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner, float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude,
		const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, CTerrainBlockDesc* in_pParent);

	//@{ �������� ����������� �������� �� ������� � ������
	float									GetMinimumLattitude() const;
	float									GetMaximumLattitude() const;

	float									GetMinimumLongitude() const;
	float									GetMaximumLongitude() const;
	//@}

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

	// �������� ���������� [�����������]
	static CTerrainBlockDesc*				CreateTerrainBlockDataInstance(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner,
		float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude, const std::wstring& in_wsTextureFileName, 
		const std::wstring& in_wsHeightmapFileName, CTerrainBlockDesc* in_pParent);

private:

	float									_fMinLattitude = 0;
	float									_fMaxLattitude = 0;

	float									_fMinLongitude = 0;
	float									_fMaxLongitude = 0;

	std::wstring							_wsTextureFileName;
	std::wstring							_wsHeightmapFileName;

	CTerrainBlockDesc*						_pParentBlock = nullptr;
	std::vector<CTerrainBlockDesc*>			_vecChildBlocks;

	CTerrainDataManager::CTerrainDataManagerImplementation*		_pOwner = nullptr;
	CTerrainBlockDesc*						_pHolder = nullptr;
};
