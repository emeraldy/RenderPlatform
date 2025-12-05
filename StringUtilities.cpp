//-----------------------------------------------------------------
// Handle string-related operations
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StringUtilities.h"
#include <cassert>

std::wstring Emerald::ConvertCharStringToWide(const std::string& source)
{
    std::wstring destination;
    std::size_t sourceLength = source.length();
    if (sourceLength == 0)
    {
        return destination;
    }
    destination.assign(sourceLength, '+');
    std::size_t destinationLength = std::mbstowcs(&destination[0], &source[0], sourceLength);
    assert(destinationLength == sourceLength);
    return destination;
}

std::string Emerald::ConvertWideStringToChar(const std::wstring& source)
{
    std::string destination;
    std::size_t sourceLength = source.length();
    if (sourceLength == 0)
    {
        return destination;
    }
    destination.assign(sourceLength, '+');
    std::size_t destinationLength = std::wcstombs(&destination[0], &source[0], sourceLength);
    assert(destinationLength == sourceLength);
    return destination;
}