//-----------------------------------------------------------------
//This class only handles Windows file I/O in byte form. It doesn't
//interpret the actual content it reads or writes.
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "WinFileRawDataReader.h"
using namespace Emerald;
//-----------------------------------------------------------------
// Class constructor/destructor definitions
//-----------------------------------------------------------------
WinFileRawDataReader::WinFileRawDataReader()
{
    pBuffer = NULL;
    bufferSize = 0;
}

WinFileRawDataReader::~WinFileRawDataReader()
{
    SAFE_DELETEARRAY(pBuffer);
}

//-----------------------------------------------------------------
// Class general function definitions
//-----------------------------------------------------------------
Error WinFileRawDataReader::LoadRawData(const std::wstring& sourceName)
{
    DWORD fileSize;//4GB for a file
    HANDLE file;
    Error err;

    clearBuffer();
    //open file
    file = CreateFile(sourceName.c_str(), GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        err = sourceName + L" open failed.";
        return err;
    }

    //read file
    DWORD bytesRead;
    fileSize = GetFileSize(file, NULL);
    pBuffer = new BYTE[fileSize];
    if (ReadFile(file, pBuffer, fileSize, &bytesRead, NULL) != TRUE)
    {
        err = sourceName + L" read failed.";
        return err;
    }

    bufferSize = bytesRead;

    //close file
    CloseHandle(file);

    return err;
}

const unsigned char* WinFileRawDataReader::GetBuffer() const
{
    return pBuffer;
}

unsigned int WinFileRawDataReader::GetBufferSize() const
{
    return bufferSize;
}

void WinFileRawDataReader::clearBuffer()
{
    SAFE_DELETEARRAY(pBuffer);
    bufferSize = 0;
}