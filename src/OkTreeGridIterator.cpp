#include "StdAfx.h"
#include "OkTreeGridIterator.h"
#include "OkTree.h"

//#ifdef _DEBUG
//TRACE_DWORD(m_CommonLevelsStatistic.m_dwCellsCount);
//TRACE_DWORD(m_CommonLevelsStatistic.m_dwCellsObjectsCount);
//TRACE_DWORD(m_CommonLevelsStatistic.m_dwCommonLevelsCount);
//TRACE_DWORD(m_CommonLevelsStatistic.m_dwCommonLevelsObjectsCount);
//#endif // _DEBUG

SIteratorInfo::SIteratorInfo(COkTree* in_pTree): 
	m_aNodes(1000), m_iTraceIndex(0), m_dwIteratorCycle(0), m_pTree(in_pTree) //, m_setTheyWere(10)
{
	assert_debug(m_pTree, L"");
}

void SIteratorInfo::Init()
{
#ifdef _DEBUG
	m_setTheyWere.clear();
#endif //_DEBUG

	m_aNodes.clear();
	m_NodeObjectChain.Init();
	m_dwIteratorCycle++;
}

void SIteratorInfo::GetNextObjectListElem()
{
	assert_debug( m_NodeObjectChain.m_pObjectListElem, L"" );

	if( m_NodeObjectChain.Next() )
		return;

	m_aNodes.pop_back();

	if( m_aNodes.empty() )
	{
		Init();
		return;
	}

	InitNodeIteratorData();
}

void SIteratorInfo::InitNodeIteratorData()
{
	assert_debug( !m_aNodes.empty(), L"" );

	SNodeForIterator* pItData = &m_aNodes.back();
	assert_debug(pItData, L"");
	assert_debug(pItData->m_pNode, L"");

	if( pItData->m_btDepth == GetCommonDepth() && pItData->m_btCrossFrustumPlaneFlag == 0)
	{
		assert_debug(pItData->m_pNode->m_pCommonLevelObjects, L"");
		m_NodeObjectChain.m_pObjectListElem = pItData->m_pNode->m_pCommonLevelObjects;
		m_NodeObjectChain.m_btCrossFrustumPlaneFlag = 0;
	}
	else
	{
		assert_debug(pItData->m_pNode->m_pOwnObject, L"");
		m_NodeObjectChain.m_pObjectListElem = pItData->m_pNode->m_pOwnObject;
		m_NodeObjectChain.m_btCrossFrustumPlaneFlag = pItData->m_btCrossFrustumPlaneFlag;
	}

	assert_debug( m_NodeObjectChain.m_pObjectListElem, L"" );

#ifdef _DEBUG
	m_CommonLevelsStatistic.m_dwCommonLevelsCount++;
	m_CommonLevelsStatistic.m_dwCommonLevelsObjectsCount += GetObjectListElemLength( m_NodeObjectChain.m_pObjectListElem );
#endif //_DEBUG

}

bool SIteratorInfo::IsObjectVisited() const
{	
	assert_debug(m_NodeObjectChain.m_pObjectListElem && m_NodeObjectChain.m_pObjectListElem->m_dwIndexInStore != DWORD(-1), L"" );

	const SObjectInfo* pInfo = m_pTree->GetObjectInfoByIndex(m_NodeObjectChain.m_pObjectListElem->m_dwObjectIndex);
	bool bResult = pInfo->m_vecIteratorTrace[m_iTraceIndex] == m_dwIteratorCycle;

	return bResult;
}

void SIteratorInfo::GotoNextObject()
{
	assert_debug(m_NodeObjectChain.m_pObjectListElem, L"");

	SObjectInfo* pInfo = m_pTree->GetObjectInfoByIndex(m_NodeObjectChain.m_pObjectListElem->m_dwObjectIndex);
	pInfo->m_vecIteratorTrace[m_iTraceIndex] = m_dwIteratorCycle;

	do 
		GetNextObjectListElem();
	while( m_NodeObjectChain.m_pObjectListElem && IsObjectVisited() );
}

unsigned SIteratorInfo::GetCommonDepth() const
{
	return m_pTree->GetCommonDepth();
}

CSingleGridIterator::CSingleGridIterator(COkTree* in_pTree): SIteratorInfo(in_pTree)
{
}

UINT_PTR CSingleGridIterator::Get() const
{
	assert_debug(m_NodeObjectChain.m_pObjectListElem, L"");

	if(!m_NodeObjectChain.m_pObjectListElem )
		return 0;

	assert_debug( !IsObjectVisited(), L"" );
	const SObjectInfo* pOI = m_pTree->GetObjectInfoByIndex(m_NodeObjectChain.m_pObjectListElem->m_dwObjectIndex);
	return pOI->m_dwObject;
}

void CSingleGridIterator::Next()
{
	GotoNextObject();
}

bool CSingleGridIterator::IsEnd() const
{
	return m_NodeObjectChain.m_pObjectListElem == NULL;
}

BYTE CSingleGridIterator::GetFrustumFlag() const
{
	return m_NodeObjectChain.m_btCrossFrustumPlaneFlag;
}

CSingleGridIterator& CSingleGridIterator::operator++(int)
{
	Next();
	return *this;
}