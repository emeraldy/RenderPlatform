//-----------------------------------------------------------------
//This class only handles asset file I/O in byte form. It doesn't
//interpret the actual content it reads or writes.
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "AssetIOHandler.h"
using namespace Emerald;
//-----------------------------------------------------------------
// Class constructor/destructor definitions
//-----------------------------------------------------------------
AssetIOHandler::AssetIOHandler()
{
    m_pBuffer = NULL;
    m_bufferSize = 0;
}

AssetIOHandler::~AssetIOHandler()
{
    SAFE_DELETEARRAY(m_pBuffer);
}

//-----------------------------------------------------------------
// Class general function definitions
//-----------------------------------------------------------------
BOOL AssetIOHandler::LoadRawAssetData(LPCWSTR pFileName)
{
    DWORD fileSize;//4GB for a file
    HANDLE file;

    //open file
    file = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        //error message display
        std::wstring errorInfo(pFileName);
        errorInfo.append(TEXT(" failed to be opened!"));
        MessageBox(NULL, errorInfo.c_str(), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    //read file
    DWORD bytesRead;
    fileSize = GetFileSize(file, NULL);
    m_pBuffer = new BYTE[fileSize];
    if (ReadFile(file, m_pBuffer, fileSize, &bytesRead, NULL) != TRUE)
    {
        //error message display
        std::wstring errorInfo(pFileName);
        errorInfo.append(TEXT(" failed to be read!"));
        MessageBox(NULL, errorInfo.c_str(), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    m_bufferSize = bytesRead;

    //close file
    CloseHandle(file);

    return TRUE;
}