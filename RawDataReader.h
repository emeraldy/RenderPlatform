//-----------------------------------------------------------------
// Raw Byte Reader Base Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Error.h"

namespace Emerald
{
    class RawDataReader
    {
    public:
       virtual Error LoadRawData(const std::wstring& sourceName) = 0;
       virtual const unsigned char* GetBuffer() const = 0;
       virtual unsigned int GetBufferSize() const = 0;
       virtual void clearBuffer() = 0;
    };
}