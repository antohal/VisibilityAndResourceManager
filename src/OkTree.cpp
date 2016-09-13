#include "stdafx.h"
#include "OkTree.h"
#include "FuncForOkTree.h"
#include "Geometry/InFrustum.h"

#include "OkTreeGridIterator.h"



#ifdef _DEBUG
//#define GRID_ASSERTS
#endif

#ifdef _DEBUG
#define MMX_ASSERT( Val ) {  /*_mm_empty();*/ assert_debug( (Val), L"MMX assert" ); }
#else
#define MMX_ASSERT( Val ) {}
#endif //_DEBUG

#ifdef _DEBUG
#define RANGE_ASSERT( Val ) {  assert_debug( _fpclass(Val) == _FPCLASS_NZ || _fpclass(Val) == _FPCLASS_PZ || _fpclass(Val) == _FPCLASS_NINF || _fpclass(Val) == _FPCLASS_PINF || _fpclass(Val) == _FPCLASS_PN || _fpclass(Val) == _FPCLASS_NN, L"Range assert" ); }
#else
#define RANGE_ASSERT( Val ) {}
#endif //_DEBUG


namespace {

	const UINT32 pdwCoordToOct5[32] = 
	{	0x0, 0x1000, 0x200, 0x1200, 0x40, 0x1040, 0x240, 0x1240, 0x8, 0x1008, 0x208, 0x1208, 
		0x48, 0x1048, 0x248, 0x1248, 0x1, 0x1001, 0x201, 0x1201, 0x41, 0x1041, 0x241, 0x1241, 
		0x9, 0x1009, 0x209, 0x1209, 0x49, 0x1049, 0x249, 0x1249
	};
	
	const UINT32 pdwCoordToOct8[256] = {
		0x0, 0x200000, 0x40000, 0x240000, 0x8000, 0x208000, 0x48000, 0x248000, 0x1000, 0x201000, 0x41000, 0x241000, 0x9000, 0x209000, 0x49000, 
		0x249000, 0x200, 0x200200, 0x40200, 0x240200, 0x8200, 0x208200, 0x48200, 0x248200, 0x1200, 0x201200, 0x41200, 0x241200, 0x9200, 0x209200, 
		0x49200, 0x249200, 0x40, 0x200040, 0x40040, 0x240040, 0x8040, 0x208040, 0x48040, 0x248040, 0x1040, 0x201040, 0x41040, 0x241040, 0x9040, 
		0x209040, 0x49040, 0x249040, 0x240, 0x200240, 0x40240, 0x240240, 0x8240, 0x208240, 0x48240, 0x248240, 0x1240, 0x201240, 0x41240, 0x241240, 
		0x9240, 0x209240, 0x49240, 0x249240, 0x8, 0x200008, 0x40008, 0x240008, 0x8008, 0x208008, 0x48008, 0x248008, 0x1008, 0x201008, 0x41008, 
		0x241008, 0x9008, 0x209008, 0x49008, 0x249008, 0x208, 0x200208, 0x40208, 0x240208, 0x8208, 0x208208, 0x48208, 0x248208, 0x1208, 0x201208, 
		0x41208, 0x241208, 0x9208, 0x209208, 0x49208, 0x249208, 0x48, 0x200048, 0x40048, 0x240048, 0x8048, 0x208048, 0x48048, 0x248048, 0x1048, 
		0x201048, 0x41048, 0x241048, 0x9048, 0x209048, 0x49048, 0x249048, 0x248, 0x200248, 0x40248, 0x240248, 0x8248, 0x208248, 0x48248, 0x248248, 
		0x1248, 0x201248, 0x41248, 0x241248, 0x9248, 0x209248, 0x49248, 0x249248, 0x1, 0x200001, 0x40001, 0x240001, 0x8001, 0x208001, 0x48001, 0x248001, 
		0x1001, 0x201001, 0x41001, 0x241001, 0x9001, 0x209001, 0x49001, 0x249001, 0x201, 0x200201, 0x40201, 0x240201, 0x8201, 0x208201, 0x48201, 0x248201, 
		0x1201, 0x201201, 0x41201, 0x241201, 0x9201, 0x209201, 0x49201, 0x249201, 0x41, 0x200041, 0x40041, 0x240041, 0x8041, 0x208041, 0x48041, 0x248041, 
		0x1041, 0x201041, 0x41041, 0x241041, 0x9041, 0x209041, 0x49041, 0x249041, 0x241, 0x200241, 0x40241, 0x240241, 0x8241, 0x208241, 0x48241, 0x248241, 
		0x1241, 0x201241, 0x41241, 0x241241, 0x9241, 0x209241, 0x49241, 0x249241, 0x9, 0x200009, 0x40009, 0x240009, 0x8009, 0x208009, 0x48009, 0x248009, 
		0x1009, 0x201009, 0x41009, 0x241009, 0x9009, 0x209009, 0x49009, 0x249009, 0x209, 0x200209, 0x40209, 0x240209, 0x8209, 0x208209, 0x48209, 0x248209, 
		0x1209, 0x201209, 0x41209, 0x241209, 0x9209, 0x209209, 0x49209, 0x249209, 0x49, 0x200049, 0x40049, 0x240049, 0x8049, 0x208049, 0x48049, 0x248049, 
		0x1049, 0x201049, 0x41049, 0x241049, 0x9049, 0x209049, 0x49049, 0x249049, 0x249, 0x200249, 0x40249, 0x240249, 0x8249, 0x208249, 0x48249, 0x248249, 
		0x1249, 0x201249, 0x41249, 0x241249, 0x9249, 0x209249, 0x49249, 0x249249
	};

	const UINT8 pbyOctToChild[8][8] = 
	{
		{0x1, 0x3, 0x5, 0xf, 0x11, 0x33, 0x55, 0xff}, 
		{0x0, 0x2, 0x0, 0xa, 0x0, 0x22, 0x0, 0xaa}, 
		{0x0, 0x0, 0x4, 0xc, 0x0, 0x0, 0x44, 0xcc}, 
		{0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x88}, 
		{0x0, 0x0, 0x0, 0x0, 0x10, 0x30, 0x50, 0xf0}, 
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0xa0}, 
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0xc0}, 
		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80}
	};

	const UINT8 pdwClearBit[8] = {~(1 << 0), ~(1 << 1),  ~(1 << 2), ~(1 << 3), ~(1 << 4), ~(1 << 5), ~(1 << 6), ~(1 << 7)};

	const unsigned MAX_OBJECT_COUNT = 1000000;//60000;
	const unsigned k_TmpStackSize = g_btDepthCount * 8;
	const unsigned k_DelQueueSize = g_btDepthCount * 8;
}


//vector<COkTree::SOkNodeStackData>	COkTree::s_aStack;
//vector<COkTree::SDelNodeData> COkTree::s_aDelQueue;

void DownByTube(COkNode*& in_pStartNode, BYTE in_btMinDepth, BYTE& io_btDepth,
				__m64& io_mm64MinCur1, __m64& io_mm64MaxCur1,
				__m64& io_mm64MinCur2, __m64& io_mm64MaxCur2)
{

	do 
	{
		bool bNoDown = io_btDepth >= in_btMinDepth;
		if( bNoDown )
		{
			////_mm_empty();
			return;
		}

		BYTE btCell_1 = GetLow3BitsFromInt64(io_mm64MinCur1.m64_u64);
		BYTE btCell_2 = GetLow3BitsFromInt64(io_mm64MaxCur1.m64_u64);
		BYTE btCell_3 = GetLow3BitsFromInt64(io_mm64MinCur2.m64_u64);
		BYTE btCell_4 = GetLow3BitsFromInt64(io_mm64MaxCur2.m64_u64);

		bool b12 = btCell_1 == btCell_2;
		bool b34 = btCell_3 == btCell_4;
		bool b23 = btCell_2 == btCell_3;


		if( !(b12 && b34 && b23) )
		{
			//COMMON TUBE IS FINISH.
			////_mm_empty();
			return;
		}

		in_pStartNode = in_pStartNode->m_ppChildren[btCell_1];
		MMX_ASSERT( in_pStartNode );

		ShearRight3Bit(io_mm64MinCur1);
		ShearRight3Bit(io_mm64MaxCur1);
		ShearRight3Bit(io_mm64MinCur2);
		ShearRight3Bit(io_mm64MaxCur2);

		io_btDepth++;

	} while( true );

	////_mm_empty();
}

struct SDebugIntersectInfo
{
	DWORD			m_dwIntersectStackDepth;
	BYTE			m_btRealIndex;
	EIntersectPlane	m_IntersectPlane;
};

char* GetIntersectPlaneName(EIntersectPlane in_IP)
{
	switch(in_IP)
	{
	case ipXY:
		return "XY";
	case ipXZ:
		return "XZ";
	case ipYZ:
		return "YZ";
	}
	return "Error";
}

COkTree::COkTree(const Vector3D<float>& in_vMinCorner, const Vector3D<float>& in_vMaxCorner, BYTE in_btCommonDepth, BYTE in_btMaxDepth) :
	m_stNodes(10), 
	m_stObjectListElems(10), 
	m_stObjectInfo(10),
	m_iObjectInfoTraceSize(1),
	m_bCommonLevelApply(true),
	m_btCommonDepth(in_btCommonDepth),
	m_vMinCorner( static_cast<double>(in_vMinCorner.x), static_cast<double>(in_vMinCorner.y), static_cast<double>(in_vMinCorner.z) ),
	m_vMaxCorner( static_cast<double>(in_vMaxCorner.x), static_cast<double>(in_vMaxCorner.y), static_cast<double>(in_vMaxCorner.z) ),
	m_btMaxDepth(in_btMaxDepth)
{
	m_pRootNode = GetNode(Vector3D<DWORD>(0), 0);

	//if (m_btCommonDepth > m_btMaxDepth)
	//	m_btCommonDepth = m_btMaxDepth;

	m_btCommonDepth = m_btMaxDepth;

	Vector3D<double> vSz = GetSize();
	UINT32 dwMAX = static_cast<UINT32>(~0);
	m_vMetersPerDWORD = vSz / static_cast<double>(dwMAX);
	m_vDWORDperMeter = Vector3D<double>( 1 / m_vMetersPerDWORD.x, 1 / m_vMetersPerDWORD.y, 1 / m_vMetersPerDWORD.z);			

	/* что бы образовать нижнюю таблицу
	__m64 mm64NewVal = _mm_setzero_si64();
	for(BYTE bt = 0; bt < 22; bt++)
	{
		__m64 mm64Temp = _mm_set_pi32( 0, 6 );
		mm64NewVal = _mm_xor_si64( _mm_slli_si64( mm64NewVal , 3 ), mm64Temp );
	}
	DWORD dw0 = _mm_cvtsi64_si32(mm64NewVal);
	mm64NewVal = _mm_srli_si64( mm64NewVal , 32 );
	DWORD dw1 = _mm_cvtsi64_si32(mm64NewVal);*/

	/*pmm64DupBits[0] = _mm_setzero_si64();
	pmm64DupBits[1] = _mm_set_pi32( 0x92492492, 0x49249249 ); //001 каждые три бита
	pmm64DupBits[2] = _mm_set_pi32( 0x24924924, 0x92492492 ); //010 каждые три бита
	pmm64DupBits[3] = _mm_set_pi32( 0xb6db6db6, 0xdb6db6db ); //011 каждые три бита
	pmm64DupBits[4] = _mm_set_pi32( 0x49249249, 0x24924924 ); //100 каждые три бита
	pmm64DupBits[5] = _mm_set_pi32( 0xdb6db6db, 0x6db6db6d ); //101 каждые три бита
	pmm64DupBits[6] = _mm_set_pi32( 0x6db6db6d, 0xb6db6db6 ); //110 каждые три бита
	pmm64DupBits[7] = _mm_set1_pi32( 0xffffffff );*/


	pmm64DupBits[0].m64_u64 = 0;
	
	pmm64DupBits[1].m64_i32[0] = 0x49249249;
	pmm64DupBits[1].m64_i32[1] = 0x92492492;

	pmm64DupBits[2].m64_i32[0] = 0x92492492;
	pmm64DupBits[2].m64_i32[1] = 0x24924924;

	pmm64DupBits[3].m64_i32[0] = 0xdb6db6db;
	pmm64DupBits[3].m64_i32[1] = 0xb6db6db6;

	pmm64DupBits[4].m64_i32[0] = 0x24924924;
	pmm64DupBits[4].m64_i32[1] = 0x49249249;

	pmm64DupBits[5].m64_i32[0] = 0x6db6db6d;
	pmm64DupBits[5].m64_i32[1] = 0xdb6db6db;

	pmm64DupBits[6].m64_i32[0] = 0xb6db6db6;
	pmm64DupBits[6].m64_i32[1] = 0x6db6db6d;

	pmm64DupBits[7].m64_i32[0] = 0xffffffff;
	pmm64DupBits[7].m64_i32[1] = 0xffffffff;


	////_mm_empty();

	m_pGridIterator = auto_ptr<IGridIterator>( new CSingleGridIterator(this) );
}

COkTree::~COkTree()
{
}

BYTE COkTree::GetDepthForMinNodeSize(DWORD in_dwMinNodeSize)
{
	BYTE btDepth = 0;
	DWORD dwNodeSize = DWORD(-1);
	while( dwNodeSize > in_dwMinNodeSize /*&& btDepth < 11*/ )
	{
		btDepth++;
		dwNodeSize >>= 1;
	}

	if (btDepth > m_btMaxDepth)
		btDepth = m_btMaxDepth;

	assert_debug(btDepth < g_btDepthCount, L"Illegal depth");
	return btDepth;
}


void COkTree::GetAABBByNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth, CBoundBox<float>& out_AABB)
{
	////_mm_empty();
	Vector3D<float> vMin = Vector3D<float>( static_cast<float>(in_vMin.x * m_vMetersPerDWORD.x + GetMinCorner().x), 
											static_cast<float>(in_vMin.y * m_vMetersPerDWORD.y + GetMinCorner().y), 
											static_cast<float>(in_vMin.z * m_vMetersPerDWORD.z + GetMinCorner().z) );
	DWORD dwWide = DWORD(-1) >> in_btDepth;
	Vector3D<float> vWide = Vector3D<float>(float(dwWide * m_vMetersPerDWORD.x), float(dwWide * m_vMetersPerDWORD.y), float(dwWide * m_vMetersPerDWORD.z));
	Vector3D<float> vMax = vMin + vWide;
	out_AABB.Set(vMin, vMax);
}

void COkTree::GetAABBByChildNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth, BYTE in_btChildIndex, Vector3D<DWORD>& out_vNewMin, CBoundBox<float>& out_AABB)
{
	////_mm_empty();
	assert_debug( in_btChildIndex < 8, L"" );

	//DWORD dwNewDepth = in_dwDepth + 1;
	in_btDepth++;
	DWORD dwWidth = DWORD(-1) >> in_btDepth;

	BYTE btMulX = (in_btChildIndex & 1);
	BYTE btMulY = ((in_btChildIndex & 2) >> 1);
	BYTE btMulZ = ((in_btChildIndex & 4) >> 2);

	out_vNewMin.x = in_vMin.x + dwWidth * btMulX;
	out_vNewMin.y = in_vMin.y + dwWidth * btMulY;
	out_vNewMin.z = in_vMin.z + dwWidth * btMulZ;

	Vector3D<float> vWide = Vector3D<float>(float(dwWidth * m_vMetersPerDWORD.x), float(dwWidth * m_vMetersPerDWORD.y), float(dwWidth * m_vMetersPerDWORD.z));

	Vector3D<float> vMin = Vector3D<float>( static_cast<float>(in_vMin.x * m_vMetersPerDWORD.x + GetMinCorner().x), 
											static_cast<float>(in_vMin.y * m_vMetersPerDWORD.y + GetMinCorner().y), 
											static_cast<float>(in_vMin.z * m_vMetersPerDWORD.z + GetMinCorner().z) );

	vMin.x += vWide.x * btMulX;
	vMin.y += vWide.y * btMulY;
	vMin.z += vWide.z * btMulZ;

	Vector3D<float> vMax = vMin + vWide;

	out_AABB.Set(vMin, vMax);

	//GetAABBByNode(out_vNewMin, dwNewDepth, out_AABB);
}

DWORD COkTree::GetMinByChildNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth, BYTE in_btChildIndex, Vector3D<DWORD>& out_vNewMin)
{
	assert_debug( in_btChildIndex < 8, L"" );

	//DWORD dwNewDepth = in_dwDepth + 1;
	in_btDepth++;
	DWORD dwWidth = DWORD(-1) >> in_btDepth;

	BYTE btMulX = (in_btChildIndex & 1);
	BYTE btMulY = ((in_btChildIndex & 2) >> 1);
	BYTE btMulZ = ((in_btChildIndex & 4) >> 2);

	out_vNewMin.x = in_vMin.x + dwWidth * btMulX;
	out_vNewMin.y = in_vMin.y + dwWidth * btMulY;
	out_vNewMin.z = in_vMin.z + dwWidth * btMulZ;

	return dwWidth;
}

void COkTree::DrawNode(DWORD in_dwX, DWORD in_dwY, DWORD in_dwZ, DWORD in_dwDepth, COkNode* in_pCurNode)
{
//	if( in_dwDepth > 22 )
//		return;
//
//	Vector3D<float> vMin = Vector3D<float>( float(in_dwX * m_vMetersPerDWORD.x), float(in_dwY * m_vMetersPerDWORD.y), float(in_dwZ * m_vMetersPerDWORD.z) );
//	vMin += Vector3D<float>( GetMinCorner() ) ;
//	DWORD dwWide = DWORD(-1) >> in_dwDepth;
//
//	bool bChildPresent = false;
//	DWORD dwWidth = dwWide >> 1;
//	for(BYTE bt = 0; bt < 8; bt++)
//	{
//		if( in_pCurNode->m_ppChildren[bt] )
//		{
//			bChildPresent = true;
//
//			DWORD dwNewX = in_dwX + dwWidth * (bt & 1);
//			DWORD dwNewY = in_dwY + dwWidth * ((bt & 2) >> 1);
//			DWORD dwNewZ = in_dwZ + dwWidth * ((bt & 4) >> 2);
//			DrawNode( dwNewX, dwNewY, dwNewZ, in_dwDepth + 1, in_pCurNode->m_ppChildren[bt] );
//		}
//	}
//
//	//if( !bChildPresent )
//	{
//		Vector3D<float> vWide = Vector3D<float>(float(dwWide * m_vMetersPerDWORD.x), float(dwWide * m_vMetersPerDWORD.y), float(dwWide * m_vMetersPerDWORD.z));
//		Vector3D<float> vMax = vMin + vWide;
//		CBoundBox<float> BB(vMin, vMax);
//		DrawDebugBoundBox(BB);
//
//		char str[10];
//		Vector3D<float> vPS = WorldToScreen( vMax );
//		if( vPS.z > 0 )
//		{
//			sprintf(str, "%d", in_dwDepth);
//			DrawDebugText( int(vPS.x), int(vPS.y), D3D_COLOR_RED, str);
//		}
//	}
}

void COkTree::DrawDebug()
{
	//DRAW OkTree
	DrawNode(0, 0, 0, 0, m_pRootNode);

//	char str[1000];
//	DWORD dwShift = 0;

	//DRAW Iterator
/*	StartIterator( CBoundBox( Vector3D(5), Vector3D(24) ) );
#ifdef _DEBUG
	g_pApp->GetDirect3DWrapper()->DrawDebugBoundBox(m_IteratorInfo.m_BoundBox, D3D_COLOR_BLUE);
#endif //_DEBUG

	DWORD dwObject = 0;
	while ( GetNextObject( dwObject ) )
	{
		STestingOkTreeObject* pTO = (STestingOkTreeObject*)dwObject;
		sprintf(str, "%d", pTO->m_pDummy);
		g_pApp->GetDirect3DWrapper()->DrawDebugText( 300 + dwShift, 500, D3D_COLOR_BLUE, str);
		dwShift += 20;
	}
*/
}

COkNode* COkTree::GetNode(const Vector3D<DWORD>& in_vMin, BYTE in_btDepth) 
{ 
	DWORD dwIndex;
	COkNode* pN = m_stNodes.AddEntries(&dwIndex);
	MMX_ASSERT( pN && dwIndex != DWORD(-1) );
	pN->m_dwIndexInStore = dwIndex;
	OnCreateNode(pN, in_vMin, in_btDepth);
	return pN;
}

void COkTree::RemoveNode(COkNode* in_pNode)
{
	MMX_ASSERT( in_pNode && in_pNode->m_dwIndexInStore != DWORD(-1) );

	OnDeleteNode(in_pNode);
	m_stNodes.RemoveAt( in_pNode->m_dwIndexInStore );
}

DWORD GetObjectCountInNode(COkNode* in_pNode, bool in_bCommonLevel)
{
	MMX_ASSERT( in_pNode );

	DWORD dwCount = 0;

	SObjectListElem* pTLE = NULL;
	if( in_bCommonLevel )
		pTLE = in_pNode->m_pCommonLevelObjects;
	else
		pTLE = in_pNode->m_pOwnObject;

	while(pTLE) 
	{
		dwCount++;
		pTLE = pTLE->m_pNext;
	}
	return dwCount;
}

bool COkTree::IsObjectInNode(COkNode* in_pNode, UINT_PTR in_dwObject, bool in_bCommonLevel) const
{
	MMX_ASSERT( in_pNode && in_dwObject );

	SObjectListElem* pTLE = NULL;
	if( in_bCommonLevel )
		pTLE = in_pNode->m_pCommonLevelObjects;
	else
		pTLE = in_pNode->m_pOwnObject;

	while(pTLE) 
	{
		const SObjectInfo* pOI = GetObjectInfoByIndex(pTLE->m_dwObjectIndex);
		if( pOI->m_dwObject == in_dwObject )
			return true;

		pTLE = pTLE->m_pNext;
	}
	return false;
}

bool COkTree::DeleteObjectFromNodeList(COkNode* in_pNode, UINT_PTR in_dwObject, DWORD& io_dwDeletedObjectIndex, bool in_bCommonLevel)
{
	MMX_ASSERT( in_dwObject );
	MMX_ASSERT( in_pNode && (in_pNode->m_pOwnObject && !in_bCommonLevel || in_pNode->m_pCommonLevelObjects && in_bCommonLevel ) );
	MMX_ASSERT( IsObjectInNode(in_pNode, in_dwObject, in_bCommonLevel) );

#ifdef GRID_ASSERTS
	DWORD dwCount = GetObjectCountInNode(in_pNode, in_bCommonLevel);
#endif //_DEBUG

	SObjectListElem* pTLE = NULL;
	if( in_bCommonLevel )
		pTLE = in_pNode->m_pCommonLevelObjects;
	else
		pTLE = in_pNode->m_pOwnObject;

	SObjectListElem* pPrevTLE = NULL;

	while( pTLE )
	{
		UINT_PTR dwObject = GetObjectInfoByIndex(pTLE->m_dwObjectIndex)->m_dwObject;
		if( dwObject == in_dwObject )
		{
			if( pPrevTLE )
				pPrevTLE->m_pNext = pTLE->m_pNext;
			else
			{
				if( in_bCommonLevel )
					in_pNode->m_pCommonLevelObjects = pTLE->m_pNext;
				else
					in_pNode->m_pOwnObject = pTLE->m_pNext;
			}

			if( io_dwDeletedObjectIndex == DWORD(-1) )
			{
				io_dwDeletedObjectIndex = pTLE->m_dwObjectIndex;
			}
			else
			{
				assert_debug(io_dwDeletedObjectIndex == pTLE->m_dwObjectIndex, L"");
			}

			m_stObjectListElems.RemoveAt( pTLE->m_dwIndexInStore );
#ifdef GRID_ASSERTS
			MMX_ASSERT( dwCount == GetObjectCountInNode(in_pNode, in_bCommonLevel) + 1 );
#endif //_DEBUG
			return true;
		}
		else
		{
			pPrevTLE = pTLE;
			pTLE = pTLE->m_pNext;
		}
	}
#ifdef GRID_ASSERTS
	MMX_ASSERT( dwCount == GetObjectCountInNode(in_pNode, in_bCommonLevel) );
#endif //_DEBUG
	return false;
}

void COkTree::AddObjectToNodeList(COkNode* in_pNode, DWORD in_dwObjectIndex, bool in_bCommonLevel)
{
#ifdef GRID_ASSERTS
	DWORD dwCount = GetObjectCountInNode(in_pNode, in_bCommonLevel);
#endif //_DEBUG

	assert_debug(in_dwObjectIndex < MAX_OBJECT_COUNT, L"");

	DWORD dwIndex;
	SObjectListElem* pObjectListElem = m_stObjectListElems.AddEntries(&dwIndex);
	pObjectListElem->m_dwIndexInStore = dwIndex;
	pObjectListElem->m_dwObjectIndex = in_dwObjectIndex;

	if( in_bCommonLevel )
	{
		pObjectListElem->m_pNext = in_pNode->m_pCommonLevelObjects;
		in_pNode->m_pCommonLevelObjects = pObjectListElem;
	}
	else
	{
		pObjectListElem->m_pNext = in_pNode->m_pOwnObject;
		in_pNode->m_pOwnObject = pObjectListElem;
	}

#ifdef GRID_ASSERTS
	MMX_ASSERT( dwCount + 1 == GetObjectCountInNode(in_pNode, in_bCommonLevel) );
#endif //_DEBUG
}

bool COkTree::CheckCoord(const Vector3D<float>& in_vPoint) const
{
	//return false;
/*	Vector3D<double> vLink = (Vector3D<double>)in_vPoint - GetMinCorner();
	if( vLink.x < 0 || vLink.y < 0 || vLink.z < 0 )
		return false;
	vLink = GetMaxCorner() - (Vector3D<double>)in_vPoint;

	if( vLink.x < 0 || vLink.y < 0 || vLink.z < 0 )
		return false;

	return true;*/
	//такой вариант в три раза быстрее
	return in_vPoint.x >= GetMinCorner().x && in_vPoint.x <= GetMaxCorner().x
		&& in_vPoint.y >= GetMinCorner().y && in_vPoint.y <= GetMaxCorner().y
		&& in_vPoint.z >= GetMinCorner().z && in_vPoint.z <= GetMaxCorner().z;
}

bool COkTree::GetCoordInCell(const Vector3D<float>& in_vPoint, __m64& out_mm64Coord) const
{
	bool bRes = CheckCoord(in_vPoint);
	assert_debug( bRes, L"" );
	if( !bRes )
		return false;

	Vector3D<double> vLink = static_cast< Vector3D<double> >(in_vPoint) - GetMinCorner();

	DWORD dwCoordMin[3];

	dwCoordMin[0] = static_cast<DWORD>(vLink.x * m_vDWORDperMeter.x);
	dwCoordMin[1] = static_cast<DWORD>(vLink.y * m_vDWORDperMeter.y);
	dwCoordMin[2] = static_cast<DWORD>(vLink.z * m_vDWORDperMeter.z);

	SDwordByByte HAdr;
	SDwordByByte MAdr;
	SDwordByByte LAdr;
	HAdr.dwDword = pdwCoordToOct8[ BYTE(dwCoordMin[0] >> 24) ]; //старшие разряды правее, верхний уровень
	HAdr.dwDword |= pdwCoordToOct8[ BYTE(dwCoordMin[1] >> 24) ] << 1;
	HAdr.dwDword |= pdwCoordToOct8[ BYTE(dwCoordMin[2] >> 24) ] << 2;
	MAdr.dwDword = pdwCoordToOct8[ BYTE(dwCoordMin[0] >> 16) ];
	MAdr.dwDword |= pdwCoordToOct8[ BYTE(dwCoordMin[1] >> 16) ] << 1;
	MAdr.dwDword |= pdwCoordToOct8[ BYTE(dwCoordMin[2] >> 16) ] << 2;
	LAdr.dwDword = pdwCoordToOct5[ (dwCoordMin[0] >> 11) & 0x1F ];
	LAdr.dwDword |= pdwCoordToOct5[ (dwCoordMin[1] >> 11) & 0x1F ] << 1;
	LAdr.dwDword |= pdwCoordToOct5[ (dwCoordMin[2] >> 11) & 0x1F ] << 2;

	SDwordByByteTo64(HAdr, MAdr, LAdr, out_mm64Coord);

	return true;
}

DWORD COkTree::GetSizeInDWORD(float in_fSize)
{
	DWORD dwX = static_cast<DWORD>(in_fSize * m_vDWORDperMeter.x);
	DWORD dwY = static_cast<DWORD>(in_fSize * m_vDWORDperMeter.y);
	DWORD dwZ = static_cast<DWORD>(in_fSize * m_vDWORDperMeter.z);

	if( dwX > dwY )
	{
		if( dwX > dwZ )
			return dwX;
		else
			return dwZ;
	}
	else
	{
		if( dwY > dwZ )
			return dwY;
		else
			return dwZ;
	}
}

bool COkTree::GetCoordInCell(const CBoundBox<float>& in_BoundBox, __m64& out_mm64Min, __m64& out_mm64Max, DWORD* out_pdwMaxObjectSize, CBoundBox<DWORD>* out_pdwRect) const
{
	Vector3D<float> vMin = in_BoundBox.m_vMin;
	Vector3D<float> vMax = in_BoundBox.m_vMax;

	bool bRes = CheckCoord(vMin);
	assert_debug( bRes, L"" );
	if( !bRes )
		return false;

	bRes = CheckCoord(vMax);
	assert_debug( bRes, L"" );
	if( !bRes )
		return false;

	Vector3D<double> vLink = (Vector3D<double>)vMin - GetMinCorner();

	CBoundBox<DWORD> dwBoundBox;
	dwBoundBox.m_vMin.x = static_cast<DWORD>(vLink.x * m_vDWORDperMeter.x);
	dwBoundBox.m_vMin.y = static_cast<DWORD>(vLink.y * m_vDWORDperMeter.y);
	dwBoundBox.m_vMin.z = static_cast<DWORD>(vLink.z * m_vDWORDperMeter.z);

//#ifdef _DEBUG
//	Trace( L"%u: %f; %f \n ", dwBoundBox.m_vMin.x, vLink.x * m_vDWORDperMeter.x, dwBoundBox.m_vMin.x - vLink.x * m_vDWORDperMeter.x);
////	assert_andrey( vLink.x * m_vDWORDperMeter.x < UINT_MAX)
//#endif //_DEBUG



	vLink = Vector3D<double>(vMax) - GetMinCorner();
	assert_debug( vLink.x >= 0 && vLink.y >= 0 && vLink.z >= 0, L"" );

	dwBoundBox.m_vMax.x = static_cast<DWORD>(vLink.x * m_vDWORDperMeter.x);
	dwBoundBox.m_vMax.y = static_cast<DWORD>(vLink.y * m_vDWORDperMeter.y);
	dwBoundBox.m_vMax.z = static_cast<DWORD>(vLink.z * m_vDWORDperMeter.z);

	if( out_pdwRect )
	{
		*out_pdwRect = dwBoundBox;
	}
	SDwordByByte HAdr;
	SDwordByByte MAdr;
	SDwordByByte LAdr;
	HAdr.dwDword = pdwCoordToOct8[ BYTE(dwBoundBox.m_vMin.x >> 24) ]; //старшие разряды правее, верхний уровень
	HAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMin.y >> 24) ] << 1;
	HAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMin.z >> 24) ] << 2;
	MAdr.dwDword = pdwCoordToOct8[ BYTE(dwBoundBox.m_vMin.x >> 16) ];
	MAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMin.y >> 16) ] << 1;
	MAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMin.z >> 16) ] << 2;
	LAdr.dwDword = pdwCoordToOct5[ (dwBoundBox.m_vMin.x >> 11) & 0x1F ];
	LAdr.dwDword |= pdwCoordToOct5[ (dwBoundBox.m_vMin.y >> 11) & 0x1F ] << 1;
	LAdr.dwDword |= pdwCoordToOct5[ (dwBoundBox.m_vMin.z >> 11) & 0x1F ] << 2;

	SDwordByByteTo64(HAdr, MAdr, LAdr, out_mm64Min);

	HAdr.dwDword = pdwCoordToOct8[ BYTE(dwBoundBox.m_vMax.x >> 24) ]; //старшие разряды правее, верхний уровень
	HAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMax.y >> 24) ] << 1;
	HAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMax.z >> 24) ] << 2;
	MAdr.dwDword = pdwCoordToOct8[ BYTE(dwBoundBox.m_vMax.x >> 16) ];
	MAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMax.y >> 16) ] << 1;
	MAdr.dwDword |= pdwCoordToOct8[ BYTE(dwBoundBox.m_vMax.z >> 16) ] << 2;
	LAdr.dwDword = pdwCoordToOct5[ (dwBoundBox.m_vMax.x >> 11) & 0x1F ];
	LAdr.dwDword |= pdwCoordToOct5[ (dwBoundBox.m_vMax.y >> 11) & 0x1F ] << 1;
	LAdr.dwDword |= pdwCoordToOct5[ (dwBoundBox.m_vMax.z >> 11) & 0x1F ] << 2;

	SDwordByByteTo64(HAdr, MAdr, LAdr, out_mm64Max);

	Vector3D<DWORD> vObjectSize = dwBoundBox.m_vMax - dwBoundBox.m_vMin;

	if( out_pdwMaxObjectSize )
	{
		if( vObjectSize.x > vObjectSize.y )
		{
			if( vObjectSize.x > vObjectSize.z )
				*out_pdwMaxObjectSize = vObjectSize.x;
			else
				*out_pdwMaxObjectSize = vObjectSize.z;
		}
		else
		{
			if( vObjectSize.y > vObjectSize.z )
				*out_pdwMaxObjectSize = vObjectSize.y;
			else
				*out_pdwMaxObjectSize = vObjectSize.z;
		}
	}

	return true;
}

void COkTree::CreateAndCollectNode(STreeData& in_TreeData, BYTE in_btMinDepth, COkNodeVector& out_OkNodeVector )
{
	//TTmpStack& aStack = *GetTmpStack();	
	//assert_kirillk(&aStack != 0);
	//MMX_ASSERT(aStack.empty());

	SOkNodeStackData buffer[k_TmpStackSize];
	TTmpStack aStack(buffer, ARRAY_SIZE(buffer));
	goto lblInSet;

	while( ! aStack.empty() )
	{
		SOkNodeStackData* pNSD = &aStack.back();

		in_TreeData.pNode = pNSD->m_pNode;
		in_TreeData.mm64MinCur = pNSD->m_mm64MinCoord;
		in_TreeData.mm64MaxCur = pNSD->m_mm64MaxCoord;
		in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
		in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
		in_TreeData.btDepth = pNSD->m_btDepth;
		in_TreeData.vMin = pNSD->m_vMin;

		aStack.pop_back();
lblInSet:
		bool bNoDown = in_TreeData.btDepth >= in_btMinDepth;
		if( bNoDown )
		{
			//			Trace( "SetInTree. Object: %d; Node: 0x%08x; NodeSize: %d; \n", in_dwObject, DWORD(in_TreeData.pNode), in_TreeData.btDepth );
			out_OkNodeVector.push_back(in_TreeData.pNode);
		}
		else
		{
			if( in_TreeData.btMinCell == in_TreeData.btMaxCell )
			{
				Vector3D<DWORD> vNewMin;
				GetMinByChildNode(in_TreeData.vMin, in_TreeData.btDepth, in_TreeData.btMinCell, vNewMin);

				if( !in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell] )
					in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell] = GetNode(vNewMin, in_TreeData.btDepth + 1);

				COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell];
				MMX_ASSERT( pChildNode );

				in_TreeData.pNode = pChildNode;
				in_TreeData.btMinCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MinCur);
				in_TreeData.btMaxCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MaxCur);
				in_TreeData.btDepth++;
				in_TreeData.vMin = vNewMin;
				MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

				goto lblInSet;
			}
			else
			{
				//Split
				BYTE dwSplitToChildren = pbyOctToChild[in_TreeData.btMinCell][in_TreeData.btMaxCell];
				MMX_ASSERT(dwSplitToChildren != 0);
				do 
				{
					int iChild = GetHighestSetBit(dwSplitToChildren);
					dwSplitToChildren &= pdwClearBit[iChild];

					Vector3D<DWORD> vNewMin;
					GetMinByChildNode(in_TreeData.vMin, in_TreeData.btDepth, BYTE(iChild), vNewMin);

					if( !in_TreeData.pNode->m_ppChildren[iChild] )
						in_TreeData.pNode->m_ppChildren[iChild] = GetNode(vNewMin, in_TreeData.btDepth + 1);

					COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[iChild];
					MMX_ASSERT( pChildNode );

					if( dwSplitToChildren )
					{						
						SOkNodeStackData* pStackElem = aStack.AddEntries();

						pStackElem->m_pNode = pChildNode;
						pStackElem->m_btDepth = in_TreeData.btDepth + 1;
						pStackElem->m_vMin = vNewMin;
						MMX_ASSERT( pStackElem->m_btDepth < g_btDepthCount );

						SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, pStackElem->m_mm64MinCoord, pStackElem->m_mm64MaxCoord);
					}
					else
					{
						in_TreeData.pNode = pChildNode;
						SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur);
						in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
						in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
						in_TreeData.btDepth++;
						in_TreeData.vMin = vNewMin;
						MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

						goto lblInSet;
					}
				} 
				while(true);
			}
		}
	}

	////_mm_empty();
	//FreeTmpStack(&aStack);
}

void COkTree::SetInTree(DWORD in_dwObjectIndex, STreeData& in_TreeData, BYTE in_btMinDepth)
{
	//TTmpStack& aStack = *GetTmpStack();	
	//assert_kirillk(&aStack != 0);
	//MMX_ASSERT(aStack.empty());

	SOkNodeStackData buffer[k_TmpStackSize];
	TTmpStack aStack(buffer, ARRAY_SIZE(buffer));

goto lblInSet;
	
	while( ! aStack.empty() )
	{
		SOkNodeStackData* pNSD = &aStack.back();

		in_TreeData.pNode = pNSD->m_pNode;
		in_TreeData.mm64MinCur = pNSD->m_mm64MinCoord;
		in_TreeData.mm64MaxCur = pNSD->m_mm64MaxCoord;
		in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
		in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
		in_TreeData.btDepth = pNSD->m_btDepth;
		in_TreeData.vMin = pNSD->m_vMin;

		aStack.pop_back();
lblInSet:

		if( m_bCommonLevelApply && m_btCommonDepth == in_TreeData.btDepth )
		{
			AddObjectToNodeList(in_TreeData.pNode, in_dwObjectIndex, true);
		}

		bool bNoDown = in_TreeData.btDepth >= in_btMinDepth;
		if( bNoDown )
		{
//			Trace( "SetInTree. Object: %d; Node: 0x%08x; NodeSize: %d; \n", in_dwObject, DWORD(in_TreeData.pNode), in_TreeData.btDepth );
			AddObjectToNodeList(in_TreeData.pNode, in_dwObjectIndex, false);
		}
		else
		{
			if( in_TreeData.btMinCell == in_TreeData.btMaxCell )
			{
				Vector3D<DWORD> vNewMin;
				GetMinByChildNode(in_TreeData.vMin, in_TreeData.btDepth, in_TreeData.btMinCell, vNewMin);

				if( !in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell] )
					in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell] = GetNode(vNewMin, in_TreeData.btDepth + 1);

				COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell];
				MMX_ASSERT( pChildNode );

				in_TreeData.pNode = pChildNode;
				in_TreeData.btMinCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MinCur);
				in_TreeData.btMaxCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MaxCur);
				in_TreeData.btDepth++;
				in_TreeData.vMin = vNewMin;
				MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

				goto lblInSet;
			}
			else
			{
				//Split
				BYTE dwSplitToChildren = pbyOctToChild[in_TreeData.btMinCell][in_TreeData.btMaxCell];
				MMX_ASSERT(dwSplitToChildren != 0);
				do 
				{
					int iChild = GetHighestSetBit(dwSplitToChildren);
					dwSplitToChildren &= pdwClearBit[iChild];

					Vector3D<DWORD> vNewMin;
					GetMinByChildNode(in_TreeData.vMin, in_TreeData.btDepth, BYTE(iChild), vNewMin);

					if( !in_TreeData.pNode->m_ppChildren[iChild] )
						in_TreeData.pNode->m_ppChildren[iChild] = GetNode(vNewMin, in_TreeData.btDepth + 1);

					COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[iChild];
					MMX_ASSERT( pChildNode );

					if( dwSplitToChildren )
					{
						SOkNodeStackData* pStackElem = aStack.AddEntries();

						pStackElem->m_pNode = pChildNode;
						pStackElem->m_btDepth = in_TreeData.btDepth + 1;
						pStackElem->m_vMin = vNewMin;
						MMX_ASSERT( pStackElem->m_btDepth < g_btDepthCount );

						SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, pStackElem->m_mm64MinCoord, pStackElem->m_mm64MaxCoord);
					}
					else
					{
						in_TreeData.pNode = pChildNode;
						SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur);
						in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
						in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
						in_TreeData.btDepth++;
						in_TreeData.vMin = vNewMin;
						MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

						goto lblInSet;
					}
				} 
				while(true);
			}
		}
	}

	////_mm_empty();
	//FreeTmpStack(&aStack);
}

template <class Type>
void COkTree::CollectCell(STreeData& in_TreeData, BYTE in_btMinDepth, const CBoundBox<DWORD>& in_dwBoundBox, const CFrustum<float>* in_pFrustum, Type& out_aNodes)
//void COkTree::CollectCell(STreeData& in_TreeData, BYTE in_btMinDepth, const Plane* in_pFrustum, CIteratorDataVector& out_aNodes)
{
	//TTmpStack& aStack = *GetTmpStack();	
	//assert_kirillk(&aStack != 0);
	//MMX_ASSERT(aStack.empty());

	SOkNodeStackData buffer[k_TmpStackSize];
	TTmpStack aStack(buffer, ARRAY_SIZE(buffer));

	//TC_AUTO( tcOkTreeCollectCell );

	goto lblInCollection;

	while( ! aStack.empty() )
	{
		SOkNodeStackData* pNSD = &aStack.back();

		in_TreeData.pNode = pNSD->m_pNode;
		in_TreeData.mm64MinCur = pNSD->m_mm64MinCoord;
		in_TreeData.mm64MaxCur = pNSD->m_mm64MaxCoord;
		in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
		in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
		in_TreeData.btDepth = pNSD->m_btDepth;
		in_TreeData.vMin = pNSD->m_vMin;
		in_TreeData.btCrossPlaneFlag = pNSD->m_btCrossPlaneFlag;

		aStack.pop_back();

lblInCollection:

		bool bNoDown = in_TreeData.btDepth >= in_btMinDepth;
		if( !bNoDown )
		{
			if( in_TreeData.btMinCell == in_TreeData.btMaxCell )
			{
				COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell];
				if( pChildNode )
				{
					bool bAdd = true;

					Vector3D<DWORD> vNewMin;
					BYTE btCrossPlaneFlag = in_TreeData.btCrossPlaneFlag;
					if( btCrossPlaneFlag )
					{
						if( in_pFrustum )
						{
							CBoundBox<float> AABB;
							GetAABBByChildNode(in_TreeData.vMin, in_TreeData.btDepth, in_TreeData.btMinCell, vNewMin, AABB);
							bAdd = IsAABBInFrustum(AABB, *in_pFrustum, btCrossPlaneFlag);
						}
						else if( m_bCommonLevelApply )
						{
							DWORD dwChildWidth = GetMinByChildNode(in_TreeData.vMin, in_TreeData.btDepth, in_TreeData.btMinCell, vNewMin);
							Vector3D<DWORD> vNewMax = vNewMin + Vector3D<DWORD>(dwChildWidth);

							bool bCellInside = vNewMin.x >= in_dwBoundBox.m_vMin.x &&
												vNewMin.y >= in_dwBoundBox.m_vMin.y &&
												vNewMin.z >= in_dwBoundBox.m_vMin.z &&
												vNewMax.x <= in_dwBoundBox.m_vMax.x &&
												vNewMax.y <= in_dwBoundBox.m_vMax.y &&
												vNewMax.z <= in_dwBoundBox.m_vMax.z;
							if( bCellInside )
								btCrossPlaneFlag = 0;
						}
					}

					if( bAdd )
					{
						BYTE btChildDepth = in_TreeData.btDepth + 1;
						if( pChildNode->m_pOwnObject || (pChildNode->m_pCommonLevelObjects && btCrossPlaneFlag == 0) )
							out_aNodes.push_back( SNodeForIterator(pChildNode, btCrossPlaneFlag, btChildDepth) );

						bool bInDepth = !m_bCommonLevelApply || (btCrossPlaneFlag > 0 || btChildDepth != m_btCommonDepth);

						if( bInDepth )
						{
							in_TreeData.pNode = pChildNode;
							in_TreeData.btMinCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MinCur);
							in_TreeData.btMaxCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MaxCur);
							in_TreeData.btDepth++;
							in_TreeData.vMin = vNewMin;
							in_TreeData.btCrossPlaneFlag = btCrossPlaneFlag;
							MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

							goto lblInCollection;
						}
					}
				}
			}
			else
			{
				//Split
				BYTE dwSplitToChildren = pbyOctToChild[in_TreeData.btMinCell][in_TreeData.btMaxCell];
				COkNode* pChildNode = NULL;
				MMX_ASSERT(dwSplitToChildren != 0);
				do 
				{
					int iChild = GetHighestSetBit(dwSplitToChildren);
					dwSplitToChildren &= pdwClearBit[iChild];

					pChildNode = in_TreeData.pNode->m_ppChildren[iChild];
					if( pChildNode )
					{
						bool bAdd = true;

						Vector3D<DWORD> vNewMin;
						BYTE btCrossPlaneFlag = in_TreeData.btCrossPlaneFlag;
						if( btCrossPlaneFlag )
						{
							if( in_pFrustum )
							{
								CBoundBox<float> AABB;
								GetAABBByChildNode(in_TreeData.vMin, in_TreeData.btDepth, BYTE(iChild), vNewMin, AABB);
								bAdd = IsAABBInFrustum(AABB, *in_pFrustum, btCrossPlaneFlag);
							}
							else if( m_bCommonLevelApply )
							{
								DWORD dwChildWidth = GetMinByChildNode(in_TreeData.vMin, in_TreeData.btDepth, BYTE(iChild), vNewMin);
								Vector3D<DWORD> vNewMax = vNewMin + Vector3D<DWORD>(dwChildWidth);

								bool bCellInside = vNewMin.x >= in_dwBoundBox.m_vMin.x &&
									vNewMin.y >= in_dwBoundBox.m_vMin.y &&
									vNewMin.z >= in_dwBoundBox.m_vMin.z &&
									vNewMax.x <= in_dwBoundBox.m_vMax.x &&
									vNewMax.y <= in_dwBoundBox.m_vMax.y &&
									vNewMax.z <= in_dwBoundBox.m_vMax.z;
								if( bCellInside )
									btCrossPlaneFlag = 0;
							}
						}

						if( bAdd )
						{
							BYTE btChildDepth = in_TreeData.btDepth + 1;
							if( pChildNode->m_pOwnObject || (pChildNode->m_pCommonLevelObjects && btCrossPlaneFlag == 0) )
								out_aNodes.push_back( SNodeForIterator(pChildNode, btCrossPlaneFlag, btChildDepth) );

							bool bInDepth = !m_bCommonLevelApply || (btCrossPlaneFlag > 0 || btChildDepth != m_btCommonDepth);

							if( bInDepth )
							{
								if( dwSplitToChildren )
								{
									SOkNodeStackData* pStackElem = aStack.AddEntries();

									pStackElem->m_pNode = pChildNode;
									pStackElem->m_btDepth = btChildDepth;
									pStackElem->m_vMin = vNewMin;
									pStackElem->m_btCrossPlaneFlag = btCrossPlaneFlag;
									MMX_ASSERT( pStackElem->m_btDepth < g_btDepthCount );

									SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, pStackElem->m_mm64MinCoord, pStackElem->m_mm64MaxCoord);
								}
								else
								{
									in_TreeData.pNode = pChildNode;
									SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur);
									in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
									in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
									in_TreeData.btDepth++;
									in_TreeData.vMin = vNewMin;
									in_TreeData.btCrossPlaneFlag = btCrossPlaneFlag;
									MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

									goto lblInCollection;
								}
							}
						}
					}
				} 
				while(dwSplitToChildren);
			}
		}
	}

	////_mm_empty();
	//FreeTmpStack(&aStack);	
}

void COkTree::RemoveFromTree(UINT_PTR in_dwObject, STreeData& in_TreeData, BYTE in_btMinDepth, DWORD& io_dwDeletedObjectIndex)
{
	//TTmpStack& aStack = *GetTmpStack();
	//assert_kirillk(&aStack != 0);
	SOkNodeStackData buffer[k_TmpStackSize];
	TTmpStack aStack(buffer, ARRAY_SIZE(buffer));

	SDelNodeData bufferDelNode[k_DelQueueSize];
	TTmpDelQueue aDelQueue(bufferDelNode, ARRAY_SIZE(bufferDelNode));

	//TTmpDelQueue& aDelQueue = *GetTmpDelQueue();
	//assert_kirillk(&aDelQueue != 0);

	//MMX_ASSERT(aStack.empty());
	//MMX_ASSERT(aDelQueue.empty());

	size_t btLastIndexForSpaceTube = -1;

	goto lblInRemove;

	while( ! aStack.empty() )
	{
		SOkNodeStackData* pNSD = &aStack.back();

		in_TreeData.pNode = pNSD->m_pNode;
		in_TreeData.mm64MinCur = pNSD->m_mm64MinCoord;
		in_TreeData.mm64MaxCur = pNSD->m_mm64MaxCoord;
		in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
		in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
		in_TreeData.btDepth = pNSD->m_btDepth;

		aStack.pop_back();

lblInRemove:
		bool bNoDown = in_TreeData.btDepth >= in_btMinDepth;

		if( m_bCommonLevelApply && m_btCommonDepth == in_TreeData.btDepth )
		{
			DeleteObjectFromNodeList(in_TreeData.pNode, in_dwObject, io_dwDeletedObjectIndex, true);
		}

		if( bNoDown )
		{
//			Trace( "DeleteObjectFromNodeList. Object: %d; Node: 0x%08x; in_TreeData.btDepth: %d \n ", in_dwObject, DWORD(in_TreeData.pNode), in_TreeData.btDepth );
			bool bRes = DeleteObjectFromNodeList(in_TreeData.pNode, in_dwObject, io_dwDeletedObjectIndex, false);
			assert_debug( bRes, L"" );
		}
		else
		{
			MMX_ASSERT( in_TreeData.pNode->IsChildrenPresent() );

			if( in_TreeData.btMinCell == in_TreeData.btMaxCell )
			{
				COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[in_TreeData.btMinCell];
				MMX_ASSERT( pChildNode );

				SDelNodeData* pDelNodeData = aDelQueue.AddEntries();
				pDelNodeData->m_pNode = pChildNode;
				pDelNodeData->m_pParent = in_TreeData.pNode;
				pDelNodeData->m_btIndexInParent = in_TreeData.btMinCell;

				in_TreeData.pNode = pChildNode;
				in_TreeData.btMinCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MinCur);
				in_TreeData.btMaxCell = ShearRight3BitAndGetLow3BitsFromM64(in_TreeData.mm64MaxCur);
				in_TreeData.btDepth++;
				MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

				goto lblInRemove;
			}
			else
			{
				if( btLastIndexForSpaceTube == -1 )
				{
					assert_debug( aDelQueue.size() < g_btDepthCount, L"" );
					btLastIndexForSpaceTube = aDelQueue.size();
				}

				//Split
#ifdef GRID_ASSERTS
				BYTE dwCount = 0;
#endif //_DEBUG
				BYTE dwSplitToChildren = pbyOctToChild[in_TreeData.btMinCell][in_TreeData.btMaxCell];
				MMX_ASSERT(dwSplitToChildren != 0);
				do 
				{

#ifdef GRID_ASSERTS
					MMX_ASSERT( dwCount < 8 );
#endif
					int iChild = GetHighestSetBit(dwSplitToChildren);
					dwSplitToChildren &= pdwClearBit[iChild];

					COkNode* pChildNode = in_TreeData.pNode->m_ppChildren[iChild];
					MMX_ASSERT( pChildNode );

					SDelNodeData* pDelNodeData = aDelQueue.AddEntries();
					pDelNodeData->m_pNode = pChildNode;
					pDelNodeData->m_pParent = in_TreeData.pNode;
					pDelNodeData->m_btIndexInParent = iChild;

					if( dwSplitToChildren )
					{
						SOkNodeStackData* pStackElem = aStack.AddEntries();

						pStackElem->m_pNode = pChildNode;
						pStackElem->m_btDepth = in_TreeData.btDepth + 1;
						MMX_ASSERT( pStackElem->m_btDepth < g_btDepthCount );

						SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, pStackElem->m_mm64MinCoord, pStackElem->m_mm64MaxCoord);
					}
					else
					{
						in_TreeData.pNode = pChildNode;
                        SuperShear(iChild, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur, in_TreeData.mm64MinCur, in_TreeData.mm64MaxCur);
						in_TreeData.btMinCell = GetLow3BitsFromInt64(in_TreeData.mm64MinCur.m64_u64);
						in_TreeData.btMaxCell = GetLow3BitsFromInt64(in_TreeData.mm64MaxCur.m64_u64);
						in_TreeData.btDepth++;
						MMX_ASSERT( in_TreeData.btDepth < g_btDepthCount );

						goto lblInRemove;
					}
#ifdef GRID_ASSERTS
					dwCount++;
#endif //_DEBUG
				} 
				while(true);
			}
		}
	}

	while( ! aDelQueue.empty() ) 
	{
		SDelNodeData* pDelNodeData = &aDelQueue.back();

		assert_debug( !IsObjectInNode(pDelNodeData->m_pNode, in_dwObject, false) , L"");

		if( pDelNodeData->m_pNode->m_pOwnObject == NULL && !pDelNodeData->m_pNode->IsChildrenPresent() )
		{
			assert_debug(pDelNodeData->m_pParent && pDelNodeData->m_pParent->m_ppChildren[pDelNodeData->m_btIndexInParent] == pDelNodeData->m_pNode, L"");
			pDelNodeData->m_pParent->m_ppChildren[pDelNodeData->m_btIndexInParent] = NULL;
			RemoveNode(pDelNodeData->m_pNode);
		}
		else if( aDelQueue.size() < btLastIndexForSpaceTube )
		{
			aDelQueue.clear();
			break;
		}

		aDelQueue.pop_back();
	}

	////_mm_empty();
	//FreeTmpStack(&aStack);
	//FreeTmpDelQueue(&aDelQueue);
}

DWORD COkTree::GetMinNodeSize(DWORD in_dwMaxObjectSize)
{
	if( in_dwMaxObjectSize > (DWORD(-1) >> 1) )
		return DWORD(-1);
	DWORD dwMinNodeSize = MAX( in_dwMaxObjectSize << 1, DWORD(-1) >> g_btAdressCount ); //либо два размера Object, минимальный размерр вообще
	return dwMinNodeSize;
}

bool COkTree::CollectNodeByAABB( const CBoundBox<float>& in_BoundBox, COkNodeVector& out_OkNodeVector )
{
	__m64 CoordMin;
	__m64 CoordMax;
	DWORD dwMaxObjectSize;
	bool bRes = GetCoordInCell( in_BoundBox, CoordMin, CoordMax, &dwMaxObjectSize, NULL );

	if( !bRes )
	{
		out_OkNodeVector.push_back(m_pRootNode);
		return true;
	}

	DWORD dwMinNodeSize = GetMinNodeSize( dwMaxObjectSize );

	STreeData TreeData;
	TreeData.pNode = m_pRootNode;
	TreeData.mm64MinCur = CoordMin;
	TreeData.mm64MaxCur = CoordMax;
	TreeData.btMinCell = GetLow3BitsFromInt64(TreeData.mm64MinCur.m64_u64);
	TreeData.btMaxCell = GetLow3BitsFromInt64(TreeData.mm64MaxCur.m64_u64);
	TreeData.btDepth = 0;
	TreeData.vMin = Vector3D<DWORD>(0);
	TreeData.btCrossPlaneFlag = 63; //первые 6 бит

	BYTE btMinDepth = GetDepthForMinNodeSize(dwMinNodeSize);

	CreateAndCollectNode(TreeData, btMinDepth, out_OkNodeVector);
	////_mm_empty();

	return true;
}

bool COkTree::RegisterObject(UINT_PTR in_dwObject, const CBoundBox<float>& in_BoundBox)
{
	// set floating point precision

#ifndef WIN64
	_control87(_PC_53, MCW_PC);
#endif

	//assert_debug( in_dwObject, L"" );

	DWORD dwObjectIndex = GetIndexForNewObject( in_dwObject );

	__m64 CoordMin;
	__m64 CoordMax;
	DWORD dwMaxObjectSize;
	bool bRes = GetCoordInCell( in_BoundBox, CoordMin, CoordMax, &dwMaxObjectSize, NULL );

	DWORD dwMinNodeSize = GetMinNodeSize( dwMaxObjectSize );

#ifdef GRID_ASSERTS
	assert_kirillk_ex(m_mapControl.find(in_dwObject) == m_mapControl.end(), L"Error: Object already existing within grid");
	//m_mapControl.insert(std::make_pair(in_dwObject, SControlData(in_BoundBox, dwMaxObjectSize, dwMinNodeSize, CoordMin, CoordMax)) );
	SControlData cd(in_BoundBox, dwMaxObjectSize, dwMinNodeSize, CoordMin, CoordMax);
	m_mapControl.insert(std::make_pair(in_dwObject, cd) );
#endif //_DEBUG

	if( !bRes )
	{
		AddObjectToNodeList(m_pRootNode, dwObjectIndex, false);
		////_mm_empty();
		return true;
	}

	STreeData TreeData;
	TreeData.pNode = m_pRootNode;
	TreeData.mm64MinCur = CoordMin;
	TreeData.mm64MaxCur = CoordMax;
	TreeData.btMinCell = GetLow3BitsFromInt64(TreeData.mm64MinCur.m64_u64);
	TreeData.btMaxCell = GetLow3BitsFromInt64(TreeData.mm64MaxCur.m64_u64);
	TreeData.btDepth = 0;
	TreeData.vMin = Vector3D<DWORD>(0);
	TreeData.btCrossPlaneFlag = 63; //первые 6 бит

	BYTE btMinDepth = GetDepthForMinNodeSize(dwMinNodeSize);
	
	SetInTree(dwObjectIndex, TreeData, btMinDepth);
	
	////_mm_empty();
	return true;
}

bool COkTree::UnregisterObject( UINT_PTR in_dwObject, const CBoundBox<float>& in_BoundBox )
{
	// set floating point precision
#ifndef WIN64
	_control87(_PC_53, MCW_PC);
#endif

	assert_debug( in_dwObject, L"" );

	__m64 CoordMin;
	__m64 CoordMax;
	DWORD dwMaxObjectSize;
	bool bRes = GetCoordInCell( in_BoundBox, CoordMin, CoordMax, &dwMaxObjectSize, NULL );

	DWORD dwMinNodeSize = GetMinNodeSize( dwMaxObjectSize );

#ifdef GRID_ASSERTS		
	assert_andrey(m_mapControl.find(in_dwObject) != m_mapControl.end());
	const SControlData& ControlData = m_mapControl[in_dwObject];
	assert_andrey( IsEqualBoundBox(ControlData.m_BB, in_BoundBox) );
	assert_andrey( ControlData.m_dwObjectSize == dwMaxObjectSize );
	assert_andrey( ControlData.m_dwNodeSize == dwMinNodeSize );
	m_mapControl.erase(in_dwObject);
#endif //_DEBUG

	if( !bRes )
	{
		DWORD dwDeletedObjectIndex = -1;
		bool bRes = DeleteObjectFromNodeList(m_pRootNode, in_dwObject, dwDeletedObjectIndex, false);
		assert_debug( bRes, L"" );
		return true;
	}

	STreeData TreeData;
	TreeData.pNode = m_pRootNode;
	TreeData.mm64MinCur = CoordMin;
	TreeData.mm64MaxCur = CoordMax;
	TreeData.btMinCell = GetLow3BitsFromInt64(TreeData.mm64MinCur.m64_u64);
	TreeData.btMaxCell = GetLow3BitsFromInt64(TreeData.mm64MaxCur.m64_u64);
	TreeData.btDepth = 0;
	TreeData.vMin = Vector3D<DWORD>(0);
	TreeData.btCrossPlaneFlag = 63; //первые 6 бит

	BYTE btMinDepth = GetDepthForMinNodeSize(dwMinNodeSize);

	DWORD dwDeletedObjectIndex = DWORD(-1);

	RemoveFromTree(in_dwObject, TreeData, btMinDepth, dwDeletedObjectIndex);
	assert_debug( dwDeletedObjectIndex < MAX_OBJECT_COUNT, L"" );

	RemoveObjectByIndex( dwDeletedObjectIndex );

	////_mm_empty();	

	return true;
}

__forceinline bool IsMoveObjectInTree(const __m64& in_m64CoordMinSource, const __m64& in_m64CoordMaxSource, 
						const __m64& in_m64CoordMinDestination, const __m64& in_m64CoordMaxDestination,
						BYTE in_btDepthSource, BYTE in_btDepthDestination)
{
	bool bRes = in_btDepthSource != in_btDepthDestination;
	if( bRes ) //если прописываться придется на разную глубину - перепрописаться обязательно!
		return true;

	bRes = !IsEqualMM64(in_m64CoordMinSource, in_m64CoordMinDestination) || !IsEqualMM64(in_m64CoordMaxSource, in_m64CoordMaxDestination);
	if( bRes ) //может быть разница в слишком глубоком уровне
	{
		__m64 m64Temp1 = in_m64CoordMinSource;
		__m64 m64Temp2 = in_m64CoordMinDestination;
		BYTE btEqDepthForMin = GetEqualDepth(in_m64CoordMinSource, in_m64CoordMinDestination);
		MMX_ASSERT( IsEqualMM64(m64Temp1, in_m64CoordMinSource) );
		MMX_ASSERT( IsEqualMM64(m64Temp2, in_m64CoordMinDestination) );

		m64Temp1 = in_m64CoordMaxSource;
		m64Temp2 = in_m64CoordMaxDestination;
		BYTE btEqDepthForMax = GetEqualDepth(in_m64CoordMaxSource, in_m64CoordMaxDestination);
		MMX_ASSERT( IsEqualMM64(m64Temp1, in_m64CoordMaxSource) );
		MMX_ASSERT( IsEqualMM64(m64Temp2, in_m64CoordMaxDestination) );

		bRes = (btEqDepthForMin < (in_btDepthDestination + 1) ) || (btEqDepthForMax < (in_btDepthDestination + 1) ); 
		//одинаковость старого и нового положения глубже, чем уровень прописывания объекта.
	}

	////_mm_empty();
	return bRes;
}

bool COkTree::MoveObject( UINT_PTR in_dwObject, const CBoundBox<float>& in_bbSource, const CBoundBox<float>& in_bbDestination )
{
	// set floating point precision

#ifndef WIN64
	_control87(_PC_53, MCW_PC);
#endif

//	assert_debug( in_dwObject, L"" );

	__m64 CoordMinSource;
	__m64 CoordMaxSource;
	DWORD dwMaxObjectSizeSource;
	bool bResSource = GetCoordInCell( in_bbSource, CoordMinSource, CoordMaxSource, &dwMaxObjectSizeSource, NULL );

	__m64 CoordMinDestination;
	__m64 CoordMaxDestination;
	DWORD dwMaxObjectSizeDestination;
	bool bResDest = GetCoordInCell( in_bbDestination, CoordMinDestination, CoordMaxDestination, &dwMaxObjectSizeDestination, NULL );

	DWORD dwMinNodeSizeSource = GetMinNodeSize( dwMaxObjectSizeSource );
	DWORD dwMinNodeSizeDestination = GetMinNodeSize( dwMaxObjectSizeDestination );

	BYTE btDepthSource = GetDepthForMinNodeSize(dwMinNodeSizeSource);
	BYTE btDepthDestination = GetDepthForMinNodeSize(dwMinNodeSizeDestination);

#ifdef GRID_ASSERTS
	assert_debug( m_mapControl.find(in_dwObject) != m_mapControl.end(), L"");
	const SControlData& ControlData = m_mapControl[in_dwObject];
	assert_debug( IsEqualBoundBox(ControlData.m_BB, in_bbSource), L"" );
	assert_debug( memcmp(&ControlData.m_qwCoordMin, &CoordMinSource, sizeof(__m64)) == 0, L"");
	assert_debug( memcmp(&ControlData.m_qwCoordMax, &CoordMaxSource, sizeof(__m64)) == 0, L"");
	assert_debug( ControlData.m_dwNodeSize == dwMinNodeSizeSource, L"" );
	assert_debug( ControlData.m_dwObjectSize == dwMaxObjectSizeSource, L"" );
		
	m_mapControl[in_dwObject] = SControlData(in_bbDestination, dwMaxObjectSizeDestination, dwMinNodeSizeDestination, CoordMinDestination, CoordMaxDestination);	
#endif //_DEBUG

	
	if( !bResSource || !bResDest )
	{
		bool bUnreg = UnregisterObject(in_dwObject, in_bbSource);
		assert_debug(bUnreg, L"");
		bool bReg = RegisterObject(in_dwObject, in_bbDestination);
		assert_debug(bReg, L"");
		return true;
	}

	if( IsMoveObjectInTree(CoordMinSource, CoordMaxSource, CoordMinDestination, CoordMaxDestination, btDepthSource, btDepthDestination) )
	{
		//Trace( "MoveObject: %d \n", in_dwObject );
		
		BYTE btMinDepth = MIN(btDepthSource, btDepthDestination);

		COkNode* in_pStartNode = m_pRootNode;
		BYTE btDepth = 0;
		DownByTube(in_pStartNode, btMinDepth, btDepth, CoordMinSource, CoordMaxSource, CoordMinDestination, CoordMaxDestination);

		DWORD dwObjectIndex = DWORD(-1);

		STreeData TreeData;
		TreeData.pNode = in_pStartNode;
		TreeData.mm64MinCur = CoordMinSource;
		TreeData.mm64MaxCur = CoordMaxSource;
		TreeData.btMinCell = GetLow3BitsFromInt64(TreeData.mm64MinCur.m64_u64);
		TreeData.btMaxCell = GetLow3BitsFromInt64(TreeData.mm64MaxCur.m64_u64);
		TreeData.btDepth = btDepth;
		TreeData.vMin = Vector3D<DWORD>(0);
		TreeData.btCrossPlaneFlag = 63; //первые 6 бит
//		Trace( "btDepthSource: %d; \n ", btDepthSource );
		RemoveFromTree(in_dwObject, TreeData, btDepthSource, dwObjectIndex);
		assert_debug( dwObjectIndex < MAX_OBJECT_COUNT, L"" );

		TreeData.pNode = in_pStartNode;
		TreeData.mm64MinCur = CoordMinDestination;
		TreeData.mm64MaxCur = CoordMaxDestination;
		TreeData.btMinCell = GetLow3BitsFromInt64(TreeData.mm64MinCur.m64_u64);
		TreeData.btMaxCell = GetLow3BitsFromInt64(TreeData.mm64MaxCur.m64_u64);
		TreeData.btDepth = btDepth;
//		Trace( "btDepthDestination: %d; \n ", btDepthDestination );
		SetInTree(dwObjectIndex, TreeData, btDepthDestination);
	}

	////_mm_empty();

	return true;
}

//return true, if stop enumerate.
bool COkTree::EnumerateNodeObjectsWithPoint(COkNode* in_pNode, const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis )
{
	assert_debug( in_pNode, L"" );

	SObjectListElem* pTLE = in_pNode->m_pOwnObject;
	while(pTLE) 
	{
		SObjectInfo* pObjectInfo = GetObjectInfoByIndex(pTLE->m_dwObjectIndex);

		bool bStop = CheckObjectByPoint(pObjectInfo->m_dwObject, in_vPoint);
		if( bStop )
		{
			return in_pFunc( reinterpret_cast<IGridObject*>(pObjectInfo->m_dwObject), in_pThis );
		}

		pTLE = pTLE->m_pNext;
	}

	return false;
}

//void COkTree::EnumerateObjectsWithPoint(const Vector3D<float>& in_vPoint)
void COkTree::for_each( const Vector3D<float>& in_vPoint, GridObjectProcess in_pFunc, UINT_PTR in_pThis )
{
	__m64 mm64Origin;
	bool bRes = GetCoordInCell(in_vPoint, mm64Origin);
	assert_debug( bRes, L"" );
	if( !bRes )
		return;
	
	bool bStop = EnumerateNodeObjectsWithPoint(m_pRootNode, in_vPoint, in_pFunc, in_pThis );
	if( bStop )
		return;

	COkNode* pNode = m_pRootNode;
	BYTE btIndexCell = GetLow3BitsFromInt64(mm64Origin.m64_u64);

	COkNode* pChildNode = pNode->m_ppChildren[btIndexCell];

	while( pChildNode )
	{
		//_mm_empty();
		bStop = EnumerateNodeObjectsWithPoint(pChildNode, in_vPoint, in_pFunc, in_pThis );
		if( bStop )
			return;

		pNode = pChildNode;
		btIndexCell = ShearRight3BitAndGetLow3BitsFromM64(mm64Origin);
		pChildNode = pNode->m_ppChildren[btIndexCell];
	}
	//_mm_empty();
}


DWORD COkTree::GetIndexForNewObject(UINT_PTR in_dwObject)
{
	DWORD dwIndex = -1;
	SObjectInfo* pObjectInfo = m_stObjectInfo.AddEntries(&dwIndex);
	pObjectInfo->m_dwObject = in_dwObject;
	pObjectInfo->m_vecIteratorTrace.resize(m_iObjectInfoTraceSize);

	assert_debug(dwIndex < DWORD(-1), L"");
	return dwIndex;
}

void COkTree::RemoveObjectByIndex(DWORD in_dwIndex)
{
	m_stObjectInfo.RemoveAt( in_dwIndex );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Iterator
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template <class Type>
bool COkTree::CollectCell(const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum, DWORD in_dwMinNodeSize, Type& out_aNodes)
{
	__m64 CoordMin;
	__m64 CoordMax;
	CBoundBox<DWORD> dwBoundBox;
	bool bRes = GetCoordInCell( in_BoundBox, CoordMin, CoordMax, NULL, &dwBoundBox );
	assert_debug( bRes, L"" );
	if( !bRes )
		return false;

	if( m_pRootNode->m_pOwnObject )
		out_aNodes.push_back( SNodeForIterator(m_pRootNode, 63, 0) );

	STreeData TreeData;
	TreeData.pNode = m_pRootNode;
	TreeData.mm64MinCur = CoordMin;
	TreeData.mm64MaxCur = CoordMax;
	TreeData.btMinCell = GetLow3BitsFromInt64(TreeData.mm64MinCur.m64_u64);
	TreeData.btMaxCell = GetLow3BitsFromInt64(TreeData.mm64MaxCur.m64_u64);
	TreeData.btDepth = 0;
	TreeData.vMin = Vector3D<DWORD>(0);
	TreeData.btCrossPlaneFlag = 63; //первые 6 бит

	BYTE btMinDepth = GetDepthForMinNodeSize(in_dwMinNodeSize);

	CollectCell<Type>(TreeData, btMinDepth, dwBoundBox, in_pFrustum, out_aNodes);

	//_mm_empty();

	return true;
}

IGridIterator& COkTree::AcquireIterator()
{
	CSingleGridIterator* pG = static_cast<CSingleGridIterator*>( m_pGridIterator.get() );
	pG->Init();
	return *m_pGridIterator;
}

IGridIterator& COkTree::GetIterator(const CBoundBox<float>& in_BoundBox, const CFrustum<float>* in_pFrustum)
{
	CSingleGridIterator& Result = static_cast<CSingleGridIterator&>( AcquireIterator() );

	DWORD dwMinNodeSize = DWORD(-1) >> g_btAdressCount; //минимальный размер вообще

	bool bRes = CollectCell<CNodeVector>(in_BoundBox, in_pFrustum, dwMinNodeSize, Result.m_aNodes);

	if( bRes && !Result.m_aNodes.empty() )
		Result.InitNodeIteratorData();

	return Result;
}

//////////////////////////////////////////////////////////////////////////
// Intersect
//////////////////////////////////////////////////////////////////////////

UINT_PTR COkTree::IsIntersectSegmentInNode(const SIntersectInputForObject& in_IntersectInput, const COkNode* const in_pNode, SIntersectOut& out_ReturnInfo) const
{
	UINT_PTR pResObject = NULL;
	SObjectListElem* pCO = in_pNode->m_pOwnObject;
	while( pCO )
	{
		const SObjectInfo* pOI = GetObjectInfoByIndex(pCO->m_dwObjectIndex);

		if( IsObjectIntersectSegment(pOI->m_dwObject, in_IntersectInput, out_ReturnInfo) )
			pResObject = pOI->m_dwObject;

		pCO = pCO->m_pNext;
	}

	return pResObject;
}

void CalcNodeParam( BYTE& io_btNext, BYTE in_btIndex, BYTE in_btCompareIndex, 
				   Vector3D<double>& io_vT0, const Vector3D<double>& in_vCurrHalfSize, 
				   const Vector3D<bool>& in_vInfinite, const Vector3D<double>& in_vStartSegment, 
				   Vector3D<double>& io_vCorner, const Vector3D<double>& in_vCurrRealHalfSize )
{
	assert_debug( in_btIndex < 3, L"" );
	double dTM;
	if( in_vInfinite.v[in_btIndex] )
	{
		dTM = io_vCorner.v[in_btIndex] + in_vCurrRealHalfSize.v[in_btIndex];
		if( in_vStartSegment.v[in_btIndex] > dTM )
		{
			io_btNext |= ( 1 << in_btIndex );
			io_vCorner.v[in_btIndex] = dTM;
		}
	}
	else
	{
		dTM = io_vT0.v[in_btIndex] + in_vCurrHalfSize.v[in_btIndex];
		RANGE_ASSERT(dTM);
		if( dTM < io_vT0.v[in_btCompareIndex] )
		{
			io_btNext |= ( 1 << in_btIndex );
			io_vT0.v[in_btIndex] = dTM;
			io_vCorner.v[in_btIndex] += in_vCurrRealHalfSize.v[in_btIndex];
		}
	}
}

BYTE GetFirstIntersectNode(EIntersectPlane in_IP, Vector3D<double>& io_vT0, const Vector3D<double>& in_vCurrHalfSize, 
						   const Vector3D<bool>& in_vInfinite, const Vector3D<double>& in_vStartSegment, Vector3D<double>& io_vCorner, 
						   const Vector3D<double>& in_vCurrRealHalfSize)
{
	BYTE btNext = 0;
	switch(in_IP) 
	{
	case ipXY:
		CalcNodeParam(btNext, 0, 2, io_vT0, in_vCurrHalfSize, in_vInfinite, in_vStartSegment, io_vCorner, in_vCurrRealHalfSize);
		CalcNodeParam(btNext, 1, 2, io_vT0, in_vCurrHalfSize, in_vInfinite, in_vStartSegment, io_vCorner, in_vCurrRealHalfSize);
		break;
	case ipXZ:
		CalcNodeParam(btNext, 0, 1, io_vT0, in_vCurrHalfSize, in_vInfinite, in_vStartSegment, io_vCorner, in_vCurrRealHalfSize);
		CalcNodeParam(btNext, 2, 1, io_vT0, in_vCurrHalfSize, in_vInfinite, in_vStartSegment, io_vCorner, in_vCurrRealHalfSize);
		break;
	case ipYZ:
		CalcNodeParam(btNext, 1, 0, io_vT0, in_vCurrHalfSize, in_vInfinite, in_vStartSegment, io_vCorner, in_vCurrRealHalfSize);
		CalcNodeParam(btNext, 2, 0, io_vT0, in_vCurrHalfSize, in_vInfinite, in_vStartSegment, io_vCorner, in_vCurrRealHalfSize);
	}

	RANGE_ASSERT(io_vT0.x);
	RANGE_ASSERT(io_vT0.y);
	RANGE_ASSERT(io_vT0.z);
	return btNext;
}

EIntersectPlane GetExitPlane(const Vector3D<double>& in_vT0, const Vector3D<double>& in_vCurrHalfSize, const Vector3D<bool>& in_vInfinite)
{
	//if( in_vT1.x < in_vT1.y ) т.к. in_vT1.x = in_vT0.x + 2 * in_vCurrHalfSize.x то можно попробовать:
	//нельзя, т.к. in_vCurrHalfSize.x != in_vCurrHalfSize.y != in_vCurrHalfSize.z

	RANGE_ASSERT(in_vT0.x);
	RANGE_ASSERT(in_vT0.y);
	RANGE_ASSERT(in_vT0.z);

	Vector3D<double> out_vT1 = 2. * in_vCurrHalfSize; //in_vT0;

	if( !in_vInfinite.x )
		out_vT1.x += in_vT0.x;
	if( !in_vInfinite.y )
		out_vT1.y += in_vT0.y;
	if( !in_vInfinite.z )
		out_vT1.z += in_vT0.z;

	RANGE_ASSERT(out_vT1.x);
	RANGE_ASSERT(out_vT1.y);
	RANGE_ASSERT(out_vT1.z);

	if( out_vT1.x < out_vT1.y )
	{
		if( out_vT1.x < out_vT1.z )
			return ipYZ;
		else
			return ipXY;
	}
	else
	{
		if( out_vT1.y < out_vT1.z )
			return ipXZ;
		else
			return ipXY;
	}
}

BYTE GetNextIntersectNode(EIntersectPlane in_IP, BYTE in_btChild, Vector3D<double>& io_vT0, const Vector3D<double>& in_vCurrHalfSize,
						  const Vector3D<bool>& in_vInfinite, Vector3D<double>& io_vCorner, const Vector3D<double>& in_vCurrRealHalfSize)
{
	BYTE btNext;
	switch(in_btChild) 
	{
	case 0:
		switch(in_IP)
		{
		case ipXY:
			btNext = 4;
			if( !in_vInfinite.z )
				io_vT0.z += 2 * in_vCurrHalfSize.z;
			io_vCorner.z += 2 * in_vCurrRealHalfSize.z;
			break;
		case ipXZ:
			btNext = 2;
			if( !in_vInfinite.y )
				io_vT0.y += 2 * in_vCurrHalfSize.y;
			io_vCorner.y += 2 * in_vCurrRealHalfSize.y;
			break;
		case ipYZ:
			btNext = 1;
			if( !in_vInfinite.x )
				io_vT0.x += 2 * in_vCurrHalfSize.x;
			io_vCorner.x += 2 * in_vCurrRealHalfSize.x;
			break;
		}
		break;
	case 1:
		switch(in_IP)
		{
		case ipXY:
			btNext = 5;
			if( !in_vInfinite.z )
				io_vT0.z += 2 * in_vCurrHalfSize.z;
			io_vCorner.z += 2 * in_vCurrRealHalfSize.z;
			break;
		case ipXZ:
			btNext = 3;
			if( !in_vInfinite.y )
				io_vT0.y += 2 * in_vCurrHalfSize.y;
			io_vCorner.y += 2 * in_vCurrRealHalfSize.y;
			break;
		case ipYZ:
			btNext = 8;
			break;
		}
		break;
	case 2:
		switch(in_IP)
		{
		case ipXY:
			btNext = 6;
			if( !in_vInfinite.z )
				io_vT0.z += 2 * in_vCurrHalfSize.z;
			io_vCorner.z += 2 * in_vCurrRealHalfSize.z;
			break;
		case ipXZ:
			btNext = 8;
			break;
		case ipYZ:
			btNext = 3;
			if( !in_vInfinite.x )
				io_vT0.x += 2 * in_vCurrHalfSize.x;
			io_vCorner.x += 2 * in_vCurrRealHalfSize.x;
			break;
		}
		break;
	case 3:
		switch(in_IP)
		{
		case ipXY:
			btNext = 7;
			if( !in_vInfinite.z )
				io_vT0.z += 2 * in_vCurrHalfSize.z;
			io_vCorner.z += 2 * in_vCurrRealHalfSize.z;
			break;
		default:
			btNext = 8;
			break;
		}
		break;
	case 4:
		switch(in_IP)
		{
		case ipXY:
			btNext = 8;
			break;
		case ipXZ:
			btNext = 6;
			if( !in_vInfinite.y )
				io_vT0.y += 2 * in_vCurrHalfSize.y;
			io_vCorner.y += 2 * in_vCurrRealHalfSize.y;
			break;
		case ipYZ:
			btNext = 5;
			if( !in_vInfinite.x )
				io_vT0.x += 2 * in_vCurrHalfSize.x;
			io_vCorner.x += 2 * in_vCurrRealHalfSize.x;
			break;
		}
		break;
	case 5:
		switch(in_IP)
		{
		case ipXZ:
			btNext = 7;
			if( !in_vInfinite.y )
				io_vT0.y += 2 * in_vCurrHalfSize.y;
			io_vCorner.y += 2 * in_vCurrRealHalfSize.y;
			break;
		default:
			btNext = 8;
			break;
		}
		break;
	case 6:
		switch(in_IP)
		{
		case ipYZ:
			btNext = 7;
			if( !in_vInfinite.x )
				io_vT0.x += 2 * in_vCurrHalfSize.x;
			io_vCorner.x += 2 * in_vCurrRealHalfSize.x;
			break;
		default:
			btNext = 8;
			break;
		}
		break;
	default:
		btNext = 8;
	}

	RANGE_ASSERT(io_vT0.x);
	RANGE_ASSERT(io_vT0.y);
	RANGE_ASSERT(io_vT0.z);
	return btNext;
}

UINT_PTR COkTree::CheckIntersectInNode(const COkNode* const in_pNode, const SIntersectInputForObject& in_IntersectInput,
									   Vector3D<double>& io_vPartSegmentBeforeIntersectObject,
									   SIntersectOut& out_ReturnInfo) const
{
	UINT_PTR pObject = IsIntersectSegmentInNode(in_IntersectInput, in_pNode, out_ReturnInfo);

	if(pObject)
	{
		Vector3D<float> vLink = in_IntersectInput.m_vEnd - in_IntersectInput.m_vStart;
		io_vPartSegmentBeforeIntersectObject.x = fabs(vLink.x * out_ReturnInfo.m_dT);
		io_vPartSegmentBeforeIntersectObject.y = fabs(vLink.y * out_ReturnInfo.m_dT);
		io_vPartSegmentBeforeIntersectObject.z = fabs(vLink.z * out_ReturnInfo.m_dT);
	}

	return pObject;
}

UINT_PTR COkTree::IsIntersectSegment(const SIntersectInput& in_IntersectInput, SIntersectOut& io_ReturnInfo) const
{
	DisableFloatExceptions();

	//m_aDebugIntersectInfo.clear();

	UINT_PTR pResObject = NULL;

	SIntersectInputForObject IntersectInput(in_IntersectInput);

	Vector3D<double> vPartSegmentBeforeIntersectObject = Vector3D<double>(0);

	if( !m_pRootNode->IsChildrenPresent() )
	{
		EnableFloatExceptions();
		return 0;
	}

	__m64 mm64Origin;
	if(! GetCoordInCell(IntersectInput.m_vStart, mm64Origin))
	{
		EnableFloatExceptions();
		assert_debug(false, L""); 
		return 0;
	}	

	Vector3D<double> vMin = GetMinCorner();
	Vector3D<double> vMax = GetMaxCorner();
	Vector3D<double> vD = Vector3D<double>(IntersectInput.m_vEnd - IntersectInput.m_vStart);

	IntersectInput.m_dblLength = vD.Length();
	IntersectInput.m_dblInvLength = 1 / IntersectInput.m_dblLength;


	Vector3D<double> vFinishT(io_ReturnInfo.m_dT,io_ReturnInfo.m_dT,io_ReturnInfo.m_dT);

	Vector3D<double> vInvD = Vector3D<double>( 1 / vD.x, 1 / vD.y, 1 / vD.z );
	IntersectInput.m_vInvD = vInvD;
	Vector3D<double> vStart = Vector3D<double>(IntersectInput.m_vStart) - vMin;

	BYTE btInvIndex = 0;
	if( vD.x < 0  )
	{
		vStart.x = GetSize().x - vStart.x;
		vD.x = -vD.x;
		vInvD.x = -vInvD.x;
		btInvIndex |= 1;
	}
	if( vD.y < 0  )
	{
		vStart.y = GetSize().y - vStart.y;
		vD.y = -vD.y;
		vInvD.y = -vInvD.y;
		btInvIndex |= 2;
	}
	if( vD.z < 0  )
	{
		vStart.z = GetSize().z - vStart.z;
		vD.z = -vD.z;
		vInvD.z = -vInvD.z;
		btInvIndex |= 4;
	}

	BYTE btCurrIndex;	
	Vector3D<double> vCurrT0;
	Vector3D<double> vCurrHalfSize;
	Vector3D<double> vCurrCorner;
	Vector3D<double> vCurrRealHalfSize;	
	const COkNode* pCurrParent = m_pRootNode;

	vCurrCorner = Vector3D<double>(0);
	Vector3D<double> vT = vCurrCorner - vStart;
	vCurrT0 = Vector3D<double>( vT.x * vInvD.x, vT.y * vInvD.y, vT.z * vInvD.z );
	vT = GetSize() - vStart;
	Vector3D<double> vT1 = Vector3D<double>( vT.x * vInvD.x, vT.y * vInvD.y, vT.z * vInvD.z );

	RANGE_ASSERT(vCurrT0.x);
	RANGE_ASSERT(vCurrT0.y);
	RANGE_ASSERT(vCurrT0.z);
	RANGE_ASSERT(vT1.x);
	RANGE_ASSERT(vT1.y);
	RANGE_ASSERT(vT1.z);

	int iInfinityRes = _fpclass(vCurrT0.x);
	IntersectInput.m_vInfinite.x = iInfinityRes == _FPCLASS_NINF || iInfinityRes == _FPCLASS_PINF;
	iInfinityRes = _fpclass(vCurrT0.y);
	IntersectInput.m_vInfinite.y = iInfinityRes == _FPCLASS_NINF || iInfinityRes == _FPCLASS_PINF;
	iInfinityRes = _fpclass(vCurrT0.z);
	IntersectInput.m_vInfinite.z = iInfinityRes == _FPCLASS_NINF || iInfinityRes == _FPCLASS_PINF;

	vCurrHalfSize = (vT1 - vCurrT0) * 0.5;
	vCurrRealHalfSize = GetSize() * 0.5;

	//intersect segment in this node
	pResObject = CheckIntersectInNode(m_pRootNode, IntersectInput, vPartSegmentBeforeIntersectObject, io_ReturnInfo);

	//на такой строчке неправильно компиляется Release
	//BYTE btRealIndex = (BYTE)( _mm_cvtsi64_si32(mm64Origin) & 0x7 );
	//получается
	//movd	mm3, dword ptr [esp+0D0h]
	//and	bl, 7
	//Нужно расписать подробнее, чтобы получилось
	//movq        mm0,mmword ptr [mm64Origin] 
	//movd        eax,mm0 
	//and         al,7 


	BYTE btRealIndex = GetLow3BitsFromInt64(mm64Origin.m64_u64);

	//_mm_empty();
	btCurrIndex = btRealIndex ^ btInvIndex;

	COkNode* pChildNode = pCurrParent->m_ppChildren[btRealIndex];


	if( !IntersectInput.m_vInfinite.x )
		vCurrT0.x = vCurrT0.x + vCurrHalfSize.x * (btCurrIndex & 1);
	vCurrCorner.x = vCurrCorner.x + vCurrRealHalfSize.x * (btCurrIndex & 1);


	if( !IntersectInput.m_vInfinite.y )
		vCurrT0.y = vCurrT0.y + vCurrHalfSize.y * ((btCurrIndex & 2) >> 1);
	vCurrCorner.y = vCurrCorner.y + vCurrRealHalfSize.y * ((btCurrIndex & 2) >> 1);

	if( !IntersectInput.m_vInfinite.z )
		vCurrT0.z = vCurrT0.z + vCurrHalfSize.z * ((btCurrIndex & 4) >> 2);
	vCurrCorner.z = vCurrCorner.z + vCurrRealHalfSize.z * ((btCurrIndex & 4) >> 2);

	assert_debug( (IntersectInput.m_vInfinite.x || vCurrT0.x < 0.001) && 
		(IntersectInput.m_vInfinite.y || vCurrT0.y < 0.001) && 
		(IntersectInput.m_vInfinite.z || vCurrT0.z < 0.001) , L"");

	vCurrHalfSize *= 0.5;
	vCurrRealHalfSize *= 0.5;

	RANGE_ASSERT(vCurrT0.x);
	RANGE_ASSERT(vCurrT0.y);
	RANGE_ASSERT(vCurrT0.z);

	unsigned int nStackMaxSize = 2 * m_btCommonDepth;
	SIntersectStackData* const m_aIntersectStack = static_cast<SIntersectStackData*>(_alloca(nStackMaxSize * sizeof(SIntersectStackData)));

	EIntersectPlane CurrentExitPlane;
	unsigned nStackDepth = 0;
	BYTE btDepth = 0;
	while( pChildNode )
	{
		//intersect segment in this node
		UINT_PTR pNewIntersectionTest = CheckIntersectInNode(pChildNode, IntersectInput, vPartSegmentBeforeIntersectObject, io_ReturnInfo);

		if( pNewIntersectionTest )
			pResObject = pNewIntersectionTest;

		if( !pChildNode->IsChildrenPresent() )
		{
			CurrentExitPlane = GetExitPlane(vCurrT0, vCurrHalfSize, IntersectInput.m_vInfinite);
			goto lblIsIntersectSegment;
		}

		//push
		SIntersectStackData* pNewISD = &m_aIntersectStack[nStackDepth++];
		pNewISD->btCurrIndex = btCurrIndex;
		pNewISD->pCurrParent = pCurrParent;
		pNewISD->vCurrT0 = vCurrT0;
		pNewISD->vCurrHalfSize = vCurrHalfSize;
		pNewISD->m_btDepth = btDepth;
		pNewISD->vCurrCorner = vCurrCorner;
		pNewISD->vCurrRealHalfSize = vCurrRealHalfSize;		

		btRealIndex = ShearRight3BitAndGetLow3BitsFromM64(mm64Origin);
		btCurrIndex = btRealIndex ^ btInvIndex;
		pCurrParent = pChildNode;
		pChildNode = pCurrParent->m_ppChildren[btRealIndex];

		if( !IntersectInput.m_vInfinite.x )
			vCurrT0.x = vCurrT0.x + vCurrHalfSize.x * (btCurrIndex & 1);
		if( !IntersectInput.m_vInfinite.y )
			vCurrT0.y = vCurrT0.y + vCurrHalfSize.y * ((btCurrIndex & 2) >> 1);
		if( !IntersectInput.m_vInfinite.z )
			vCurrT0.z = vCurrT0.z + vCurrHalfSize.z * ((btCurrIndex & 4) >> 2);

		vCurrCorner.x = vCurrCorner.x + vCurrRealHalfSize.x * (btCurrIndex & 1);
		vCurrCorner.y = vCurrCorner.y + vCurrRealHalfSize.y * ((btCurrIndex & 2) >> 1);
		vCurrCorner.z = vCurrCorner.z + vCurrRealHalfSize.z * ((btCurrIndex & 4) >> 2);

		assert_debug(	(IntersectInput.m_vInfinite.x || vCurrT0.x < 0.001) && 
			(IntersectInput.m_vInfinite.y || vCurrT0.y < 0.001) && 
			(IntersectInput.m_vInfinite.z || vCurrT0.z < 0.001), L"" );

		RANGE_ASSERT(vCurrT0.x);
		RANGE_ASSERT(vCurrT0.y);
		RANGE_ASSERT(vCurrT0.z);

		vCurrHalfSize *= 0.5;
		vCurrRealHalfSize *= 0.5;
		btDepth++;
	}

	CurrentExitPlane = GetExitPlane(vCurrT0, vCurrHalfSize, IntersectInput.m_vInfinite);

	//SDebugIntersectInfo* pDebugIntersectInfo = m_aDebugIntersectInfo.AddEntries();
	//pDebugIntersectInfo->m_dwIntersectStackDepth = btDepth;
	//pDebugIntersectInfo->m_btRealIndex = btRealIndex;
	//pDebugIntersectInfo->m_IntersectPlane = CurrentExitPlane;

	goto lblIsIntersectSegment;

	while( nStackDepth )
	{		
		const SIntersectStackData* pISD = &m_aIntersectStack[--nStackDepth];
		btCurrIndex			= pISD->btCurrIndex;
		pCurrParent			= pISD->pCurrParent;
		vCurrT0					= pISD->vCurrT0;
		vCurrHalfSize			= pISD->vCurrHalfSize;
		btDepth					= pISD->m_btDepth;
		vCurrCorner			= pISD->vCurrCorner;
		vCurrRealHalfSize = pISD->vCurrRealHalfSize;
		assert_debug( pCurrParent , L"");			

lblIsIntersectSegment:

		btCurrIndex = GetNextIntersectNode(CurrentExitPlane, btCurrIndex, vCurrT0, vCurrHalfSize, IntersectInput.m_vInfinite, vCurrCorner, vCurrRealHalfSize);

		while( btCurrIndex < 8 )
		{
			if( (vCurrT0.x > vFinishT.x && !IntersectInput.m_vInfinite.x) || 
				(vCurrT0.y > vFinishT.y && !IntersectInput.m_vInfinite.y) || 
				(vCurrT0.z > vFinishT.z && !IntersectInput.m_vInfinite.z) )
			{
				//all cells far
				nStackDepth = 0;
				btCurrIndex = 8;
				continue;				
			}

			if( pResObject && (
				(vCurrT0.x > vPartSegmentBeforeIntersectObject.x && !IntersectInput.m_vInfinite.x) ||
				(vCurrT0.y > vPartSegmentBeforeIntersectObject.y && !IntersectInput.m_vInfinite.y) || 
				(vCurrT0.z > vPartSegmentBeforeIntersectObject.z && !IntersectInput.m_vInfinite.z) ) )
			{
				//all cells far from us then intersect object
				nStackDepth = 0;
				btCurrIndex = 8;
				continue;				
			}

			btRealIndex = btCurrIndex ^ btInvIndex;
			const COkNode* pCurrNode = pCurrParent->m_ppChildren[btRealIndex];

			//pDebugIntersectInfo = m_aDebugIntersectInfo.AddEntries();
			//pDebugIntersectInfo->m_dwIntersectStackDepth = btDepth;
			//pDebugIntersectInfo->m_btRealIndex = btRealIndex;
			//pDebugIntersectInfo->m_IntersectPlane = CurrentExitPlane;

			if( pCurrNode )
			{

				//intersect segment in this node
				UINT_PTR pNewIntersectionTest = CheckIntersectInNode(pCurrNode, IntersectInput, vPartSegmentBeforeIntersectObject, io_ReturnInfo);
				if( pNewIntersectionTest )
					pResObject = pNewIntersectionTest;

				if( pCurrNode->IsChildrenPresent() )
				{
					//push
					assert_debug(nStackDepth < nStackMaxSize, L"");
					SIntersectStackData* pNewISD = &m_aIntersectStack[nStackDepth++];
					pNewISD->btCurrIndex = btCurrIndex;
					pNewISD->pCurrParent = pCurrParent;
					pNewISD->vCurrT0 = vCurrT0;
					pNewISD->vCurrHalfSize = vCurrHalfSize;
					pNewISD->m_btDepth = btDepth;
					pNewISD->vCurrCorner = vCurrCorner;
					pNewISD->vCurrRealHalfSize = vCurrRealHalfSize;					

					//find first child
					btCurrIndex = GetFirstIntersectNode(CurrentExitPlane, vCurrT0, vCurrHalfSize, IntersectInput.m_vInfinite, vStart, vCurrCorner, vCurrRealHalfSize);
					pCurrParent = pCurrNode;
					vCurrHalfSize *= 0.5;
					vCurrRealHalfSize *= 0.5;
					btDepth++;
					assert_debug( btCurrIndex < 7, L"" );
					continue;
				}
			}

			CurrentExitPlane = GetExitPlane(vCurrT0, vCurrHalfSize, IntersectInput.m_vInfinite);
			btCurrIndex = GetNextIntersectNode(CurrentExitPlane, btCurrIndex, vCurrT0, vCurrHalfSize, IntersectInput.m_vInfinite, vCurrCorner, vCurrRealHalfSize);
		}
	}

	EnableFloatExceptions();		
	return pResObject;
}


void COkTree::Clear()
{
#ifdef GRID_ASSERTS
	m_mapControl.clear();
#endif

	for(COkNode* pNode = m_stNodes.GetFirstElement(); pNode; pNode = m_stNodes.GetNextElement())
	{
		pNode->m_pCommonLevelObjects = 0;
		pNode->m_pOwnObject = 0;
	}

	m_stObjectInfo.RemoveAll();
	m_stObjectListElems.RemoveAll();
}

//#pragma optimize("", on)



#pragma warning(default:4731)

