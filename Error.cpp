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
    if (!newText.empty())
    {
        text += newText + L'\n';
    }

    return *this;
}

Error Error::operator += (const Error& moreErr)
{
    if (moreErr)
    {
        text += moreErr.text + L'\n';
    }

    return *this;
}

Error Error::operator = (const std::wstring& newText)
{
    if (!newText.empty())
    {
        text = newText + L'\n';
    }

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