//-----------------------------------------------------------------
// D3D11 Renderer Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_4.h>
#include <cassert>

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------
#define SAFE_DELETE(p) { if(p) { delete p; p = nullptr; } }
#define SAFE_DELETEARRAY(p) { if(p) { delete[] p; p = nullptr; } }
#define RELEASECOM(x) { if(x) { x->Release(); x = nullptr; } }

class D3D11Renderer
{
    public:
        D3D11Renderer();
        ~D3D11Renderer();

        BOOL                     Initialise(HWND hWindow, int winWidth, int winHeight);
        void                     OnWindowResize(int winWidth, int winHeight);
        ID3D11Device*           GetDevice() { return m_pD3D11Device; };
        ID3D11DeviceContext*    GetDeviceContext() { return m_pD3D11ImmediateContext; }
        IDXGISwapChain*         GetSwapChain() { return m_pSwapChain; }
        ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }
        ID3D11DepthStencilView*  GetDepthStencilView() { return m_pDepthStencilView; }

    private:
        ID3D11Device*        m_pD3D11Device;
        ID3D11DeviceContext* m_pD3D11ImmediateContext;
        IDXGISwapChain*      m_pSwapChain;
    
        ID3D11RenderTargetView*  m_pRenderTargetView;
        ID3D11DepthStencilView*  m_pDepthStencilView;
    
        D3D11_VIEWPORT m_screenViewport;
};
