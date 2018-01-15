#include "TerrainObjectManager.h"
#include "Log.h"
#include "FileUtil.h"
#include <Shlwapi.h>

#include <algorithm>
#include "vecmath.h"
#include "wgs84.h"

#pragma comment (lib, "Shlwapi.lib")

struct LodInfoStruct_Ver10
{
	short Width;			// øèðèíû òåêñóòðû
	short Height;			// âûñîòà òåêñóòðû
	short CountY;			// êîë-âî òåêñòóð ïî X
	short CountX;			// êîë-âî òåêñòóð ïî Y
};

namespace {
	CTerrainObjectManager*	g_pTerrainObjectManager = nullptr;
};

CTerrainObjectManager*	GetObjectManager()
{
	return g_pTerrainObjectManager;
}

CTerrainObjectManager::CTerrainObjectManager()
{
	g_pTerrainObjectManager = this;

	_vecLODResolution.resize(20);

	for (size_t i = 0; i < 20; i++)
	{
		_vecLODResolution[i] = 512;
	}

	_fLattitudeRange = M_PI;
	_fLongitudeRange = 2 * M_PI;
}

CTerrainObjectManager::~CTerrainObjectManager()
{
	g_pTerrainObjectManager = nullptr;
}


double CTerrainObjectManager::AngularDistance(double a1, double a2)
{
	double cos1 = cos(a1);
	double cos2 = cos(a2);

	double sin1 = sin(a1);
	double sin2 = sin(a2);

	vm::Vector3df v1(cos1, sin1, 0);
	vm::Vector3df v2(cos2, sin2, 0);

	double angle = acos(vm::dot_prod(v1, v2));

	vm::Vector3df v = vm::cross(v1, v2);

	if (v[2] > 0)
		angle = -angle;

	return angle;
}

bool CTerrainObjectManager::LoadDatabaseFile(const wchar_t* in_pcwszDatabaseFile, unsigned int in_uiMaxDepth, unsigned int & out_resultingDepth)
{
	out_resultingDepth = 0;
	std::wstring wsDbFileName = in_pcwszDatabaseFile;

	if (!PathFileExistsW(wsDbFileName.c_str()))
	{
		wsDbFileName = std::wstring(GetStartDir() + in_pcwszDatabaseFile);

		if (!PathFileExistsW(wsDbFileName.c_str()))
		{
			LogMessage("Cannot open database file %ls", wsDbFileName.c_str());
			return false;
		}
	}

	_rootDirectory = ExtractFileDirectory(wsDbFileName);

	bool bSuccessifulRead = true;

	FILE* fp = nullptr;

	_wfopen_s(&fp, wsDbFileName.c_str(), L"rb");

	if (fp)
	{
		if (fread_s(&_databaseInfo, sizeof(DataBaseInfo), sizeof(DataBaseInfo), 1, fp) != 1)
		{
			bSuccessifulRead = false;
		}

		if (bSuccessifulRead)
		{
			_vecLodInfos.resize(_databaseInfo.LodCount);

			if (_databaseInfo.Major == 1 && _databaseInfo.Minor == 1)
			{
				size_t nReadLods = fread_s(&_vecLodInfos[0], sizeof(LodInfoStruct) * _databaseInfo.LodCount, sizeof(LodInfoStruct), _databaseInfo.LodCount, fp);

				if (nReadLods != _databaseInfo.LodCount)
				{
					LogMessage("Error reading lod elements from file %ls, aborting. Readed only %d, while expected %d", in_pcwszDatabaseFile, nReadLods, _databaseInfo.LodCount);
					bSuccessifulRead = false;
				}
			}
			else if (_databaseInfo.Major == 1 && _databaseInfo.Minor == 0)
			{
				std::vector<LodInfoStruct_Ver10> vecReadLods;
				vecReadLods.resize(_databaseInfo.LodCount);

				size_t nReadLods = fread_s(&vecReadLods[0], sizeof(LodInfoStruct_Ver10) * _databaseInfo.LodCount, sizeof(LodInfoStruct_Ver10), _databaseInfo.LodCount, fp);

				if (nReadLods != _databaseInfo.LodCount)
				{
					LogMessage("Error reading lod elements from file %ls, aborting. Readed only %d, while expected %d", in_pcwszDatabaseFile, nReadLods, _databaseInfo.LodCount);
					bSuccessifulRead = false;
				}

				for (size_t iLod = 0; iLod < nReadLods; iLod++)
				{
					memcpy(&_vecLodInfos[iLod], &vecReadLods[iLod], sizeof(LodInfoStruct_Ver10));

					_vecLodInfos[iLod].AltWidth = vecReadLods[iLod].Width;
					_vecLodInfos[iLod].AltHeight = vecReadLods[iLod].Height;
				}
			}
		}

		fclose(fp);
	}
	else
	{
		LogMessage("Cannot open database file %ls, aborting", in_pcwszDatabaseFile);
		return false;
	}

	if (!bSuccessifulRead)
	{
		LogMessage("Error reading earth database file %ls, aborting", in_pcwszDatabaseFile);
		return false;
	}
	else
	{

		for (int i = 0; i < _databaseInfo.LodCount; i++)
		{
			_vecLODResolution[i] = std::max<short>(_vecLodInfos[i].Width, _vecLodInfos[i].Height);
		}

	}

	double totalPixelsX = _vecLodInfos[_databaseInfo.LodCount - 1].Width;
	double totalPixelsY = _vecLodInfos[_databaseInfo.LodCount - 1].Height;

	_vecTotalXCountPerLOD.resize(_databaseInfo.LodCount);
	_vecTotalYCountPerLOD.resize(_databaseInfo.LodCount);

	_vecValidXCountPerLOD.resize(_databaseInfo.LodCount);
	_vecValidYCountPerLOD.resize(_databaseInfo.LodCount);

	for (int iLodLevel = 0; iLodLevel < _databaseInfo.LodCount; iLodLevel++)
	{
		totalPixelsX *= _vecLodInfos[iLodLevel].CountX;
		totalPixelsY *= _vecLodInfos[iLodLevel].CountY;

		_vecTotalXCountPerLOD[iLodLevel] = _vecLodInfos[iLodLevel].CountX;
		_vecTotalYCountPerLOD[iLodLevel] = _vecLodInfos[iLodLevel].CountY;

		for (int iLowerLevel = iLodLevel - 1; iLowerLevel >= 0; iLowerLevel--)
		{
			_vecTotalXCountPerLOD[iLodLevel] *= _vecLodInfos[iLowerLevel].CountX;
			_vecTotalYCountPerLOD[iLodLevel] *= _vecLodInfos[iLowerLevel].CountY;
		}
	}

	float fLattitudeScaleCoeff = (totalPixelsX) / (totalPixelsX - _databaseInfo.DeltaX);
	float fLongitudeScaleCoeff = (totalPixelsY) / (totalPixelsY - _databaseInfo.DeltaY);

	_fLattitudeRange = static_cast<float>(M_PI) *fLattitudeScaleCoeff;
	_fLongitudeRange = 2 * static_cast<float>(M_PI) *fLattitudeScaleCoeff;

	for (int iLodLevel = 0; iLodLevel < _databaseInfo.LodCount; iLodLevel++)
	{
		double deltaX = _fLattitudeRange / _vecTotalXCountPerLOD[iLodLevel];
		double deltaY = _fLongitudeRange / _vecTotalYCountPerLOD[iLodLevel];

		_vecValidXCountPerLOD[iLodLevel] = (unsigned short)(ceil(M_PI / deltaX));
		_vecValidYCountPerLOD[iLodLevel] = (unsigned short)(ceil(2 * M_PI / deltaY));
	}

	if (_databaseInfo.LodCount > in_uiMaxDepth)
	{
		_databaseInfo.LodCount = in_uiMaxDepth;

		_vecLodInfos.resize(in_uiMaxDepth);
		_vecTotalXCountPerLOD.resize(in_uiMaxDepth);
		_vecTotalYCountPerLOD.resize(in_uiMaxDepth);

		_vecValidXCountPerLOD.resize(in_uiMaxDepth);
		_vecValidYCountPerLOD.resize(in_uiMaxDepth);
	}

	out_resultingDepth = _databaseInfo.LodCount;


	return true;
}

TerrainObjectID CTerrainObjectManager::GenerateTerrainObjectID(unsigned char in_nLOD, unsigned short X, unsigned short Y) const
{
	return ((TerrainObjectID)(in_nLOD) << 32) | ((TerrainObjectID)X << 16) | ((TerrainObjectID)Y);
}

void CTerrainObjectManager::DecomposeTerrainObjectID(TerrainObjectID ID, unsigned char& out_nLOD, unsigned short& out_X, unsigned short& out_Y) const
{
	out_nLOD = static_cast<unsigned char>((ID >> 32) & 0xFF);
	out_X = static_cast<unsigned short>((ID >> 16) & 0xFFFF);
	out_Y = static_cast<unsigned short>((ID) & 0xFFFF);
}

bool CTerrainObjectManager::IsObjectHasSubhierarchy(TerrainObjectID ID)
{
//	return true;

	auto it = _mapHasSubhierarchy.find(ID);

	// if we have cached subhierarchy flag - return it
	if (it != _mapHasSubhierarchy.end())
	{
		return it->second;
	}

	// otherwise, compute it

	STerrainBlockParams params;
	ComputeTerrainObjectParams(ID, params);

	std::wstring wsSubdirectoryName = _rootDirectory;

	for (unsigned int i = 0; i <= params.uiDepth; i++)
	{
		std::wstring wsXX = std::to_wstring(params.aTreePosition[i].ucLattitudeIndex);
		std::wstring wsYY = std::to_wstring(params.aTreePosition[i].ucLongitudeIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;

		wsSubdirectoryName += L"\\" + wsXX + L"_" + wsYY;
	}

	bool bHasSubhierarchy = PathFileExistsW(wsSubdirectoryName.c_str());

	_mapHasSubhierarchy[ID] = bHasSubhierarchy;

	return bHasSubhierarchy;
}

std::wstring CTerrainObjectManager::GetTextureFileName(TerrainObjectID ID) const
{
	STerrainBlockParams params;
	ComputeTerrainObjectParams(ID, params);

	std::wstring wsTextureName = _rootDirectory;

	for (unsigned int i = 0; i <= params.uiDepth; i++)
	{
		std::wstring wsXX = std::to_wstring(params.aTreePosition[i].ucLattitudeIndex);
		std::wstring wsYY = std::to_wstring(params.aTreePosition[i].ucLongitudeIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;

		if (i < params.uiDepth)
			wsTextureName += L"\\" + wsXX + L"_" + wsYY;
		else
			wsTextureName += L"\\T_" + wsXX + L"_" + wsYY + L".dds";
	}

	return wsTextureName;
}

std::wstring CTerrainObjectManager::GetHeighmapFileName(TerrainObjectID ID) const
{
	STerrainBlockParams params;
	ComputeTerrainObjectParams(ID, params);

	std::wstring wsTextureName = _rootDirectory;

	for (unsigned int i = 0; i <= params.uiDepth; i++)
	{
		std::wstring wsXX = std::to_wstring(params.aTreePosition[i].ucLattitudeIndex);
		std::wstring wsYY = std::to_wstring(params.aTreePosition[i].ucLongitudeIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;

		if (i < params.uiDepth)
			wsTextureName += L"\\" + wsXX + L"_" + wsYY;
		else
			wsTextureName += L"\\H_" + wsXX + L"_" + wsYY + L".dds";
	}

	return wsTextureName;
}

unsigned char CTerrainObjectManager::GetObjectDepth(TerrainObjectID ID) const
{
	return static_cast<unsigned char>((ID >> 32) & 0xFF);
}

std::pair<unsigned int, unsigned int>	CTerrainObjectManager::GetObjectHfResolution(TerrainObjectID ID, unsigned int in_uiCompressionRatio) const
{
	unsigned char depth = GetObjectDepth(ID);

	if (depth >= _vecLodInfos.size())
	{
		LogMessage("CTerrainObjectManager::GetObjectHfResolution, depth of object is more than config, %ld", ID);
		return std::make_pair<unsigned int, unsigned int>(16, 16);
	}

	if (_vecLodInfos[depth].HasBorder)
		return std::make_pair<unsigned int, unsigned int>(((unsigned int)_vecLodInfos[depth].AltWidth) / in_uiCompressionRatio + 1, ((unsigned int)_vecLodInfos[depth].AltHeight) / in_uiCompressionRatio + 1);

	return std::make_pair<unsigned int, unsigned int>((unsigned int)_vecLodInfos[depth].AltWidth / in_uiCompressionRatio, (unsigned int)_vecLodInfos[depth].AltHeight / in_uiCompressionRatio);
}

void CTerrainObjectManager::GetTerrainObjectChildren(TerrainObjectID ID, std::vector<TerrainObjectID>& out_vecChildren)
{
	out_vecChildren.resize(0);

	if (!IsObjectHasSubhierarchy(ID))
		return;

	unsigned char lod;
	unsigned short X, Y;

	DecomposeTerrainObjectID(ID, lod, X, Y);

	if (lod >= _databaseInfo.LodCount - 1)
		return;

	const LodInfoStruct& nextLodInfo = _vecLodInfos[lod + 1];

	unsigned short startX = nextLodInfo.CountX * X;
	unsigned short startY = nextLodInfo.CountY * Y;

	out_vecChildren.reserve(nextLodInfo.CountX * nextLodInfo.CountY);

	for (unsigned short iX = 0; iX < nextLodInfo.CountX; iX++)
	{
		for (unsigned short iY = 0; iY < nextLodInfo.CountY; iY++)
		{
			out_vecChildren.push_back(GenerateTerrainObjectID(lod + 1, startX + iX, startY + iY));
		}
	}
}

void CTerrainObjectManager::GetTerrainObjectNeighbours(TerrainObjectID ID, TerrainObjectID outNeighbours[8])
{
	unsigned char lod;
	unsigned short X, Y;

	DecomposeTerrainObjectID(ID, lod, X, Y);

	unsigned short XCount = _vecValidXCountPerLOD[lod];
	unsigned short YCount = _vecValidYCountPerLOD[lod];

	if (XCount == 0 || YCount == 0)
	{
		LogMessage("CTerrainObjectManager::GetTerrainObjectNeighbours: objects count on lod number %d is zero. possible not initialized", (int)lod);
		return;
	}

	// 0, 1, 7
	if (X > 0)
	{
		outNeighbours[0] = GenerateTerrainObjectID(lod, X - 1, Y);

		if (Y < YCount - 1)
		{
			outNeighbours[1] = GenerateTerrainObjectID(lod, X - 1, Y + 1);
		}
		else
		{
			outNeighbours[1] = GenerateTerrainObjectID(lod, X - 1, 0);
		}

		if (Y > 0)
		{
			outNeighbours[7] = GenerateTerrainObjectID(lod, X - 1, Y - 1);
		}
		else
		{
			outNeighbours[7] = GenerateTerrainObjectID(lod, X - 1, YCount - 1);
		}
	}
	else
	{
		outNeighbours[0] = INVALID_TERRAIN_OBJECT_ID;
		outNeighbours[7] = INVALID_TERRAIN_OBJECT_ID;
		outNeighbours[1] = INVALID_TERRAIN_OBJECT_ID;
	}


	//6

	if (Y > 0)
	{
		outNeighbours[6] = GenerateTerrainObjectID(lod, X, Y - 1);
	}
	else
	{
		outNeighbours[6] = GenerateTerrainObjectID(lod, X, YCount - 1);
	}


	//2
	if (Y < YCount - 1)
	{
		outNeighbours[2] = GenerateTerrainObjectID(lod, X, Y + 1);
	}
	else
	{
		outNeighbours[2] = GenerateTerrainObjectID(lod, X, 0);
	}


	// 5, 4, 3
	if (X < XCount - 1)
	{
		outNeighbours[4] = GenerateTerrainObjectID(lod, X + 1, Y);

		if (Y > 0)
		{
			outNeighbours[5] = GenerateTerrainObjectID(lod, X + 1, Y - 1);
		}
		else
		{
			outNeighbours[5] = GenerateTerrainObjectID(lod, X + 1, YCount - 1);
		}


		if (Y < YCount - 1)
		{
			outNeighbours[3] = GenerateTerrainObjectID(lod, X + 1, Y + 1);
		}
		else
		{
			outNeighbours[3] = GenerateTerrainObjectID(lod, X + 1, 0);
		}
	}
	else
	{
		outNeighbours[5] = INVALID_TERRAIN_OBJECT_ID;
		outNeighbours[4] = INVALID_TERRAIN_OBJECT_ID;
		outNeighbours[3] = INVALID_TERRAIN_OBJECT_ID;
	}
}

bool CTerrainObjectManager::IsObjectValid(TerrainObjectID ID) const
{
	STerrainBlockParams params;
	ComputeTerrainObjectParams(ID, params, COMPUTE_GEODETIC_PARAMS | COMPUTE_CUT_PARAMS);

	if (params.fLattitudeCutCoeff == 0 || params.fLongitudeÑutCoeff == 0)
		return false;

	return true;
}

void CTerrainObjectManager::ComputeTerrainObjectParams(TerrainObjectID ID, STerrainBlockParams& out_Params, int flags) const
{
	double dfGlobalMinLat = static_cast<float>(M_PI * 0.5) - _fLattitudeRange;
	double dfGlobalMaxLat = static_cast<float>(M_PI * 0.5);

	double dfGlobalMinLong = 0;
	double dfGlobalMaxLong = _fLongitudeRange;

	unsigned char lod;
	unsigned short X, Y;

	DecomposeTerrainObjectID(ID, lod, X, Y);

	if (lod >= _databaseInfo.LodCount)
		return;

	if (flags & COMPUTE_GEODETIC_PARAMS)
	{
		unsigned short totalXObjectsCount = _vecTotalXCountPerLOD[lod];
		unsigned short totalYObjectsCount = _vecTotalYCountPerLOD[lod];

		double dfDeltaLatPerObject = (dfGlobalMaxLat - dfGlobalMinLat) / totalXObjectsCount;
		double dfDeltaLongPerObject = (dfGlobalMaxLong - dfGlobalMinLong) / totalYObjectsCount;

		double dfMinLongitude = dfGlobalMinLong + Y * dfDeltaLongPerObject;
		double dfMaxLongitude = dfMinLongitude + dfDeltaLongPerObject;
		double dfMaxLattitude = dfGlobalMaxLat - X * dfDeltaLatPerObject;
		double dfMinLattitude = dfMaxLattitude - dfDeltaLatPerObject;

		out_Params.fMinLongitude = static_cast<float>(dfMinLongitude);
		out_Params.fMaxLongitude = static_cast<float>(dfMaxLongitude);
		out_Params.fMaxLattitude = static_cast<float>(dfMaxLattitude);
		out_Params.fMinLattitude = static_cast<float>(dfMinLattitude);
	}

	out_Params.uiDepth = lod;


	if (flags & COMPUTE_CUT_PARAMS)
	{

		if (out_Params.fMaxLattitude <= -static_cast<float>(M_PI*0.5))
		{
			out_Params.fLattitudeCutCoeff = 0;
		}
		else if (out_Params.fMinLattitude <= -static_cast<float>(M_PI*0.5))
		{
			out_Params.fLattitudeCutCoeff = (out_Params.fMaxLattitude + static_cast<float>(M_PI*0.5)) / (out_Params.fMaxLattitude - out_Params.fMinLattitude);
		}
		else
			out_Params.fLattitudeCutCoeff = 1;


		if (out_Params.fMinLongitude >= static_cast<float>(M_PI * 2))
		{
			out_Params.fLongitudeÑutCoeff = 0;
		}
		else if (out_Params.fMaxLongitude >= static_cast<float>(M_PI * 2))
		{
			out_Params.fLongitudeÑutCoeff = (static_cast<float>(M_PI * 2) - out_Params.fMinLongitude) / (out_Params.fMaxLongitude - out_Params.fMinLongitude);
		}
		else
			out_Params.fLongitudeÑutCoeff = 1;
	}


	if (out_Params.fMinLattitude < -M_PI*0.5)
	{
		out_Params.fMinLattitude = -M_PI*0.5;
	}

	if (out_Params.fMaxLongitude > 2 * M_PI)
	{
		out_Params.fMaxLongitude = 2 * M_PI;
	}

	if (flags & COMPUTE_TREE_PATH)
	{
		out_Params.aTreePosition[lod] = GetCoordInParent(ID);
		TerrainObjectID parentID = GetTerrainObjectParent(ID);

		for (int i = lod - 1; i >= 0; i--)
		{
			out_Params.aTreePosition[i] = GetCoordInParent(parentID);

			if (i > 0)
				parentID = GetTerrainObjectParent(parentID);
		}
	}
}

STerrainBlockIndex CTerrainObjectManager::GetCoordInParent(TerrainObjectID ID) const
{
	unsigned char lod;
	unsigned short X, Y;

	DecomposeTerrainObjectID(ID, lod, X, Y);

	STerrainBlockIndex result;

	result.ucLattitudeIndex = X - (X / _vecLodInfos[lod].CountX) * _vecLodInfos[lod].CountX;
	result.ucLongitudeIndex = Y - (Y / _vecLodInfos[lod].CountY) * _vecLodInfos[lod].CountY;

	return result;
}

TerrainObjectID CTerrainObjectManager::GetTerrainObjectParent(TerrainObjectID ID) const
{
	unsigned char lod;
	unsigned short X, Y;

	DecomposeTerrainObjectID(ID, lod, X, Y);

	if (lod == 0)
		return INVALID_TERRAIN_OBJECT_ID;

	unsigned char parentLod = lod - 1;

	unsigned short parentX = X / _vecLodInfos[lod].CountX;
	unsigned short parentY = Y / _vecLodInfos[lod].CountY;

	return GenerateTerrainObjectID(parentLod, parentX, parentY);
}

std::vector<TerrainObjectID> CTerrainObjectManager::GetRootObjects() const
{
	std::vector<TerrainObjectID> resultVector;

	if (_vecLodInfos.empty())
	{
		LogMessage("Error getting root objects: CTerrainObjectManager is not initialized!");
		return resultVector;
	}

	const LodInfoStruct& zeroLod = _vecLodInfos[0];

	for (short iX = 0; iX < zeroLod.CountX; iX++)
	{
		for (short iY = 0; iY < zeroLod.CountY; iY++)
		{
			resultVector.push_back(GenerateTerrainObjectID(0, (unsigned short)iX, (unsigned short)iY));
		}
	}

	return resultVector;
}

bool CTerrainObjectManager::GetClippedProjection(TerrainObjectID ID, const vm::Vector3df& in_vPos, double& out_dfLat, double& out_dfLong)
{
	double dfLong, dfLat, dfHeight, dfLen;
	GetWGS84LongLatHeight(in_vPos, dfLong, dfLat, dfHeight, dfLen);

	STerrainBlockParams params;
	GetObjectManager()->ComputeTerrainObjectParams(ID, params, CTerrainObjectManager::COMPUTE_GEODETIC_PARAMS | CTerrainObjectManager::COMPUTE_CUT_PARAMS);

	double dfMinLat = params.fMinLattitude;
	double dfMaxLat = params.fMaxLattitude;
	double dfMinLong = params.fMinLongitude;
	double dfMaxLong = params.fMaxLongitude;

	bool isPositionAboveBlock = false;

	if (dfLat >= dfMinLat && dfLat <= dfMaxLat && dfLong >= dfMinLong && dfLong <= dfMaxLong)
	{
		isPositionAboveBlock = true;
	}
	else
	{
		vm::CLIP_NUMBER(dfLat, dfMinLat, dfMaxLat);

		if (dfLong < dfMinLong || dfLong > dfMaxLong)
		{
			if (GetObjectManager()->AngularDistance(dfLong, dfMaxLong) > 0)
			{
				dfLong = dfMaxLong;
			}
			else
			{
				dfLong = dfMinLong;
			}
		}
	}

	out_dfLat = dfLat;
	out_dfLong = dfLong;

	return isPositionAboveBlock;
}
