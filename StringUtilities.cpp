//-----------------------------------------------------------------
// String Utilities Class
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
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