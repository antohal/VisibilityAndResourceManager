#pragma once

#include "TerrainManager.h"

#include <vector>
#include <map>

class CTerrainObjectManager
{
public:

	enum EComputeParamsFlags
	{
		COMPUTE_GEODETIC_PARAMS = 0x1,
		COMPUTE_CUT_PARAMS = 0x2,
		COMPUTE_TREE_PATH = 0x4,

		COMPUTE_FULL_PARAMS = COMPUTE_GEODETIC_PARAMS | COMPUTE_CUT_PARAMS | COMPUTE_TREE_PATH
	};

	CTerrainObjectManager();

	bool LoadDatabaseFile(const wchar_t* in_pcwszDatabaseFile, unsigned int in_uiMaxDepth, unsigned int & out_resultingDepth);

	const std::vector<size_t>&			GetLodsResolution() const {
		return _vecLODResolution;
	}

	const DataBaseInfo&					GetDatabaseInfo() const {
		return _databaseInfo;
	}

	const std::vector<LodInfoStruct>&	GetLodInfos() const {
		return _vecLodInfos;
	}

	void							GetTerrainObjectChildren(TerrainObjectID ID, std::vector<TerrainObjectID>& out_vecChildren);
	void							ComputeTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams& out_Params, int flags = COMPUTE_FULL_PARAMS) const;
	TerrainObjectID					GetTerrainObjectParent(TerrainObjectID ID) const;
	STerrainBlockIndex				GetCoordInParent(TerrainObjectID ID) const;
	unsigned char					GetObjectDepth(TerrainObjectID ID) const;
	void							GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8]);

	bool							IsObjectValid(TerrainObjectID ID) const;

	std::wstring					GetTextureFileName(TerrainObjectID ID) const;
	std::wstring					GetHeighmapFileName(TerrainObjectID ID) const;

	std::vector<TerrainObjectID>	GetRootObjects() const;

private:

	TerrainObjectID					GenerateTerrainObjectID(unsigned char in_nLOD, unsigned short X, unsigned short Y) const;
	void							DecomposeTerrainObjectID(TerrainObjectID ID, unsigned char& out_nLOD, unsigned short& out_X, unsigned short& out_Y) const;
	bool							IsObjectHasSubhierarchy(TerrainObjectID ID);

	DataBaseInfo					_databaseInfo;
	std::vector<LodInfoStruct>		_vecLodInfos;


	std::map<TerrainObjectID, bool>	_mapHasSubhierarchy;

	std::vector<size_t>				_vecLODResolution;
	std::vector<unsigned short>		_vecTotalXCountPerLOD;
	std::vector<unsigned short>		_vecTotalYCountPerLOD;

	float							_fLattitudeRange;
	float							_fLongitudeRange;

	std::wstring					_rootDirectory;
};
