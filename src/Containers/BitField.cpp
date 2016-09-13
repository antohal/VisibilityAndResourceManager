// CBitField.cpp: implementation of the CBitField class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BitField.h"

BYTE SetBitCountTable[256] = 
{ 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,2,3,3,4,2,3,3,4,3,4,
4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,
4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,
3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,
4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,
5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8}; //сколько установлено битов в любом числе от 0 до 256

DWORD SetBitDWORDTable[32] = 
{ 0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535,131071,
262143,524287,1048575,2097151,4194303,8388607,16777215,33554431,67108863,
134217727,268435455,536870911,1073741823,2147483647}; //в рез. числе столько установленных бит - какой индекс

__forceinline DWORD GetMask(DWORD in_dwVal)
{
	if( in_dwVal > 31 )
		return DWORD(-1);
	return SetBitDWORDTable[in_dwVal];
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//BYTE GetSetBitCount(BYTE in_Byte)
//{
//	BYTE btCount = 0;
//	for(BYTE bt = 0; bt < 8; bt++)
//	{
//		btCount += (in_Byte >> bt) & 1;
//	}
//	return btCount;
//}

DWORD SetFirstBit(BYTE in_btCount)
{
	DWORD dwRes = 1;
	for(BYTE bt = 1; bt < in_btCount; bt++)
	{
		dwRes = dwRes << 1;
		dwRes++;
	}
	return dwRes;
}

CBitField::CBitField(const CBitField& in_Src) :
	m_pField(0),
	m_dwSize(0)
{
	Resize(in_Src.m_dwSize);

	if(m_dwSize) {
		memcpy(m_pField, in_Src.m_pField, GetBFSize() * sizeof(DWORD));
	}
}


CBitField::CBitField(unsigned int _size)
{
	m_dwSize = _size;
	m_pField = new DWORD[(_size >> 5) + 1];
	ClearAll();

	//	for(DWORD bt = 0; bt < 256; bt++)
	//	{
	//		BYTE btC = ::GetSetBitCount( BYTE(bt) );
	//		Trace("%u,", btC);
	//	}

	//	for(BYTE bt = 0; bt < 32; bt++)
	//	{
	//		DWORD dwC = SetFirstBit( bt );
	//		Trace("%u,", dwC);
	//	}
}

CBitField::~CBitField()
{
	SafeDelete(m_pField);
}

void CBitField::Delete()
{
	SafeDelete(m_pField);
	m_dwSize = 0;
}

void CBitField::Resize(size_t new_size,bool in_bRaiseNewBits)
{
	if (new_size == 0)
	{
		Delete();
		return;
	}

	int OldByteSize = (m_dwSize >> 5) + 1;
	int NewByteSize = (static_cast<int>(new_size) >> 5) + 1;

	if (m_dwSize)
	{
		m_dwSize = static_cast<DWORD>(new_size);
		if (OldByteSize != NewByteSize || m_pField == NULL)
		{
			DWORD* new_field = new DWORD[NewByteSize];
			FillMemory( new_field, sizeof(DWORD) * NewByteSize, in_bRaiseNewBits? BYTE(-1): 0 );
			if (m_pField)
			{
				memcpy(new_field, m_pField, min(OldByteSize, NewByteSize) * sizeof(DWORD));
				delete m_pField;
			}
			m_pField = new_field;
		}
	}
	else
	{
		assert_debug(!m_pField, L"bitfield must not be allocated here");
		m_dwSize = static_cast<DWORD>(new_size);
		m_pField = new DWORD[NewByteSize];
		FillMemory( m_pField, sizeof(DWORD) * NewByteSize, in_bRaiseNewBits? BYTE(-1): 0 );
	}
}

unsigned int CBitField::NumberSet() const
{
	unsigned int num = 0;
	for(unsigned int i = 0; i < m_dwSize; i++)
	{
		if(GetBit(i)) num++;
	}
	return num;
}

DWORD CBitField::GetSetBitCount() const
{
	DWORD dwCount = 0;
	for(DWORD dw = 0; dw < GetBFSize(); dw++)
	{
		//		DWORD dwLast = m_dwSize - 32 * dw;
		//		DWORD dwMask = GetMask( dwLast );
		DWORD dwContant = m_pField[dw];
		//		dwContant &= dwMask;

		dwCount += SetBitCountTable[ BYTE(dwContant) ];
		dwContant = dwContant >> 8;
		dwCount += SetBitCountTable[ BYTE(dwContant) ];
		dwContant = dwContant >> 8;
		dwCount += SetBitCountTable[ BYTE(dwContant) ];
		dwContant = dwContant >> 8;
		dwCount += SetBitCountTable[ BYTE(dwContant) ];
	}
	assert_debug( dwCount == NumberSet(), L"Illegal count" );
	return dwCount;
}


void CBitField::SetAll()
{
	if(m_dwSize)
	{
		FillMemory( m_pField, sizeof(DWORD) * GetBFSize(), BYTE(-1) );

		BYTE btOffset = BYTE(m_dwSize & 31);
		m_pField[GetBFSize() - 1] &= SetBitDWORDTable[btOffset];
	}
}

void CBitField::ClearAll()
{
	if(m_dwSize)
	{
		ZeroMemory( m_pField, sizeof(DWORD) * GetBFSize() );
	}
}

void CBitField::Append(CBitField aField)
{
	int old_size = m_dwSize;
	Resize(m_dwSize + aField.m_dwSize);
	for(unsigned int i = m_dwSize; i < m_dwSize + aField.m_dwSize; i++)
		SetBit(i,aField[i]);
}

DWORD CBitField::GetLastSetBit() const
{
	DWORD dwRes = DWORD(-1);
	for(DWORD dw = GetBFSize() - 1; dw >= 0; dw--)
	{
		DWORD dwContant = m_pField[dw];
		if( dwContant )
		{
			DWORD dwOffset = GetHighestSetBit(dwContant);
			dwRes = 32 * dw + dwOffset;
			break;
		}
	}

	assert_debug( GetLastSetBitCheck() == dwRes, FormatString( L"GetLastSetBit! GetLastSetBitCheck(): %d, dwRes: %d ", GetLastSetBitCheck(), dwRes ).c_str() );
	return dwRes;
}

DWORD CBitField::GetFirstSetBit() const
{
	DWORD dwRes = DWORD(-1);
	for(DWORD dw = 0; dw < GetBFSize(); dw++)
	{
		DWORD dwContant = m_pField[dw];
		if( dwContant )
		{
			DWORD dwOffset = GetLowestSetBit(dwContant);
			dwRes = 32 * dw + dwOffset;
			break;
		}
	}

	assert_debug( GetFirstSetBitCheck() == dwRes, FormatString( L"GetFirstSetBit! GetFirstSetBitCheck(): %d, dwRes: %d ", GetFirstSetBitCheck(), dwRes ).c_str() );
	return dwRes;
}

unsigned int CBitField::GetLastSetBitCheck() const
{
	for(unsigned int dw = GetSize() - 1; dw >= 0; dw--)
	{
		if( GetBit(dw) > 0 )
			return dw;
	}
	return unsigned int(-1);
}

unsigned int CBitField::GetFirstSetBitCheck() const
{
	for(unsigned int dw = 0; dw < GetSize(); dw++)
	{
		if( GetBit(dw) > 0 )
			return dw;
	}
	return unsigned int(-1);
}

bool CBitField::FindFirstClear(DWORD& io_dwBit)
{
	unsigned length = (GetSize()+31)>>5;
	DWORD mask = ((DWORD)(~0))<<io_dwBit;
	DWORD start = io_dwBit & 31;
	for (unsigned dw = io_dwBit>>5; dw < length; dw++)
	{
		if ((m_pField[dw] & mask) != mask) {	//  акой-то из битов маски сброшен. (2 & 7) != 7
			DWORD dwCurrOffset = 1L << start;
			DWORD dwCurrDWORD = m_pField[dw];
			// ѕри переходе i на следующее слово dwCurrOFfset станет равным нулю.
			for(unsigned i = (dw<<5)+start;i<m_dwSize && dwCurrOffset;i++,dwCurrOffset <<= 1)
				if (!(dwCurrOffset & dwCurrDWORD))	// Ќайден!
				{
					assert_debug(!GetBit(i), L"Illegal bit");
					io_dwBit = i;
					return true;
				}
				assert_debug(0, L"Illegal code point");
		}
		mask = ~0;
		start = 0;
	}
	return false;
}

bool CBitField::FindFirstSet(DWORD& io_dwBit)
{
	unsigned length = (GetSize()+31)>>5;
	DWORD mask = ((DWORD)(~0))<<io_dwBit;
	DWORD start = io_dwBit & 31;
	for (unsigned dw = io_dwBit>>5; dw < length; dw++)
	{
		if ((m_pField[dw] & mask) != 0) {	//  акой-то из битов взведен.
			DWORD dwCurrOffset = 1L << start;
			DWORD dwCurrDWORD = m_pField[dw];
			// ѕри переходе i на следующее слово dwCurrOFfset станет равным нулю.
			for(unsigned i = (dw<<5)+start;i<m_dwSize && dwCurrOffset;i++,dwCurrOffset <<= 1)
				if (dwCurrOffset & dwCurrDWORD)	// Ќайден!
				{
					assert_debug(GetBit(i), L"Illegal bit");
					io_dwBit = i;
					return true;
				}
				assert_debug(0, L"Illegal");
		}
		mask = ~0;
		start = 0;
	}
	return false;
}

void CBitField::Union(const CBitField& in_yBitField)
{
	DWORD dwOurSize = GetBFSize();
	DWORD dwYSize = in_yBitField.GetBFSize();
	DWORD dwProcessSize = min(dwOurSize,dwYSize);
	DWORD* pdwYField = in_yBitField.m_pField;
	for (DWORD i = 0; i<dwProcessSize; i++)
		m_pField[i] |= pdwYField[i];
}

bool CBitField::Erase(unsigned int bit_num)
{
	if (bit_num >= m_dwSize)
	{
		return false;
	}
	BYTE* btField = (BYTE*)m_pField;
	BYTE mask[8] = {0,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};
	DWORD dwByte = (bit_num >> 3);
	DWORD dwBit = bit_num%8;
	BYTE btSave = btField[dwByte] & mask[dwBit];
	BYTE btShifted = btField[dwByte] - btSave;
	btShifted = btShifted >> 1;
	if(dwByte*8 < m_dwSize)
		btShifted |= (btField[dwByte+1] << 7) & 0x80;
	btField[dwByte] = btSave | (btShifted & ~mask[dwBit]);

	for (DWORD i = dwByte%4+1; i<4; i++)
	{
		DWORD dwShiftedByte = dwByte+i;
		btField[dwShiftedByte] = btField[dwShiftedByte] >> 1;
		if(i == 3 && dwShiftedByte*8 < m_dwSize)
			btField[dwShiftedByte] |= (btField[dwShiftedByte+1] << 7) & 0x80;

	}
	DWORD dwCount = (m_dwSize >> 5) + 1;
	for (DWORD i = (bit_num >> 5)+1; i< dwCount; i++)
	{
		m_pField[i] = m_pField[i] >> 1;
		if(i < dwCount -1)
			m_pField[i] |= (m_pField[i+1] << 31) & 0x80000000;
	}
	Resize(m_dwSize-1);

	return true;
}

DWORD	CBitField::SaveLoadBits(DWORD in_dwPosition,DWORD in_dwCount, bool* io_bBits,bool in_bSave)
{
	int dwEnd = MAX(0,MIN(in_dwCount,m_dwSize-in_dwPosition));//   // MIN(m_dwSize,in_dwPosition+ in_dwCount);
	if(in_bSave)
		for(int dw = 0; dw < dwEnd; dw++)
			SetBit(in_dwPosition+dw, io_bBits[dw]? 1 : 0);
	else
		for(int dw = 0; dw < dwEnd; dw++)
			io_bBits[dw] = GetBit(in_dwPosition+dw)>0;
	return dwEnd;
}
