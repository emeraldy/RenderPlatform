#include "TestGame.h"

using namespace DirectX;
using namespace Emerald;
using namespace TestGameApp;

extern GameEngine* pGameEngine;
extern BOOL USED3D11;

int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    MSG         msg;
    static int  tickTrigger = 0;
    int         tickCount;
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

    RM_GLSLEffectPassThru = 0;
    RM_MeshTriangle = 0;
    RM_MeshQuad = 0;
    OR_GLSLEffectPassThru = 0;
    OR_VAOTriangle = 0;
    OR_VAOQuad = 0;

    float triangle[9] = {0};
    float quad[12] = {0};
    float triangleColour[9] = {0};
    float quadColour[12] = {0};
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
        {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
        {XMFLOAT3(0.0f,  0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
        {XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
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
    HRESULT hr = pGameEngine->GetResourceManager()->GenerateHLSLShaderResource();
    if (FAILED(hr))
    {
        MessageBox(0, L"Load HLSL Shaders Failed", 0, 0);
        return false;
    }

    //vertex buffer layout description
    D3D11_INPUT_ELEMENT_DESC vbElementDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
          0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
          0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    ID3D10Blob* pShaderCode = pGameEngine->GetResourceManager()->GetHLSLShader("SampleVertexShader");
    if (pShaderCode == nullptr)
    {
        MessageBox(0, L"Sample Vertex Shader Does Not Exist", 0, 0);
        return false;
    }
    hr = pDevice->CreateInputLayout(
        vbElementDesc,
        2,
        pShaderCode->GetBufferPointer(),
        pShaderCode->GetBufferSize(),
        pInputLayout.GetAddressOf());
    if (FAILED(hr))
    {
        MessageBox(0, L"Create Input Layout Failed.", 0, 0);
        return false;
    }

    //create vertex shader object
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
    pDevice->CreateVertexShader(pShaderCode->GetBufferPointer(),
        pShaderCode->GetBufferSize(), NULL, pVertexShader.GetAddressOf());

    //create pixel shader object
    pShaderCode = pGameEngine->GetResourceManager()->GetHLSLShader("SamplePixelShader");
    if (pShaderCode == nullptr)
    {
        MessageBox(0, L"Sample Pixel Shader Does Not Exist", 0, 0);
        return false;
    }
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    pDevice->CreatePixelShader(pShaderCode->GetBufferPointer(),
        pShaderCode->GetBufferSize(), NULL, pPixelShader.GetAddressOf());

    //prep IA
    ID3D11DeviceContext* pContext = pGameEngine->GetD3D11Renderer()->GetDeviceContext();

    ID3D11Buffer* pVertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {nullptr};
    UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {0};
    UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {0};

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

    return true;
}
int TestGame::CreateGLSLEffectResource_PassThru()
{
    //instruct resource manager to create glsl effect resource needed for the game
    int shaderCount = 2;
    int attribCount = 2;

    //1. fill in GLSLShader structure for all the shaders
    GLSLShader* pglsPassThru = new GLSLShader[shaderCount];
    pglsPassThru[0].type = GL_VERTEX_SHADER;
    pglsPassThru[0].pFileName = L"triangle.vert";
    pglsPassThru[1].type = GL_FRAGMENT_SHADER;
    pglsPassThru[1].pFileName = L"triangle.frag";

    //2. create the resource
    LPWSTR* ppAttribs = new LPWSTR[attribCount];
    //ATTENTION: vertex attribute names have to be exactly the same as the ones declared in the shaders
    //also the order is crucial!!!
    ppAttribs[0] = L"VertexPosition";
    ppAttribs[1] = L"VertexColor";
    int index = pGameEngine->GetResourceManager()->GenerateGLSLEffectResource(pglsPassThru, shaderCount, L"pass through", ppAttribs, attribCount);
    SAFE_DELETEARRAY(ppAttribs);

    return index;
}
int TestGame::SetupGLSLEffectProgram_PassThru(int RM_Index)
{
    //instruct opengl renderer to create a GLSLEffectProgram object based on its resource manager counterpart 
    int shaderCount = pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).glShaderCount;
    int attribCount = pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).attribCount;
    //ask for a GLSLEffectProgram object in OpenGLRenderer's repository
    int OR_Index = pGameEngine->GetOpenGLRenderer()->NewEffectProgram(
        pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).glShaderCount,
        pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).pEffectName,
        pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).attribCount,
        pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).ppAttribNames);

    if (OR_Index != -1)
    {
        //create shader objects
        for (int i = 0; i < shaderCount; i++)
        {
            if (pGameEngine->GetOpenGLRenderer()->CreateShader(OR_Index,
                pGameEngine->GetResourceManager()->GetTextResource(pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).pGLSLShaders[i].index),
                pGameEngine->GetResourceManager()->GetGLSLEffectResource(RM_Index).pGLSLShaders[i].type) == false)
            {
                return -1;
            }
        }
    }

    return OR_Index;
}
int TestGame::LoadMesh()
{
    // Setup triangle vertices
    triangle[0] = -0.4f; triangle[1] = 0.1f; triangle[2] = 0.0f;
    triangle[3] = 0.4f;  triangle[4] = 0.1f; triangle[5] = 0.0f;
    triangle[6] = 0.0f;  triangle[7] = 0.7f; triangle[8] = 0.0f;

    // Setup triangle color
    triangleColour[0] = 1.0f; triangleColour[1] = 0.0f; triangleColour[2] = 0.0f;
    triangleColour[3] = 0.0f; triangleColour[4] = 1.0f; triangleColour[5] = 0.0f;
    triangleColour[6] = 0.0f; triangleColour[7] = 0.0f; triangleColour[8] = 1.0f;

    // Setup quad vertices
    quad[0] = -1.0f; quad[1] = 1.0f; quad[2] = 0.0f;
    quad[3] = -1.0f; quad[4] = -1.0f; quad[5] = 0.0f;
    quad[6] = 1.0f; quad[7] = 1.0f; quad[8] = 0.0f;
    quad[9] = 1.0f; quad[10] = -1.0f; quad[11] = 0.0f;

    // Setup quad color
    quadColour[0] = 1.0f; quadColour[1] = 0.0f; quadColour[2] = 0.0f;
    quadColour[3] = 0.0f; quadColour[4] = 1.0f; quadColour[8] = 0.0f;
    quadColour[6] = 0.0f; quadColour[7] = 0.0f; quadColour[5] = 1.0f;
    quadColour[9] = 1.0f; quadColour[10] = 1.0f; quadColour[11] = 0.0f;

    //store meshes into the resource manager
    Mesh mTemp;
    //the triangle
    mTemp.vertexCount = 3;
    mTemp.vertexSize = 3;
    mTemp.pPositions = new float[mTemp.vertexCount * mTemp.vertexSize];
    for (int i = 0; i < mTemp.vertexCount * mTemp.vertexSize; i++)
    {
        mTemp.pPositions[i] = triangle[i];
    }
    mTemp.pColours = new float[mTemp.vertexCount * mTemp.vertexSize];
    for (int i = 0; i < mTemp.vertexCount * mTemp.vertexSize; i++)
    {
        mTemp.pColours[i] = triangleColour[i];
    }
    RM_MeshTriangle = pGameEngine->GetResourceManager()->GenerateMeshResource(&mTemp);
    SAFE_DELETEARRAY(mTemp.pPositions);
    SAFE_DELETEARRAY(mTemp.pColours);
    if (RM_MeshTriangle == -1)
    {
        return RM_MeshTriangle;
    }

    //the quad
    mTemp.vertexCount = 4;
    mTemp.vertexSize = 3;
    mTemp.pPositions = new float[mTemp.vertexCount * mTemp.vertexSize];
    for (int i = 0; i < mTemp.vertexCount * mTemp.vertexSize; i++)
        mTemp.pPositions[i] = quad[i];
    mTemp.pColours = new float[mTemp.vertexCount * mTemp.vertexSize];
    for (int i = 0; i < mTemp.vertexCount * mTemp.vertexSize; i++)
        mTemp.pColours[i] = quadColour[i];
    RM_MeshQuad = pGameEngine->GetResourceManager()->GenerateMeshResource(&mTemp);
    SAFE_DELETEARRAY(mTemp.pPositions);
    SAFE_DELETEARRAY(mTemp.pColours);
    if (RM_MeshQuad == -1)
    {
        return RM_MeshQuad;
    }

    //create VAO for each mesh
    //the triangle
    //first, ask for an instance of VertexArrayObject structure
    OR_VAOTriangle = pGameEngine->GetOpenGLRenderer()->NewVertexArrayObject(2, L"triangle");
    if (OR_VAOTriangle == -1)
    {
        return OR_VAOTriangle;
    }
    //create VBO, one attribute one VBO. MIND THE ATTRIB ORDER!!!!!
    //1.VertexPosition
    pGameEngine->GetOpenGLRenderer()->CreateVBO(OR_VAOTriangle, 0,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshTriangle).pPositions,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshTriangle).vertexCount *
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshTriangle).vertexSize *
        sizeof(float));
    //2.VertexColour
    pGameEngine->GetOpenGLRenderer()->CreateVBO(OR_VAOTriangle, 1,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshTriangle).pColours,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshTriangle).vertexCount *
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshTriangle).vertexSize *
        sizeof(float));

    //now VAO
    pGameEngine->GetOpenGLRenderer()->CreateVAO(OR_VAOTriangle);

    //the quad
    //first, ask for an instance of VertexArrayObject structure
    OR_VAOQuad = pGameEngine->GetOpenGLRenderer()->NewVertexArrayObject(2, L"quad");
    if (OR_VAOQuad == -1)
    {
        return OR_VAOQuad;
    }
    //create VBO, one attribute one VBO. MIND THE ATTRIB ORDER!!!!!
    //1.VertexPosition
    pGameEngine->GetOpenGLRenderer()->CreateVBO(OR_VAOQuad, 0,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshQuad).pPositions,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshQuad).vertexCount *
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshQuad).vertexSize *
        sizeof(float));
    //2.VertexColour
    pGameEngine->GetOpenGLRenderer()->CreateVBO(OR_VAOQuad, 1,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshQuad).pColours,
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshQuad).vertexCount *
        pGameEngine->GetResourceManager()->GetMeshResource(RM_MeshQuad).vertexSize *
        sizeof(float));

    //now VAO
    pGameEngine->GetOpenGLRenderer()->CreateVAO(OR_VAOQuad);

    return 1;//temporary return value for success
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
        SetupD3D11Rendering();
    }
    else//use opengl
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

        if (pGameEngine->GetResourceManager()->GenerateTexture2DResource(L"wow.jpg", L"jpeg") == -1)
        {
            PostMessage(pGameEngine->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }
    }
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
    if (m_useD3D11)
    {
        ID3D11DeviceContext* pDeviceContext = pGameEngine->GetD3D11Renderer()->GetDeviceContext();
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
    }
    else//use opengl
    {
        pGameEngine->GetOpenGLRenderer()->RenderScene();
    }
}

void TestGame::GameCycle()
{
    GamePaint();
}