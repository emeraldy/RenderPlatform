//-----------------------------------------------------------------
// D3D9 renderer class
// C++ Header - D3D9Renderer.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include "d3d9.h"
#include "d3dx9.h"

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------
#define SAFE_DELETE(p) {if(p) {delete p; p = NULL;}}
#define SAFE_DELETEARRAY(p) {if(p) {delete[] p; p = NULL;}}


//-----------------------------------------------------------------
// D3D9Renderer Class
//-----------------------------------------------------------------

class D3D9Renderer
{
	protected:
		  IDirect3D9*             m_pD3D;
		  D3DDISPLAYMODE*         m_pD3DDisplayMode;//a display mode pointer pointing to all possible display modes
		  UINT                    m_uiDisplayModeCount;
		  D3DPRESENT_PARAMETERS*  m_pD3DPresentParam;
		  IDirect3DDevice9*       m_pD3DDevice;
		  IDirect3DVertexBuffer9* m_pD3DVertexBuffer;

	public:
		//Constructor/Destructor
		D3D9Renderer();
		~D3D9Renderer();

		//General class functions
		BOOL                 Initialise(HWND hWindow);
		BOOL				 EnumDisplayModes(D3DFORMAT format,BOOL windowed);
		BOOL				 CreateD3DDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags);
		BOOL				 CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool);
		void				 RestoreDevice();

		//Accessors
		IDirect3D9*			     GetD3D(){return m_pD3D;}
		D3DDISPLAYMODE*		     GetDisplayModes(){return m_pD3DDisplayMode;}
		UINT                     GetDisplayModeCount(){return m_uiDisplayModeCount;}
		D3DPRESENT_PARAMETERS*   GetPresentParam(){return m_pD3DPresentParam;}
		IDirect3DDevice9*	     GetD3DDevice(){return m_pD3DDevice;}
		IDirect3DVertexBuffer9*  GetD3DVertexBuffer(){return m_pD3DVertexBuffer;}
		void					 SetD3DVertexBuffer(IDirect3DVertexBuffer9* value){m_pD3DVertexBuffer = value;}
};