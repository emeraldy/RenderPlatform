//-----------------------------------------------------------------
// String utilities Class Header
// 
// Handle string-related operations
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <string>
#include "Error.h"

namespace Emerald
{
    class StringUtilities
    {
    public:
        static std::wstring ConvertCharStringToWide(const std::string& source);
        static std::string ConvertWideStringToChar(const std::wstring& source);
        static std::wstring DecodeStringBytes(unsigned int codePage, const unsigned char* sourceBytes, int sourceByteCount, unsigned int flag, Error& err);
    };
}