//-----------------------------------------------------------------
// Error Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <string>

namespace Emerald
{
    class Error
    {
    public:
        Error operator += (const std::wstring& newText);
        Error operator += (const Error& moreErr);
        Error operator = (const std::wstring& newText);
        explicit operator bool() const;
        const std::wstring GetErrorText() const;
    private:
        std::wstring text;
    };
}