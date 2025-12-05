//-----------------------------------------------------------------
// Handle string-related operations
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <string>

namespace Emerald
{
    std::wstring ConvertCharStringToWide(const std::string& source);
    std::string ConvertWideStringToChar(const std::wstring& source);
}