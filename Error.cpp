//-----------------------------------------------------------------
// Error Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Error.h"

using namespace Emerald;

Error Error::operator += (const std::wstring& newText)
{
    text += newText + L'\n';

    return *this;
}

Error Error::operator = (const std::wstring& newText)
{
    text = newText + L'\n';

    return *this;
}

const std::wstring Error::GetErrorText() const
{
    return text;
}

Error::operator bool() const
{
    //if(err) is true means there is error => text is not empty
    return !text.empty();
}