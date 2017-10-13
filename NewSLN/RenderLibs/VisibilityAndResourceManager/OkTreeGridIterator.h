#pragma once
#include "GridIterator.h"
#include "OkTree.h"

struct SCommonLevelsStatistic
{
	DWORD	m_dwCommonLevelsCount;
	DWORD	m_dwCellsCount;
	DWORD	m_dwCommonLevelsObjectsCount;
	DWORD	m_dwCellsObjectsCount;

	SCommonLevelsStatistic()
	{
		Init();
	}

	void Init()
	{
		m_dwCommonLevelsCount = 0;
		m_dwCellsCount = 0;
		m_dwCommonLevelsObjectsCount = 0;
		m_dwCellsObjectsCount = 0;
	}
};

struct SNodeForIterator
{
	COkNode*	m_pNode;
	BYTE		m_btCrossFrustumPlaneFlag;
	BYTE		m_btDepth;

	SNodeForIterator()
	{
		m_pNode = NULL;
		m_btDepth = 0;
		m_btCrossFrustumPlaneFlag = 63; //первые 6 бит
	}

	SNodeForIterator(COkNode* in_pNode, BYTE in_btCrossFrustumPlaneFlag, BYTE in_btDepth)
	{
		assert_debug( in_btCrossFrustumPlaneFlag < 64, L"Illegal cross plane frustum flag" );
		m_pNode = in_pNode;
		m_btCrossFrustumPlaneFlag = in_btCrossFrustumPlaneFlag;
		m_btDepth = in_btDepth;
	}
};
typedef vector<SNodeForIterator> CNodeVector;

__forceinline BOOL EqualNodeForIterator(void* in_pKey1, void* in_pKey2)
{
	return ((SNodeForIterator*)in_pKey1)->m_pNode == ((SNodeForIterator*)in_pKey2)->m_pNode;
}

__forceinline BOOL LessNodeForIterator(void* in_pKey1, void* in_pKey2)
{
	return ((SNodeForIterator*)in_pKey1)->m_pNode < ((SNodeForIterator*)in_pKey2)->m_pNode;
}

struct SNodeObjectChain
{
	SObjectListElem*	m_pObjectListElem;
	BYTE				m_btCrossFrustumPlaneFlag;

	SNodeObjectChain()
	{
		Init();
	}

	SNodeObjectChain(SObjectListElem* in_pObjectListElem, BYTE in_btCrossFrustumPlaneFlag)
	{
		assert_debug( in_btCrossFrustumPlaneFlag < 64, L"" );
		m_pObjectListElem = in_pObjectListElem;
		m_btCrossFrustumPlaneFlag = in_btCrossFrustumPlaneFlag;
	}

	void Init()
	{
		m_pObjectListElem = NULL;
		m_btCrossFrustumPlaneFlag = 63; //первые 6 бит
	}

	bool	Next()
	{
		m_pObjectListElem = m_pObjectListElem->m_pNext;

		return (m_pObjectListElem != 0);
	}
};

struct SIteratorInfo
{
	SIteratorInfo(COkTree* in_pTree);
	void							Init();
	void							GetNextObjectListElem();
	bool							IsObjectVisited() const;
	void							GotoNextObject();

protected:

	void							InitNodeIteratorData();

	unsigned						GetCommonDepth() const;

#ifdef _DEBUG
	set<UINT_PTR>					m_setTheyWere;
	SCommonLevelsStatistic			m_CommonLevelsStatistic;
#endif //_DEBUG	

	CNodeVector						m_aNodes;
	SNodeObjectChain				m_NodeObjectChain;
	DWORD							m_dwIteratorCycle;
	unsigned						m_iTraceIndex;

	COkTree*						m_pTree;

	friend class COkTree;
};

class CSingleGridIterator: public IGridIterator, public SIteratorInfo
{
public:
	CSingleGridIterator(COkTree* in_pTree);
	virtual ~CSingleGridIterator() {}

	virtual UINT_PTR				Get() const;
	virtual bool					IsEnd() const;
	virtual void					Next();
	virtual BYTE					GetFrustumFlag() const;
	virtual CSingleGridIterator&	operator++(int);

	friend class COkTree;
};
