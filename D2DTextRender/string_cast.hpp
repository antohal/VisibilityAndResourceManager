#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <cwchar>
#include <cassert>

template<typename Td> 
Td string_cast( const char* pSource, unsigned int codePage = CP_ACP );

template<typename Td> 
Td string_cast( const wchar_t* pSource, unsigned int codePage = 1200 );

template<typename Td> 
Td string_cast( const std::string& source, unsigned int codePage = CP_ACP );

template<typename Td> 
Td string_cast( const std::wstring& source, unsigned int codePage = 1200 );

template<>
std::string string_cast( const char* pSource, unsigned int codePage )
{
    assert( pSource != 0 );
    return std::string( pSource );
}

template<>
std::wstring string_cast( const char* pSource, unsigned int codePage )
{
    assert( pSource != 0 );
    std::size_t sourceLength = std::strlen( pSource );
    if( sourceLength == 0 )
    {
        return std::wstring();
    }

    int length = ::MultiByteToWideChar( codePage, 0, pSource, static_cast<int>(sourceLength), NULL, 0 );
    if( length == 0 )
    {
        return std::wstring();
    }

    std::vector<wchar_t> buffer( length );
    ::MultiByteToWideChar( codePage, 0, pSource, static_cast<int>(sourceLength), &buffer[ 0 ], length );

    return std::wstring( buffer.begin(), buffer.end() );
}

template<>
std::string string_cast( const wchar_t* pSource, unsigned int codePage )
{
    assert( pSource != 0 );
    size_t sourceLength = std::wcslen( pSource );
    if( sourceLength == 0 )
    {
        return std::string();
    }

    int length = ::WideCharToMultiByte( codePage, 0, pSource, static_cast<int>(sourceLength), NULL, 0, NULL, NULL );
    if( length == 0 )
    {
        return std::string();
    }

    std::vector<char> buffer( length );
    ::WideCharToMultiByte( codePage, 0, pSource, static_cast<int>(sourceLength), &buffer[ 0 ], length, NULL, NULL );

    return std::string( buffer.begin(), buffer.end() );
}

template<>
std::wstring string_cast( const wchar_t* pSource, unsigned int codePage )
{
    assert( pSource != 0 );
    return std::wstring( pSource );
}

template<>
std::string string_cast( const std::string& source, unsigned int codePage )
{
    return source;
}

template<>
std::wstring string_cast( const std::string& source, unsigned int codePage )
{
    if( source.empty() )
    {
        return std::wstring();
    }

    int length = ::MultiByteToWideChar( codePage, 0, source.data(), static_cast<int>(source.length()), NULL, 0 );
    if( length == 0 )
    {
        return std::wstring();
    }

    std::vector<wchar_t> buffer( length );
    ::MultiByteToWideChar( codePage, 0, source.data(), static_cast<int>(source.length()), &buffer[ 0 ], length );

    return std::wstring( buffer.begin(), buffer.end() );
}

template<>
std::string string_cast( const std::wstring& source, unsigned int codePage )
{
    if( source.empty() )
    {
        return std::string();
    }

    int length = ::WideCharToMultiByte( codePage, 0, source.data(), static_cast<int>(source.length()), NULL, 0, NULL, NULL );
    if( length == 0 )
    {
        return std::string();
    }

    std::vector<char> buffer( length );
    ::WideCharToMultiByte( codePage, 0, source.data(), static_cast<int>(source.length()), &buffer[ 0 ], length, NULL, NULL );

    return std::string( buffer.begin(), buffer.end() );
}

template<>
std::wstring string_cast( const std::wstring& source, unsigned int codePage )
{
    return source;
}
