//-----------------------------------------------------------------
//This class only handles Windows file I/O in byte form. It doesn't
//interpret the actual content it reads or writes.
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "RawDataReader.h"
#include "StandardIncludes.h"


namespace Emerald
{
    class WinFileRawDataReader : public RawDataReader
    {
    private:
        BYTE* pBuffer;
        DWORD bufferSize;//in byte, of course

    public:
        WinFileRawDataReader();
        virtual ~WinFileRawDataReader();

        Error LoadRawData(const std::wstring& sourceName) override;
        const unsigned char* GetBuffer() const override;
        unsigned int GetBufferSize() const override;
        void clearBuffer() override;
    };
}