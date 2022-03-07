//-----------------------------------------------------------------
// D3D9 renderer class
// C++ Source - D3D9Renderer.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "D3D9Renderer.h"

//-----------------------------------------------------------------
// Class function definitions
//-----------------------------------------------------------------
D3D9Renderer::D3D9Renderer()
{
  m_pD3D = NULL;
  m_pD3DDisplayMode = NULL;
  m_uiDisplayModeCount = 0;
  m_pD3DPresentParam = NULL;
  m_pD3DDevice = NULL;
  m_pD3DVertexBuffer = NULL;
}

D3D9Renderer::~D3D9Renderer()
{
	if(m_pD3DDisplayMode != NULL)
	{
		if(m_pD3DPresentParam->Windowed)
		{
			SAFE_DELETE(m_pD3DDisplayMode);
		}
		else
		{
			SAFE_DELETEARRAY(m_pD3DDisplayMode);//full screen application has a series of display modes.
		}
	}

	SAFE_DELETE(m_pD3DPresentParam);

	if(m_pD3DDevice != NULL)
		m_pD3DDevice->Release();

	if(m_pD3DVertexBuffer != NULL)
		m_pD3DVertexBuffer->Release();

	if(m_pD3D != NULL)
		m_pD3D->Release();
}

BOOL D3D9Renderer::Initialise(HWND hWindow)
{
	  //create a D3D object
	  m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	  if(m_pD3D == NULL)
	  {
		  MessageBox(hWindow,TEXT("error in creating D3D object"),TEXT("ERROR"),MB_OK | MB_ICONEXCLAMATION);
		  return FALSE;
	  }

	  //create a present paramter object
	  m_pD3DPresentParam = new D3DPRESENT_PARAMETERS;
	  if(m_pD3DPresentParam == NULL)
	  {
		  MessageBox(hWindow,TEXT("error in creating D3D present param object"),TEXT("ERROR"),MB_OK | MB_ICONEXCLAMATION);
		  return FALSE;
	  }

	  return TRUE;
}

BOOL D3D9Renderer::EnumDisplayModes(D3DFORMAT format, BOOL windowed)
{
	if(windowed)
	{
		//for window mode, you can only use the display mode the current desktop uses.
		m_pD3DDisplayMode = new D3DDISPLAYMODE;
		m_uiDisplayModeCount = 1;
		if(m_pD3DDisplayMode == NULL)
			return FALSE;
		if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, m_pD3DDisplayMode)))//get the current display mode
			return FALSE;
	}
	else
	{
		//for fullscreen mode, you specify a surface format and then query d3d if it is supported. 
		//if yes, you can ask for all the resolution/refresh rate supported and then pick one for display mode.
		m_uiDisplayModeCount = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,format);
		if(m_uiDisplayModeCount == 0)
			return FALSE;//something is wrong in getting the total number of the display modes
		m_pD3DDisplayMode = new D3DDISPLAYMODE[m_uiDisplayModeCount];
		if(m_pD3DDisplayMode == NULL)
			return FALSE;

		for(UINT i =0;i < m_uiDisplayModeCount;i++)
		{
			if((m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,format,i,m_pD3DDisplayMode+i)) != D3D_OK)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL D3D9Renderer::CreateD3DDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags)
{
	if(FAILED(m_pD3D->CreateDevice(Adapter,DeviceType, hFocusWindow, 
		BehaviorFlags,m_pD3DPresentParam, &m_pD3DDevice)))
		return FALSE;

	return TRUE;
}


BOOL D3D9Renderer::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool)
{
	if(FAILED(m_pD3DDevice->CreateVertexBuffer(Length,Usage,FVF,Pool,&m_pD3DVertexBuffer,NULL)))
		return FALSE;

	return TRUE;
}

void D3D9Renderer::RestoreDevice()
{
	HRESULT result = m_pD3DDevice->TestCooperativeLevel();

	while(result == D3DERR_DEVICELOST)
	{
		while(result != D3DERR_DEVICENOTRESET)
		{
			Sleep(1000);//give up the cpu for others

			//while waiting,incoming messages should still be taken care of
			MSG Message;
			PeekMessage(&Message,0,0,0,PM_REMOVE);
			TranslateMessage(&Message);
			DispatchMessage(&Message);

			result = m_pD3DDevice->TestCooperativeLevel();
		}
		if(FAILED(m_pD3DDevice->Reset(m_pD3DPresentParam)))
			result = D3DERR_DEVICELOST;
	}
}


