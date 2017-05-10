#pragma once

#include "Bitfield.h"

template <class Type> 
class CStore
{

	struct SHeadElementBuffer
	{
		Type*			m_pBuffer;
		DWORD*			m_bfRemoved;

		SHeadElementBuffer()
		{
			m_pBuffer = NULL;
			m_bfRemoved = NULL;
		}
	};

protected:

	DWORD				m_dwOffset;
	SHeadElementBuffer*	m_pHeads;
	DWORD				m_dwHeadsCount;

	vector<DWORD>		m_aRemoved;

#ifdef _DEBUG
	std::set<DWORD>		m_setDebugRemoved;
#endif

	DWORD				m_dwFill_size;

	mutable DWORD m_dwCounter;
	static const DWORD	m_dwClosedCounterValue = DWORD(-2);

	__forceinline DWORD GetBufferSize() const { return 1 << m_dwOffset; }

	__forceinline DWORD GetBFSize() const
	{ 
		return (GetBufferSize() >> 5) + 1;
	}

	__forceinline void ClearBF(DWORD* in_bfRemoved)
	{
		assert_debug( in_bfRemoved, L"" );
		ZeroMemory( in_bfRemoved, sizeof(DWORD) * GetBFSize() );
	}

	__forceinline int GetBit(DWORD in_dwBitNum, DWORD* in_bfRemoved) const // Считать бит
	{
		assert_debug(in_dwBitNum < GetBufferSize(), FormatString( L"Выход за границы коллекции (%i >= %i).", in_dwBitNum, GetBufferSize()).c_str() );

		DWORD offset = in_dwBitNum & 31;
		return (in_bfRemoved[in_dwBitNum >> 5] & (1 << offset)) >> offset;
	};

	__forceinline void SetBit(DWORD in_dwBitNum, bool in_bValue, DWORD* in_bfRemoved)	// Установить значение бита в value
	{
		assert_debug(in_dwBitNum < GetBufferSize(), FormatString( L"Выход за границы коллекции (%i >= %i).", in_dwBitNum, GetBufferSize()).c_str() );

		DWORD offset = in_dwBitNum & 31;
		if(in_bValue)
			in_bfRemoved[in_dwBitNum>>5] |= 1 << offset;
		else
			in_bfRemoved[in_dwBitNum>>5] &= ~(1 << offset);
	};

	__forceinline Vector2D<DWORD> GetIndexs(DWORD in_dwIndex) const
	{
		DWORD dwIndexBuffer = in_dwIndex >> m_dwOffset;
		assert_debug(dwIndexBuffer < m_dwHeadsCount, L"");
		DWORD dwIndexInBuffer = ( (1 << m_dwOffset) - 1 ) & in_dwIndex;
		return Vector2D<DWORD>(dwIndexBuffer, dwIndexInBuffer);
	}

	__forceinline Vector2D<DWORD> GetEmptyPlace()
	{
		DWORD dwSz = static_cast<DWORD>( m_aRemoved.size() );
		if(dwSz)
		{
			Vector2D<DWORD> vN = GetIndexs( m_aRemoved[dwSz - 1] );

			assert_debug( GetBit( vN.y, m_pHeads[vN.x].m_bfRemoved ) == 1, L"" );
			SetBit( vN.y, false, m_pHeads[vN.x].m_bfRemoved );

#ifdef _DEBUG
			std::set<DWORD>::iterator itDel = m_setDebugRemoved.find(m_aRemoved.back());
			assert_debug(itDel != m_setDebugRemoved.end(), L"");
			if(itDel != m_setDebugRemoved.end())
				m_setDebugRemoved.erase(itDel);
#endif
			m_aRemoved.pop_back();
			return vN;
		}

		return Vector2D<DWORD>(DWORD(-1), DWORD(-1));
	}

public:
	/*CStore(): m_aRemoved(0, 150)
	{
	m_dwOffset = 0;
	m_pHeads = NULL;
	m_dwHeadsCount = 0;
	m_dwFill_size = 0;
	m_dwCounter = m_dwClosedCounterValue;
	};*/

	CStore(DWORD in_dwPow2): m_aRemoved(0)
	{ 
		m_dwOffset = 0;
		m_pHeads = NULL;
		m_dwHeadsCount = 0;
		m_dwFill_size = 0;
		m_dwCounter = m_dwClosedCounterValue;

		Init(in_dwPow2);
	}

	void Init(DWORD in_dwPow2)
	{
		assert_debug(!m_dwOffset || !m_dwHeadsCount, L"");
		assert_debug(in_dwPow2, L"");
		if( (!m_dwOffset || !m_dwHeadsCount) && in_dwPow2)
		{
			m_dwOffset = in_dwPow2;
			//SetSize();
			m_dwFill_size = 0;
		}
	}

	~CStore() 
	{ 
		Clear();
	};

	void Clear()
	{
		m_dwFill_size = 0;
		for(DWORD dw = 0; dw < m_dwHeadsCount; dw++)
		{
			SAFE_DELETE_ARRAY( m_pHeads[dw].m_pBuffer );
			SAFE_DELETE_ARRAY( m_pHeads[dw].m_bfRemoved );
		}
		SAFE_DELETE_ARRAY(m_pHeads);
		m_dwHeadsCount = 0;
		m_aRemoved.clear();

#ifdef _DEBUG
		m_setDebugRemoved.clear();
#endif
	}

	__forceinline Type &operator [](DWORD id)
	{
		return ElementAt(id);
	}

	__forceinline Type& ElementAt(DWORD id)
	{
		assert_debug(id < m_dwFill_size + m_aRemoved.size(), FormatString(L"Выход за границы коллекции (%i >= %i).", id, m_dwFill_size + m_aRemoved.size() ) );

		Vector2D<DWORD> dwInds = GetIndexs(id);

		//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), id ) == m_aRemoved.end() );
		assert_antoha(m_setDebugRemoved.find(id) == m_setDebugRemoved.end());
		return m_pHeads[dwInds.x].m_pBuffer[dwInds.y];
	}

	__forceinline const Type* GetPointer(DWORD id) const
	{
		assert_debug( id < m_dwFill_size + m_aRemoved.size(), FormatString( L"Выход за границы коллекции (%i >= %i).", id, m_dwFill_size + m_aRemoved.size() ).c_str() );

		Vector2D<DWORD> dwInds = GetIndexs(id);

		//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), id ) == m_aRemoved.end() );
		assert_debug(m_setDebugRemoved.find(id) == m_setDebugRemoved.end(), L"");
		return m_pHeads[dwInds.x].m_pBuffer + dwInds.y;
	}

	__forceinline Type* GetPointer(DWORD id)
	{
		assert_debug( id < m_dwFill_size + m_aRemoved.size(), FormatString( L"Выход за границы коллекции (%i >= %i).", id, m_dwFill_size + m_aRemoved.size() ).c_str() );

		Vector2D<DWORD> dwInds = GetIndexs(id);

		//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), id ) == m_aRemoved.end() );
		assert_debug(m_setDebugRemoved.find(id) == m_setDebugRemoved.end(), L"");
		return m_pHeads[dwInds.x].m_pBuffer + dwInds.y;
	}

	__forceinline DWORD GetIndex(const Type& entry) 
	{
		DWORD dwElementBufferSize = GetBufferSize();
		for(DWORD dwH = 0; dwH < m_dwHeadsCount; dwH++)
		{
			for(DWORD dwB = 0; dwB < dwElementBufferSize; dwB++)
			{
				DWORD dwCurrentIndex = dwH * dwElementBufferSize + dwB;
				if( m_pHeads[dwH].m_pBuffer[dwB] == entry )
				{
					//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), dwCurrentIndex ) == m_aRemoved.end() );
					assert_debug(m_setDebugRemoved.find(dwCurrentIndex) == m_setDebugRemoved.end(), L"");
					return dwCurrentIndex;
				}
			}
		}

		return DWORD(-1);
	}

	__forceinline size_t GetIndex(Type* entry) 
	{
		DWORD dwElementBufferSize = GetBufferSize();
		DWORD dwS = sizeof(Type);

		for(DWORD dwH = 0; dwH < m_dwHeadsCount; dwH++)
		{
			Type* pEnd = m_pHeads[dwH].m_pBuffer + dwElementBufferSize;

			if( entry >= m_pHeads[dwH].m_pBuffer && entry < pEnd )
			{
				size_t dwL = size_t(entry) - size_t(m_pHeads[dwH].m_pBuffer);
				size_t dwCurrentIndex = dwL / dwS + dwH * dwElementBufferSize;

				//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), dwCurrentIndex ) == m_aRemoved.end() );
				assert_debug(m_setDebugRemoved.find(dwCurrentIndex) == m_setDebugRemoved.end(), L"");

				return dwCurrentIndex;
			}
		}

		return size_t(-1);
	}

	//WARNING!!! It is not allowed to use in cycles!
	__forceinline DWORD GetFillSize() const 
	{ 
		return m_dwFill_size; 
	}

	__forceinline DWORD GetFullSize() const 
	{ 
		return m_dwFill_size + m_aRemoved.GetSize(); 
	}

	__forceinline bool IsIndexValid(DWORD in_dwIndex) const
	{
		//Vector2D<DWORD> dwInds = GetIndexs(in_dwIndex);
		//return GetBit( dwInds.iy, m_pHeads[dwInds.ix].m_bfRemoved ) == 0;

		if( in_dwIndex < m_dwFill_size + m_aRemoved.size() && find( m_aRemoved.begin(), m_aRemoved.end(), in_dwIndex ) == m_aRemoved.end() ) 
			return true;

		return false;
	}

	__forceinline Type* GetFirstElement() const
	{
		assert_debug(m_dwCounter == m_dwClosedCounterValue, L"GetFirstElement() без CloseNextElement().");
		m_dwCounter = -1;
		return GetNextElement();
	}

	__forceinline Type* GetNextElement() const
	{
		assert_debug(m_dwCounter != m_dwClosedCounterValue, L"GetNextElement() без GetFirstElement() после CloseNextElement().");
		m_dwCounter++;

		size_t dwFullSize = m_dwFill_size + m_aRemoved.size();
		if( m_dwCounter >= dwFullSize )
		{
			CloseNextElement();
			return NULL;
		}

		Vector2D<DWORD> dwInds = GetIndexs(m_dwCounter);

		while( GetBit( dwInds.y, m_pHeads[dwInds.x].m_bfRemoved ) ) //m_aRemoved.GetIndex(m_dwCounter) != DWORD(-1) )
		{
			m_dwCounter++;
			if( m_dwCounter >= dwFullSize )
			{
				CloseNextElement();
				return NULL;
			}
			dwInds = GetIndexs(m_dwCounter);
		}



		//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), m_dwCounter ) == m_aRemoved.end() );
		assert_debug(m_setDebugRemoved.find(m_dwCounter) == m_setDebugRemoved.end(), L"");

		return m_pHeads[dwInds.x].m_pBuffer + dwInds.y;
	}

	__forceinline void CloseNextElement() const
	{
		assert_debug(m_dwCounter != m_dwClosedCounterValue, L"Повторный вызов CStore::CloseNextElement().");
		m_dwCounter = m_dwClosedCounterValue;
	}

	__forceinline DWORD GetFirstIndex() const
	{
		return GetNextIndex( DWORD(-1) );
	}

	__forceinline DWORD GetNextIndex(DWORD in_dwCurrentIndex) const
	{
		DWORD dwCurrentIndex = in_dwCurrentIndex + 1;

		DWORD dwFullSize = m_dwFill_size + m_aRemoved.size();

		if( dwCurrentIndex >= dwFullSize )
			return DWORD(-1);

		Vector2D<DWORD> dwInds = GetIndexs(dwCurrentIndex);

		while( GetBit( dwInds.y, m_pHeads[dwInds.x].m_bfRemoved ) ) //m_aRemoved.GetIndex(dwCurrentIndex) != DWORD(-1) )
		{
			dwCurrentIndex++;
			if( dwCurrentIndex >= dwFullSize )
				return DWORD(-1);
			dwInds = GetIndexs(dwCurrentIndex);
		}

		return dwCurrentIndex;
	}

	__forceinline DWORD GetOffset() { return m_dwOffset; }

	__forceinline void RemoveAll() 
	{ 
		m_dwFill_size = 0;
		m_aRemoved.clear();

#ifdef _DEBUG
		m_setDebugRemoved.clear();
#endif

		DWORD dwElementBufferSize = GetBufferSize();

		for(DWORD dw = 0; dw < m_dwHeadsCount; dw++)
		{
			ClearBF( m_pHeads[dw].m_bfRemoved );
		}
	}

	__forceinline void SetSize();

	__forceinline bool IsSolid() const
	{
		return m_aRemoved.size() == 0;
	}

	__forceinline DWORD Add()
	{
		DWORD dwElementBufferSize = GetBufferSize();

		m_dwFill_size++;

		Vector2D<DWORD> vInds = GetEmptyPlace();

		if(vInds.x == DWORD(-1))
		{
			//not found empty place
			if( m_dwFill_size > m_dwHeadsCount * dwElementBufferSize )
				SetSize();

			vInds = GetIndexs(m_dwFill_size - 1);
			assert_debug(vInds.x != DWORD(-1), L"");
		}

		DWORD dwCurrentIndex = vInds.x * dwElementBufferSize + vInds.y;
		assert_debug(dwCurrentIndex < m_dwFill_size + m_aRemoved.size(), L"");

		Type* pObj = &m_pHeads[vInds.x].m_pBuffer[vInds.y];
//		new( pObj ) Type;

		return dwCurrentIndex;
	}

	__forceinline DWORD Add(const Type &entry)
	{
		DWORD dwCurrentIndex = Add();

		Vector2D<DWORD> vInds = GetIndexs(dwCurrentIndex);

		m_pHeads[vInds.x].m_pBuffer[vInds.y] = entry;

		return dwCurrentIndex;
	}

	__forceinline Type* AddEntries(DWORD* out_pdwIndex = NULL)	// append and return pointer to a block of new uninitialized entries
	{
		DWORD dwCurrentIndex = Add();

		if( out_pdwIndex )
			*out_pdwIndex = dwCurrentIndex;

		Vector2D<DWORD> vInds = GetIndexs(dwCurrentIndex);

		Type* pEntr = m_pHeads[vInds.x].m_pBuffer + vInds.y;

		return pEntr;
	}

	__forceinline DWORD AddUnique(const Type& in_Entry)		// Appends entry only if the same entry does not exists.
	{
		DWORD dwInd = GetIndex(in_Entry);
		if(dwInd != DWORD(-1))
			return DWORD(-1);

		return Add(in_Entry);
	}

	__forceinline void SetAt(DWORD id, const Type &entry) 
	{
		assert_debug(id < m_dwFill_size + m_aRemoved.size(), 
			FormatString( L"Выход за границы коллекции (%i >= %i).", id, m_dwFill_size + m_aRemoved.size()).c_str() );

		Vector2D<DWORD> dwInds = GetIndexs(id);

		//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), id ) == m_aRemoved.end() );
		assert_debug(m_setDebugRemoved.find(id) == m_setDebugRemoved.end(), L"");

		m_pHeads[vInds.x].m_pBuffer[vInds.y] = entry;
		SetBit(dwInds.y, false, m_pHeads[dwInds.x].m_bfRemoved);
	}

	__forceinline void RemoveAt(DWORD id)
	{
		assert_debug(id < m_dwFill_size + m_aRemoved.size(), 
			FormatString( L"Выход за границы коллекции (%i >= %i).", id, m_dwFill_size + m_aRemoved.size()).c_str() );
		assert_debug(m_dwFill_size > 0, L"Удаляем элемент у пустого контейнера!" );

		Vector2D<DWORD> vInds = GetIndexs(id);
		m_dwFill_size--;

		bool bLastElemSimpleRemove = IsSolid() && id == m_dwFill_size;
		if(!bLastElemSimpleRemove)
		{
			//assert_andrey( find( m_aRemoved.begin(), m_aRemoved.end(), id ) == m_aRemoved.end() );
			assert_debug(m_setDebugRemoved.find(id) == m_setDebugRemoved.end(), L"");

			m_aRemoved.push_back(id);
#ifdef _DEBUG
			m_setDebugRemoved.insert(id);
#endif

			assert_debug( GetBit(vInds.y, m_pHeads[vInds.x].m_bfRemoved) == 0, L"");
			SetBit(vInds.y, true, m_pHeads[vInds.x].m_bfRemoved);
		}

	};

	__forceinline void RemoveLast()
	{
		RemoveAt(m_dwFill_size - 1);
	}

	__forceinline DWORD GetMemorySize() const
	{
		DWORD dwBufferMemSize = GetBufferSize() * sizeof(Type);
		DWORD dwRemovedMemSize = GetBFSize() * sizeof(DWORD);
		DWORD dwCommonBufMemSize = (dwBufferMemSize + dwRemovedMemSize) * m_dwHeadsCount;
		DWORD dwHeadsMemSize = m_dwHeadsCount * sizeof(SHeadElementBuffer);

		dwCommonBufMemSize += dwHeadsMemSize + 4 * sizeof(DWORD); //+ service

		dwCommonBufMemSize += m_aRemoved.GetMemorySize();

		return dwCommonBufMemSize;
	}
};

template <class Type> 
void CStore<Type>::SetSize()
{
	assert_debug(m_dwOffset, L"");

	SHeadElementBuffer*	pNewHeads = new SHeadElementBuffer[m_dwHeadsCount + 1];

	if(m_dwHeadsCount)
		memcpy( pNewHeads, m_pHeads, m_dwHeadsCount * sizeof(SHeadElementBuffer) );

	SAFE_DELETE_ARRAY(m_pHeads);
	m_pHeads = pNewHeads;

	m_pHeads[m_dwHeadsCount].m_pBuffer = new Type[ GetBufferSize() ];
	m_pHeads[m_dwHeadsCount].m_bfRemoved = new DWORD[ GetBFSize() ];
	ClearBF( m_pHeads[m_dwHeadsCount].m_bfRemoved );

	m_dwHeadsCount++;
};
