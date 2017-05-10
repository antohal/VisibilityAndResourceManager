#pragma once

#include <string>

template <class T> void SafeDelete (T*& pObject)
{
	if (pObject)
		delete pObject;
	pObject = NULL;
}

template <class T> void SafeDestroy (T*& pObject)
{
	if (pObject)
		pObject->Destroy();
	pObject = NULL;
}

#define FRAND static_cast<float>(rand())/static_cast<float>(RAND_MAX)


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define ARRAY_END(array) ((array) + ARRAY_SIZE(array))

#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete[] (p);   (p)=NULL; } }

__forceinline DWORD GetHighestSetBit (DWORD value)
{
	DWORD dwRet(-1);
	if (! _BitScanReverse(&dwRet, value))
		return -1;

	return dwRet;
}

__forceinline DWORD GetLowestSetBit(DWORD value)
{
	DWORD dwRet(-1);
	if (! _BitScanForward(&dwRet, value))
		return -1;

	return dwRet;
}

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif


template<class Container, typename _Fn>
inline void for_each_in (const Container& container, const _Fn& func)
{
	std::for_each(container.begin(), container.end(), func);
}

void replace_in_str(std::string& str, const std::string& oldStr, const std::string& newStr);
void replace_char_in_str(std::string& str, const char ch, const std::string& newStr);

std::string GetOnlyHexDigits(const std::string& str);

std::string IPAddressToString (DWORD in_dwIP);
void ConvertIPToBytes (DWORD in_dwIP, BYTE out_pbBytes[4]);
DWORD ConvertBytesToIP (BYTE in_pbBytes[4]);

template <typename Type>
__forceinline bool IsEqualT( Type in_Float1, Type in_Float2, Type in_fEpsilon = Type(1e-4f) )
{
	return static_cast<Type>( fabs( in_Float1 - in_Float2 ) ) < in_fEpsilon;
}

template <typename TYPE>
inline TYPE TAbs( const TYPE& v )
{
	if( v < TYPE(0) )
		return -v;
	return v;
}

std::wstring GetStartDirectory();