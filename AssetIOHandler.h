//-----------------------------------------------------------------
// Asset IO handler class
// C++ Header - AssetIOHandler.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <Strsafe.h>

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------
#define SAFE_DELETE(p) {if(p) {delete p; p = NULL;}}
#define SAFE_DELETEARRAY(p) {if(p) {delete[] p; p = NULL;}}


//-----------------------------------------------------------------
//Asset IO class
//
//This class only handles asset file I/O in byte form. It doesn't
//interpret the actual content it reads or writes.
//-----------------------------------------------------------------

class AssetIOHandler
{
	protected:
		BYTE*       m_pbBuffer;
		DWORD       m_dwBufferSize;//in byte, of course

	public:
		//Constructor/Destructor
		AssetIOHandler();
		~AssetIOHandler();

		//General class functions
		BOOL LoadAsset(LPCWSTR lpcwFileName);

		//Accessors
		const BYTE*   GetBuffer() { return m_pbBuffer; }
		void          SetBuffer(BYTE* pbNewBuffer) { SAFE_DELETEARRAY(m_pbBuffer); m_pbBuffer = pbNewBuffer; }//test this!!!!!!!!!!!!
		DWORD         GetBufferSize() { return m_dwBufferSize; }
		void          SetBufferSize(DWORD dwNewSize) { m_dwBufferSize = dwNewSize; }
};