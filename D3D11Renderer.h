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
#define SAFE_DELETE(p) {if(p) {delete p; p = nullptr;}}
#define SAFE_DELETEARRAY(p) {if(p) {delete[] p; p = nullptr;}}
#define RELEASECOM(x) { if(x) { x->Release(); x = nullptr; } }

class D3D11Renderer
{
    public:
        D3D11Renderer();
        ~D3D11Renderer();

        BOOL Initialise(HWND hWindow, int winWidth, int winHeight);
        void OnWindowResize(int winWidth, int winHeight);
    private:
        ID3D11Device4*        m_pD3D11Device;
        ID3D11DeviceContext4* m_pD3D11ImmediateContext;
        IDXGISwapChain4*      m_pSwapChain;
    
        ID3D11RenderTargetView1* m_pRenderTargetView;
        ID3D11DepthStencilView*  m_pDepthStencilView;
    
        D3D11_VIEWPORT m_screenViewport;
};
