#pragma once

#include "vecmath.h"
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
	~CTerrainObjectManager();

	bool LoadDatabaseFile(const wchar_t* in_pcwszDatabaseFile, unsigned int in_uiMaxDepth, unsigned int & out_resultingDepth);

	const std::vector<size_t>&			GetLodsResolution() const {
		return _vecLODResolution;
	}

	const DataBaseInfo&					GetDatabaseInfo() const {
		return _databaseInfo;
	}

	const std::vector<LodInfoStruct_Ver_1_2>&	GetLodInfos() const {
		return _vecLodInfos;
	}

	static unsigned int				DepthDifference(unsigned int d1, unsigned int d2)
	{
		if (d1 > d2)
			return d1 - d2;

		return d2 - d1;
	}

	void							GetTerrainObjectChildren(TerrainObjectID ID, std::vector<TerrainObjectID>& out_vecChildren);
	void							ComputeTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams& out_Params, int flags = COMPUTE_FULL_PARAMS) const;
	TerrainObjectID					GetTerrainObjectParent(TerrainObjectID ID) const;
	STerrainBlockIndex				GetCoordInParent(TerrainObjectID ID) const;
	unsigned char					GetObjectDepth(TerrainObjectID ID) const;

	std::pair<unsigned int, unsigned int>	GetObjectHfResolution(TerrainObjectID ID, unsigned int in_uiCompressionRatio) const;
	void							GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8]);

	bool							IsObjectValid(TerrainObjectID ID) const;

	std::wstring					GetTextureFileName(TerrainObjectID ID) const;
	std::wstring					GetHeighmapFileName(TerrainObjectID ID) const;

	std::vector<TerrainObjectID>	GetRootObjects() const;
	const wchar_t*					GetRootDirectory() const {
		return _rootDirectory.c_str();
	}

	double	AngularDistance(double a1, double a2);
	
	bool	GetClippedProjection(TerrainObjectID ID, const vm::Vector3df& in_vPos, double& out_dfLat, double& out_dfLong);

private:

	TerrainObjectID					GenerateTerrainObjectID(unsigned char in_nLOD, unsigned short X, unsigned short Y) const;
	void							DecomposeTerrainObjectID(TerrainObjectID ID, unsigned char& out_nLOD, unsigned short& out_X, unsigned short& out_Y) const;
	bool							IsObjectHasSubhierarchy(TerrainObjectID ID);

	DataBaseInfo_Ver_1_3			_databaseInfo;
	std::vector<LodInfoStruct_Ver_1_2>		_vecLodInfos;


	std::map<TerrainObjectID, bool>	_mapHasSubhierarchy;

	std::vector<size_t>				_vecLODResolution;
	
	std::vector<unsigned short>		_vecTotalXCountPerLOD;
	std::vector<unsigned short>		_vecTotalYCountPerLOD;

	std::vector<unsigned short>		_vecValidXCountPerLOD;
	std::vector<unsigned short>		_vecValidYCountPerLOD;

	float							_fLattitudeRange;
	float							_fLongitudeRange;

	std::wstring					_rootDirectory;
};

CTerrainObjectManager*	GetObjectManager();