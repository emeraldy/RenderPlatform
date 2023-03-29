//-----------------------------------------------------------------
//This class only handles asset file I/O in byte form. It doesn't
//interpret the actual content it reads or writes.
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <string>
#include "pch.h"


namespace Emerald
{
    class AssetIOHandler
    {
    protected:
        BYTE* m_pBuffer;
        DWORD m_bufferSize;//in byte, of course

    public:
        //Constructor/Destructor
        AssetIOHandler();
        virtual ~AssetIOHandler();

        //General class functions
        BOOL LoadRawAssetData(LPCWSTR lpcwFileName);

        //Accessors
        const BYTE* GetBuffer() { return m_pBuffer; }
        void        SetBuffer(BYTE* pNewBuffer) { SAFE_DELETEARRAY(m_pBuffer); m_pBuffer = pNewBuffer; }//test this!!!!!!!!!!!!
        DWORD       GetBufferSize() { return m_bufferSize; }
        void        SetBufferSize(DWORD newSize) { m_bufferSize = newSize; }
    };
}