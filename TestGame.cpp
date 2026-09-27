#include "TestGame.h"
#include "Quaternion.h"
#include "Matrix4.h"
#include "WinMaterialImporter.h"
#include "HLSLShaderProgramImporter.h"
#include "MaterialManager.h"
#include "RenderingPass.h"
#include "HLSLShaderProgramManager.h"


using namespace Emerald;
using namespace TestGameApp;

extern GameEngine* pGameEngine;
extern BOOL USED3D11;

int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    MSG         msg;
    static int  tickTrigger = 0;
    DWORD       tickCount = 0;
    TestGame* pTestGame = new TestGame();

    if (pTestGame == nullptr)
    {
        return FALSE;
    }

    if (pTestGame->GameInitialise(hInstance, pTestGame))//Instantiate an engine
    {
        // Initialise the game engine (bring subsystems online)
        if (!pGameEngine->Initialise(iCmdShow))
        {
            return FALSE;
        }

        // Enter the main message loop
        while (TRUE)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                // Process the message
                if (msg.message == WM_QUIT)
                {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                // Make sure the game engine isn't sleeping
                if (!pGameEngine->GetSleep())
                {
                    // Check the tick count to see if a game cycle has elapsed
                    tickCount = GetTickCount();
                    if (tickCount > tickTrigger)
                    {
                        tickTrigger = tickCount + pGameEngine->GetFrameDelay();
                        pTestGame->GameCycle();
                    }
                }
            }
        }
    }

    // End the game
    pTestGame->GameEnd();
    SAFE_DELETE(pTestGame);

    return TRUE;
}


TestGame::TestGame()
{
    m_pGameTitle = L"Test";
    m_useD3D11 = USED3D11;
}

bool TestGame::SetupD3D11Rendering()
{
    ID3D11Device* pDevice = pGameEngine->GetD3D11Renderer()->GetDevice();
    if (pDevice == nullptr)
    {
        MessageBox(0, L"D3D11 Device Does Not Exist", 0, 0);
        return false;
    }

    //vertex data
    GameVertexFormat triangleMesh[] =
    {
        {Vector3(-0.5f, -0.5f, 0.5f), Vector3(0.0f, 0.0f, 0.0f)},
        {Vector3(0.0f,  0.5f, 0.5f), Vector3(0.0f, 0.0f, 1.0f)},
        {Vector3(0.5f, -0.5f, 0.5f), Vector3(0.0f, 1.0f, 0.0f)}
    };
    D3D11_SUBRESOURCE_DATA vertexData{};
    vertexData.pSysMem = triangleMesh;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    //vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc{};
    vertexBufferDesc.ByteWidth = sizeof(triangleMesh);
    vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, pVertexBuffer.GetAddressOf());

    //load shaders and compile them
    WinMaterialImporter materialImporter;//TODO: importers may need a central hub to keep them
    MaterialManager::GetInstance().SetImporter(&materialImporter);
    std::wstring materialToLoad = L"..\\assets\\materials\\passthrough.material";
    std::shared_ptr<Resource> pResource = MaterialManager::GetInstance().Load(materialToLoad, error);
    if (error)
    {
        MessageBox(0, error.GetErrorText().c_str(), 0, 0);
        return false;
    }
    std::shared_ptr<Material> pPassthroughMaterial = std::dynamic_pointer_cast<Material>(pResource);
    if (!pPassthroughMaterial)
    {
        error += L"Obtaining material " + materialToLoad + L" failed";
        return false;
    }
    HLSLShaderProgramImporter hlslImporter;
    HLSLShaderProgramManager::GetInstance().SetImporter(&hlslImporter);
    const std::vector<RenderingPass>& allPasses = pPassthroughMaterial->GetAllPasses();
    for (const RenderingPass& rp : allPasses)
    {
        std::shared_ptr<ShaderProgram> pShaderProgram = rp.GetShaderProgram(error);
        if (error)
        {
            return false;
        }
        std::shared_ptr<HLSLShaderProgram> pHLSLShaderProgram = std::dynamic_pointer_cast<HLSLShaderProgram>(pShaderProgram);
        if (!pHLSLShaderProgram)
        {
            error += L"Obtaining shader program of pass " + rp.GetName() + L" failed.";
            return false;
        }
        Microsoft::WRL::ComPtr<ID3DBlob> pShaderCode = pHLSLShaderProgram->GetCompiledVertexShader();

        //vertex buffer layout description
        D3D11_INPUT_ELEMENT_DESC vbElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
              0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
              0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
        HRESULT hr = pDevice->CreateInputLayout(
            vbElementDesc,
            2,
            pShaderCode->GetBufferPointer(),
            pShaderCode->GetBufferSize(),
            pInputLayout.GetAddressOf());
        if (FAILED(hr))
        {
            MessageBox(0, L"Create Input Layout Failed.", 0, 0);
            error += L"Create Input Layout Failed.";
            return false;
        }

        //create vertex shader object
        Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
        pDevice->CreateVertexShader(pShaderCode->GetBufferPointer(),
            pShaderCode->GetBufferSize(), NULL, pVertexShader.GetAddressOf());

        //create pixel shader object
        pShaderCode = pHLSLShaderProgram->GetCompiledPixelShader();
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
        pDevice->CreatePixelShader(pShaderCode->GetBufferPointer(),
            pShaderCode->GetBufferSize(), NULL, pPixelShader.GetAddressOf());

        //prep IA
        ID3D11DeviceContext* pContext = pGameEngine->GetD3D11Renderer()->GetDeviceContext();

        ID3D11Buffer* pVertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };
        UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
        UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };

        pVertexBuffers[0] = pVertexBuffer.Get();
        strides[0] = sizeof(GameVertexFormat);
        pContext->IASetVertexBuffers(0, 1, pVertexBuffers, strides, offsets);

        pContext->IASetInputLayout(pInputLayout.Get());

        pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

        pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

        // Bind the render target view and depth/stencil view to the pipeline.
        ID3D11RenderTargetView* pRenderTargetViews[] = { pGameEngine->GetD3D11Renderer()->GetRenderTargetView() };
        pContext->OMSetRenderTargets(ARRAYSIZE(pRenderTargetViews), pRenderTargetViews, pGameEngine->GetD3D11Renderer()->GetDepthStencilView());
    }

    return true;
}

bool TestGame::GameInitialise(HINSTANCE hInstance, GameApp* pGA)
{
    pGameEngine = new GameEngine(hInstance, pGA, m_pGameTitle, m_pGameTitle, NULL, NULL);
    if (pGameEngine)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TestGame::GameStart()
{
    if (m_useD3D11)
    {
        if(!SetupD3D11Rendering())
        {
            MessageBox(0, error.GetErrorText().c_str(), 0, 0);
        }
    }
    /*else//use opengl
    {
        RM_GLSLEffectPassThru = CreateGLSLEffectResource_PassThru();
        if (RM_GLSLEffectPassThru == -1)
        {
            PostMessage(pGameEngine->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        OR_GLSLEffectPassThru = SetupGLSLEffectProgram_PassThru(RM_GLSLEffectPassThru);
        if (OR_GLSLEffectPassThru == -1)
        {
            PostMessage(pGameEngine->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        if (pGameEngine->GetOpenGLRenderer()->CreateShaderProgram(OR_GLSLEffectPassThru) == FALSE)
        {
            PostMessage(pGameEngine->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        if (LoadMesh() == -1)
        {
            PostMessage(pGameEngine->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }
    }*/
}

void TestGame::GameEnd()
{
    SAFE_DELETE(pGameEngine);
}

void TestGame::GameActivate()
{

}
void TestGame::GameDeactivate()
{

}

void TestGame::GamePaint()
{
    Error gamePaintErr;

    if (m_useD3D11)
    {
        static Degree angle;
        ID3D11Device* pDevice = pGameEngine->GetD3D11Renderer()->GetDevice();
        ID3D11DeviceContext* pDeviceContext = pGameEngine->GetD3D11Renderer()->GetDeviceContext();

        //constant buffer for transformation matrix
        Quaternion revolve(gamePaintErr, angle, Vector3(0, 0, 1.0f));
        Matrix3 revolveMat3 = revolve.ToMatrix3(gamePaintErr);
        Matrix4 revolveMat4(revolveMat3);
        revolveMat4 = revolveMat4.Transpose();
        if (gamePaintErr)
        {
            MessageBox(0, gamePaintErr.GetErrorText().c_str(), 0, 0);
        }

        D3D11_SUBRESOURCE_DATA vertexCBData{};
        vertexCBData.pSysMem = &revolveMat4;
        vertexCBData.SysMemPitch = 0;
        vertexCBData.SysMemSlicePitch = 0;

        D3D11_BUFFER_DESC vertexCBDesc{};
        vertexCBDesc.ByteWidth = sizeof(revolveMat4);
        vertexCBDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        vertexCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexConstantBuffer;
        pDevice->CreateBuffer(&vertexCBDesc, &vertexCBData, pVertexConstantBuffer.GetAddressOf());

        ID3D11Buffer* pVertexConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = { nullptr };
        pVertexConstantBuffers[0] = pVertexConstantBuffer.Get();

        pDeviceContext->VSSetConstantBuffers(0, 1, pVertexConstantBuffers);

        const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
        pDeviceContext->ClearRenderTargetView(
            pGameEngine->GetD3D11Renderer()->GetRenderTargetView(),
            teal
        );
        pDeviceContext->ClearDepthStencilView(
            pGameEngine->GetD3D11Renderer()->GetDepthStencilView(),
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f,
            0);
        pDeviceContext->Draw(3, 0);
        pGameEngine->GetD3D11Renderer()->GetSwapChain()->Present(0, 0);
        
        angle += 5.0f;
        if (angle > 360.0f)
        {
            angle = 0.0f;
        }
    }
    /*else//use opengl
    {
        pGameEngine->GetOpenGLRenderer()->RenderScene();
    }*/
}

void TestGame::GameCycle()
{
    GamePaint();
}