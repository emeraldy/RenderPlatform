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
    m_pbBuffer = NULL;
    m_dwBufferSize = 0;
}

AssetIOHandler::~AssetIOHandler()
{
    SAFE_DELETEARRAY(m_pbBuffer);
}

//-----------------------------------------------------------------
// Class general function definitions
//-----------------------------------------------------------------
BOOL AssetIOHandler::LoadAsset(LPCWSTR lpcwFileName)
{
    DWORD dwFileSize;//4GB for a file
    HANDLE hFile;

    //open file
    hFile = CreateFile(lpcwFileName, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        //error message display
        unsigned int uiErrorInfoLength = (unsigned int)lstrlen(lpcwFileName);
        uiErrorInfoLength = uiErrorInfoLength + lstrlen(TEXT(" failed to be opened!")) + 1;//for the NULL terminator
        LPWSTR lpwErrorInfo = new WCHAR[uiErrorInfoLength];

        if (StringCbCopy(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), lpcwFileName) != S_OK)
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else if (StringCbCat(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), TEXT(" failed to be read!")) != S_OK)
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else
            MessageBox(NULL, lpwErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);

        SAFE_DELETEARRAY(lpwErrorInfo);
        return FALSE;
    }

    //read file
    DWORD dwBytesRead;
    dwFileSize = GetFileSize(hFile, NULL);
    m_pbBuffer = new BYTE[dwFileSize];
    if (ReadFile(hFile, m_pbBuffer, dwFileSize, &dwBytesRead, NULL) != TRUE)
    {
        //error message display
        unsigned int uiErrorInfoLength = (unsigned int)lstrlen(lpcwFileName);
        uiErrorInfoLength = uiErrorInfoLength + lstrlen(TEXT(" failed to be read!")) + 1;//for the NULL terminator
        LPWSTR lpwErrorInfo = new TCHAR[uiErrorInfoLength];

        if (StringCbCopy(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), lpcwFileName) != S_OK)
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else if (StringCbCat(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), TEXT(" failed to be read!")) != S_OK)
            MessageBox(NULL, TEXT("AssetIOHandler::LoadTextAsset String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else
            MessageBox(NULL, lpwErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);

        SAFE_DELETEARRAY(lpwErrorInfo);
        return FALSE;
    }

    m_dwBufferSize = dwBytesRead;

    //close file
    CloseHandle(hFile);

    return TRUE;
}