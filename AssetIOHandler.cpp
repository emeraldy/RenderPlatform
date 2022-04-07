//-----------------------------------------------------------------
// Asset IO handler class
// C++ Source - AssetIOHandler.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "AssetIOHandler.h"

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
BOOL AssetIOHandler::LoadAsset(LPCWSTR pFileName)
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
        unsigned int errorInfoLength = (unsigned int)lstrlen(pFileName);
        errorInfoLength = errorInfoLength + lstrlen(TEXT(" failed to be opened!")) + 1;//for the NULL terminator
        LPWSTR pErrorInfo = new WCHAR[errorInfoLength];

        if (StringCbCopy(pErrorInfo, errorInfoLength * sizeof(WCHAR), pFileName) != S_OK)
        {
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else if (StringCbCat(pErrorInfo, errorInfoLength * sizeof(WCHAR), TEXT(" failed to be read!")) != S_OK)
        {
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {
            MessageBox(NULL, pErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        SAFE_DELETEARRAY(pErrorInfo);
        return FALSE;
    }

    //read file
    DWORD bytesRead;
    fileSize = GetFileSize(file, NULL);
    m_pBuffer = new BYTE[fileSize];
    if (ReadFile(file, m_pBuffer, fileSize, &bytesRead, NULL) != TRUE)
    {
        //error message display
        unsigned int errorInfoLength = (unsigned int)lstrlen(pFileName);
        errorInfoLength = errorInfoLength + lstrlen(TEXT(" failed to be read!")) + 1;//for the NULL terminator
        LPWSTR pErrorInfo = new TCHAR[errorInfoLength];

        if (StringCbCopy(pErrorInfo, errorInfoLength * sizeof(WCHAR), pFileName) != S_OK)
        {
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else if (StringCbCat(pErrorInfo, errorInfoLength * sizeof(WCHAR), TEXT(" failed to be read!")) != S_OK)
        {
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {
            MessageBox(NULL, pErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        SAFE_DELETEARRAY(pErrorInfo);
        return FALSE;
    }

    m_bufferSize = bytesRead;

    //close file
    CloseHandle(file);

    return TRUE;
}