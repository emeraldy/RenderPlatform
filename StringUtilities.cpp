//-----------------------------------------------------------------
// String Utilities Class
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <Windows.h>
#include "StringUtilities.h"

using namespace Emerald;

std::wstring StringUtilities::ConvertCharStringToWide(const std::string& source)
{
    std::wstring destination;
    std::size_t sourceLength = source.length();
    if (sourceLength == 0)
    {
        return destination;
    }
    destination.assign(sourceLength, '+');
    std::size_t destinationLength = std::mbstowcs(&destination[0], &source[0], sourceLength);
    if (destinationLength == static_cast<std::size_t>(-1))
    {
        destination.clear();
    }
    return destination;
}

std::string StringUtilities::ConvertWideStringToChar(const std::wstring& source)
{
    std::string destination;
    std::size_t sourceLength = source.length();
    if (sourceLength == 0)
    {
        return destination;
    }
    destination.assign(sourceLength, '+');
    std::size_t destinationLength = std::wcstombs(&destination[0], &source[0], sourceLength);
    if (destinationLength == static_cast<std::size_t>(-1))
    {
        destination.clear();
    }
    return destination;
}

std::wstring StringUtilities::DecodeStringBytes(unsigned int codePage, const unsigned char* sourceBytes, int sourceByteCount, unsigned int flag, Error& err)
{
    wchar_t* pDestination = new wchar_t[sourceByteCount + 1];//enough space for conversion, plus a terminating null.

    int resultLength = 0;
    resultLength = MultiByteToWideChar(codePage, flag, reinterpret_cast<const char*>(sourceBytes), sourceByteCount, pDestination, sourceByteCount + 1);
    if (resultLength == 0)
    {
        err += L"String byte stream decoding failed with DecodeStringBytes().";
        delete[] pDestination;
        return std::wstring();
    }
    else
    {
        pDestination[resultLength] = L'\0';
        std::wstring result(pDestination, resultLength + 1);
        delete[] pDestination;
        return result;
    }
}