#pragma once

static __m64 pmm64DupBits[8];

struct SDwordByByte
{
	union
	{
		struct 
		{
			BYTE btLowestByte; //младший
			BYTE btLowByte;
			BYTE btHighByte;
			BYTE btHighestByte; //старший
		};
		DWORD	dwDword;
	};
};


__forceinline BYTE GetLow3BitsFromInt64(unsigned __int64 in_M64)
{
	return static_cast<BYTE>(in_M64 & 7);
}


__forceinline void ShearRight3Bit(__m64& io_Val)
{

	io_Val.m64_u64 = (io_Val.m64_u64 >> 3);
}

__forceinline void ShearLeft3Bit(__m64& io_Val)
{

	io_Val.m64_u64 = (io_Val.m64_u64 << 3);
}

__forceinline BYTE ShearRight3BitAndGetLow3BitsFromM64(__m64& io_Val)
{

	ShearRight3Bit(io_Val);
	return GetLow3BitsFromInt64(io_Val.m64_u64);
}

__forceinline void SDwordByByteTo64(const SDwordByByte& HAdr, const SDwordByByte& MAdr, const SDwordByByte& LAdr, __m64& out_mm64Coord)
{
	BYTE Tmp[8];

	SDwordByByte* TmpH = (SDwordByByte*) (Tmp + 4);
	TmpH->btLowestByte = MAdr.btLowByte;
	TmpH->btLowByte = MAdr.btHighByte;
	TmpH->btHighByte = LAdr.btLowestByte;
	TmpH->btHighestByte = LAdr.btLowByte;

	SDwordByByte* TmpL = (SDwordByByte*) Tmp;
	TmpL->dwDword = HAdr.dwDword;
	TmpL->btHighestByte = MAdr.btLowestByte;

	__m64* outP = &out_mm64Coord;
	outP->m64_u64 = *((unsigned __int64*)Tmp);
}

__forceinline bool IsEqualMM64(__m64 in_Val1, __m64 in_Val2)
{

	bool bRes = in_Val1.m64_u64 == in_Val2.m64_u64;

//	_mm_empty();
	return bRes;
}

BYTE GetEqualDepth(__m64 in_Val1, __m64 in_Val2)
{
	BYTE btIndex = g_btDepthCount;
	while ( !IsEqualMM64(in_Val1, in_Val2) )
	{
		ShearLeft3Bit(in_Val1);
		ShearLeft3Bit(in_Val2);
		btIndex--;
	}
//	_mm_empty();
	return btIndex;
}

__forceinline void SuperShear(int in_iChild, const __m64& in_Min, const __m64& in_Max, __m64& out_Min, __m64& out_Max )
{
	__m64 mm0 = in_Max;
	__m64 mm1 = in_Min;

	mm0.m64_u64 = mm0.m64_u64 ^ mm1.m64_u64;
	DWORD edx = mm0.m64_u32[0];

	mm1.m64_u64 = mm1.m64_u64 >> 3;
	DWORD ecx = in_iChild;
	DWORD ebx = ecx;

	ecx &= edx;
	ecx = ~ecx;
	ecx &= 7;

	mm0 = pmm64DupBits[ecx];
	mm1.m64_u64 = mm1.m64_u64 & mm0.m64_u64;

	mm0 = in_Max;
	out_Min = mm1;
	mm0.m64_u64 = mm0.m64_u64 >> 3;

	ebx = ~ebx;
	ebx &= edx;
	ebx &= 7;

	mm1 = pmm64DupBits[ebx];
	mm0.m64_u64 = mm0.m64_u64 | mm1.m64_u64;
	out_Max = mm0;

	//_mm_empty();
}