//-----------------------------------------------------------------
// D3D11 Renderer Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "D3D11Renderer.h"

using namespace Microsoft::WRL;

D3D11Renderer::D3D11Renderer()
{
    m_pD3D11Device = nullptr;
    m_pD3D11ImmediateContext = nullptr;
    m_pSwapChain = nullptr;

    m_pRenderTargetView = nullptr;
    m_pDepthStencilView = nullptr;
}

D3D11Renderer::~D3D11Renderer()
{
    RELEASECOM(m_pD3D11ImmediateContext);
    RELEASECOM(m_pD3D11Device);
}

BOOL D3D11Renderer::Initialise(HWND hWindow, int winWidth, int winHeight)
{
    // Create the device and device context.

    UINT createDeviceFlags = 0;
    #if defined(DEBUG) || defined(_DEBUG)  
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Device* pDevice;
    ID3D11DeviceContext* pContext;
    HRESULT hr = D3D11CreateDevice(
            0,                 // default adapter
            D3D_DRIVER_TYPE_HARDWARE,
            0,                 // no software device
            createDeviceFlags,
            0, 0,              // default feature level array
            D3D11_SDK_VERSION,
            &pDevice,
            &featureLevel,
            &pContext);

    if (FAILED(hr))
    {
        MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
        return false;
    }

    if (featureLevel != D3D_FEATURE_LEVEL_11_0 || featureLevel != D3D_FEATURE_LEVEL_11_1)
    {
        MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
        return false;
    }

    //have to convert base pointer to derived (ID3D11Device -> ID3D11Device4, etc.)
    m_pD3D11Device = dynamic_cast<ID3D11Device4*>(pDevice);
    m_pD3D11ImmediateContext = dynamic_cast<ID3D11DeviceContext4*>(pContext);

    // Check 4X MSAA quality support for our back buffer format.
    UINT msaaQuality;
    m_pD3D11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality);
    assert(msaaQuality > 0);

    // Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = winWidth;
    swapChainDesc.BufferDesc.Height = winHeight;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    if (msaaQuality > 1)
    {
        swapChainDesc.SampleDesc.Count = 4;
        swapChainDesc.SampleDesc.Quality = msaaQuality - 1;
    }
    else
    {
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hWindow;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    // Create the swap chain via COM hierarchy
    IDXGIDevice4* dxgiDevice = nullptr;
    hr = m_pD3D11Device->QueryInterface(__uuidof(IDXGIDevice4), (void**)&dxgiDevice);
    if (FAILED(hr))
    {
        MessageBox(0, L"Obtain DXGIDevice Failed.", 0, 0);
        return false;
    }

    IDXGIAdapter3* dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter3), (void**)&dxgiAdapter);
    if (FAILED(hr))
    {
        MessageBox(0, L"Obtain DXGIAdapter Failed.", 0, 0);
        goto failExit;
    }

    IDXGIFactory5* dxgiFactory = nullptr;
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory5), (void**)&dxgiFactory);
    if (FAILED(hr))
    {
        MessageBox(0, L"Obtain DXGIFactory Failed.", 0, 0);
        goto failExit;
    }

    IDXGISwapChain* pSwapChain;
    hr = dxgiFactory->CreateSwapChain(m_pD3D11Device, &swapChainDesc, &pSwapChain);
    if (FAILED(hr))
    {
        MessageBox(0, L"Obtain DXGISwapChain Failed.", 0, 0);
        goto failExit;
    }
    m_pSwapChain = dynamic_cast<IDXGISwapChain4*>(pSwapChain);

    RELEASECOM(dxgiDevice);
    RELEASECOM(dxgiAdapter);
    RELEASECOM(dxgiFactory);

    OnWindowResize(winWidth, winHeight);

    return true;

failExit:
    RELEASECOM(dxgiDevice);
    RELEASECOM(dxgiAdapter);
    RELEASECOM(dxgiFactory);
    return false;
}

void D3D11Renderer::OnWindowResize(int winWidth, int winHeight)
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_pD3D11ImmediateContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    RELEASECOM(m_pRenderTargetView);
    RELEASECOM(m_pDepthStencilView);
    m_pD3D11ImmediateContext->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

    // Resize the swap chain and recreate the render target view.
    HRESULT hr = m_pSwapChain->ResizeBuffers(1, winWidth, winHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
    {
        MessageBox(0, L"SwapChain Resize Failed.", 0, 0);
        //continue with old size...
    }

    // Create the render target buffer and view.
    ID3D11Texture2D1* backBuffer;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D1), reinterpret_cast<void**>(&backBuffer));
    if (FAILED(hr))
    {
        MessageBox(0, L"Obtain Render Target Failed.", 0, 0);
    }

    hr = m_pD3D11Device->CreateRenderTargetView1(backBuffer, 0, &m_pRenderTargetView);
    if (FAILED(hr))
    {
        MessageBox(0, L"Obtain Render Target View Failed.", 0, 0);
    }
    RELEASECOM(backBuffer);

    // Create the depth/stencil buffer and view.
    ID3D11Texture2D1* depthStencilBuffer;
    D3D11_TEXTURE2D_DESC1 depthStencilDesc = {};
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    
    m_pSwapChain->GetDesc(&swapChainDesc);
    depthStencilDesc.Width = winWidth;
    depthStencilDesc.Height = winHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = swapChainDesc.SampleDesc.Count;
    depthStencilDesc.SampleDesc.Quality = swapChainDesc.SampleDesc.Quality;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;

    hr = m_pD3D11Device->CreateTexture2D1(&depthStencilDesc, 0, &depthStencilBuffer);
    if (FAILED(hr))
    {
        MessageBox(0, L"Create Depth Stencil Failed.", 0, 0);
    }
    hr = m_pD3D11Device->CreateDepthStencilView(depthStencilBuffer, 0, &m_pDepthStencilView);
    if (FAILED(hr))
    {
        MessageBox(0, L"Create Depth Stencil View Failed.", 0, 0);
    }
    RELEASECOM(depthStencilBuffer);

    // Bind the render target view and depth/stencil view to the pipeline.
    m_pD3D11ImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


    // Set the viewport transform.
    m_screenViewport.TopLeftX = 0;
    m_screenViewport.TopLeftY = 0;
    m_screenViewport.Width = static_cast<float>(winWidth);
    m_screenViewport.Height = static_cast<float>(winHeight);
    m_screenViewport.MinDepth = 0.0f;
    m_screenViewport.MaxDepth = 1.0f;

    m_pD3D11ImmediateContext->RSSetViewports(1, &m_screenViewport);
}