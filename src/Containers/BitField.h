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

	unsigned int NumberSet() const;									// Количество установленных битов //for check

	unsigned int GetLastSetBitCheck() const;				//индекс последнего бита, который больше нуля //for check
	unsigned int GetFirstSetBitCheck() const;				//индекс первого бита, который больше нуля //for check

public:
	CBitField(const CBitField& in_Src);

	CBitField() :
		m_dwSize(0),
		m_pField(0)
	{ 
	}

	CBitField(unsigned int _size);						// Создает битовое поле заданной длины
	~CBitField();

	void Append(CBitField aField);						// Добавить битовое поле
	void ClearAll();									// Очистить массив
	void SetAll();										// Установить все в 1

	void Resize(size_t new_size, bool in_bRaiseNewBits = false);					// Изменить размер поля
	void Delete();										// Очистка
	bool Erase(unsigned int bit_num);

	DWORD GetSetBitCount() const;						// Количество установленных битов

	__forceinline DWORD GetMaskBit(unsigned int bit_num) const // Считать маскированый бит
	{
		assert_debug((int)bit_num >= 0 && bit_num < m_dwSize, FormatString(L"Выход за границы коллекции (%i >= %i).", bit_num, m_dwSize).c_str() );

		int offset = bit_num & 31;
		return (m_pField[bit_num >> 5] & (1 << offset));
	};

	__forceinline int GetBit(unsigned int bit_num) const // Считать бит
	{
		int offset = bit_num & 31;
		return GetMaskBit(bit_num) >> offset;
	};

	__forceinline int SetBit(unsigned int bit_num, int value)	// Установить значение бита в value
	{
		assert_debug((int)bit_num >= 0 && bit_num < m_dwSize, FormatString(L"Выход за границы коллекции (%i >= %i).", bit_num, m_dwSize).c_str());

		int offset = bit_num & 31;
		if (value)
			m_pField[bit_num>>5] |= 1 << offset;
		else
			m_pField[bit_num>>5] &= ~(1 << offset);
		return value;
	};

	__forceinline void RaiseBit(unsigned int bit_num)	// Установить значение бита в 1
	{
		assert_debug(bit_num < m_dwSize, FormatString(L"Выход за границы коллекции (%i >= %i).", bit_num, m_dwSize).c_str() );
		int offset = bit_num & 31;
		m_pField[bit_num>>5] |= 1 << offset;
	}

	__forceinline int operator[](unsigned int bit_num) const	// Переопределенный оператор массива
	{
		assert_debug((int)bit_num >= 0 && bit_num < m_dwSize, FormatString(L"Выход за границы коллекции (%i >= %i).", bit_num, m_dwSize).c_str() );

		int offset = bit_num & 31;
		return (m_pField[bit_num >> 5] & (1 << offset)) >> offset;
	};

	__forceinline DWORD GetSize() const {return m_dwSize;}	// Размер битового поля

	DWORD GetLastSetBit() const;				//индекс последнего бита, который больше нуля
	DWORD GetFirstSetBit() const;				//индекс первого бита, который больше нуля
	bool FindFirstClear(DWORD& out_dwBit);		// Ищем первый сброшеный бит. Индекс вернем в out_dwBit, успех - в качестве результата. Поиск начинается с 0.
	bool FindFirstSet(DWORD& io_dwBit);			// Ищем первый установленный бит, начиная с некоторого заданного в io_dwBit


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
		if (&v != this) //чтобы memcpy не получил указатели на один и тот же участок памяти
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
