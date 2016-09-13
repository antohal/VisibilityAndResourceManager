// BitField.h: interface for the BitField class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CEngineFile;

class CBitField
{
	DWORD* m_pField;
	DWORD m_dwSize;

	// size in DWORDs
	DWORD GetBFSize() const
	{ 
		return (m_dwSize >> 5) + 1;
	}

	unsigned int NumberSet() const;									// ���������� ������������� ����� //for check

	unsigned int GetLastSetBitCheck() const;				//������ ���������� ����, ������� ������ ���� //for check
	unsigned int GetFirstSetBitCheck() const;				//������ ������� ����, ������� ������ ���� //for check

public:
	CBitField(const CBitField& in_Src);

	CBitField() :
		m_dwSize(0),
		m_pField(0)
	{ 
	}

	CBitField(unsigned int _size);						// ������� ������� ���� �������� �����
	~CBitField();

	void Append(CBitField aField);						// �������� ������� ����
	void ClearAll();									// �������� ������
	void SetAll();										// ���������� ��� � 1

	void Resize(size_t new_size, bool in_bRaiseNewBits = false);					// �������� ������ ����
	void Delete();										// �������
	bool Erase(unsigned int bit_num);

	DWORD GetSetBitCount() const;						// ���������� ������������� �����

	__forceinline DWORD GetMaskBit(unsigned int bit_num) const // ������� ������������ ���
	{
		assert_debug((int)bit_num >= 0 && bit_num < m_dwSize, FormatString(L"����� �� ������� ��������� (%i >= %i).", bit_num, m_dwSize).c_str() );

		int offset = bit_num & 31;
		return (m_pField[bit_num >> 5] & (1 << offset));
	};

	__forceinline int GetBit(unsigned int bit_num) const // ������� ���
	{
		int offset = bit_num & 31;
		return GetMaskBit(bit_num) >> offset;
	};

	__forceinline int SetBit(unsigned int bit_num, int value)	// ���������� �������� ���� � value
	{
		assert_debug((int)bit_num >= 0 && bit_num < m_dwSize, FormatString(L"����� �� ������� ��������� (%i >= %i).", bit_num, m_dwSize).c_str());

		int offset = bit_num & 31;
		if (value)
			m_pField[bit_num>>5] |= 1 << offset;
		else
			m_pField[bit_num>>5] &= ~(1 << offset);
		return value;
	};

	__forceinline void RaiseBit(unsigned int bit_num)	// ���������� �������� ���� � 1
	{
		assert_debug(bit_num < m_dwSize, FormatString(L"����� �� ������� ��������� (%i >= %i).", bit_num, m_dwSize).c_str() );
		int offset = bit_num & 31;
		m_pField[bit_num>>5] |= 1 << offset;
	}

	__forceinline int operator[](unsigned int bit_num) const	// ���������������� �������� �������
	{
		assert_debug((int)bit_num >= 0 && bit_num < m_dwSize, FormatString(L"����� �� ������� ��������� (%i >= %i).", bit_num, m_dwSize).c_str() );

		int offset = bit_num & 31;
		return (m_pField[bit_num >> 5] & (1 << offset)) >> offset;
	};

	__forceinline DWORD GetSize() const {return m_dwSize;}	// ������ �������� ����

	DWORD GetLastSetBit() const;				//������ ���������� ����, ������� ������ ����
	DWORD GetFirstSetBit() const;				//������ ������� ����, ������� ������ ����
	bool FindFirstClear(DWORD& out_dwBit);		// ���� ������ ��������� ���. ������ ������ � out_dwBit, ����� - � �������� ����������. ����� ���������� � 0.
	bool FindFirstSet(DWORD& io_dwBit);			// ���� ������ ������������� ���, ������� � ���������� ��������� � io_dwBit


	void	SetFirstNumBit(unsigned int bit_num, int value)
	{
		assert_debug( bit_num <= GetSize(), L"out of range" );
		for(unsigned int dw = 0; dw < bit_num; dw++)
		{
			SetBit(dw, value);
		}
	}

	CBitField& operator = (const CBitField& v)
	{ 
		if (&v != this) //����� memcpy �� ������� ��������� �� ���� � ��� �� ������� ������
		{	
			if( GetSize() != v.GetSize() )
				Resize( v.GetSize() );

			if( m_dwSize )
				memcpy(m_pField, v.m_pField, GetBFSize()*sizeof(m_pField[0]));
		}
		return *this; 
	}

	void Union(const CBitField& in_yBitField);

	void resize(size_t nSize) {
		Resize(nSize);
	}

	size_t size() const {
		return GetSize();
	}

	DWORD	SaveLoadBits(DWORD in_dwPosition,DWORD in_dwCount, bool* io_bBits,bool in_bSave);
};
