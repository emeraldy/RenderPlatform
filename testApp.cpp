#include"testApp.h"

using namespace DirectX;
extern BOOL USED3D11;

BOOL SetupD3D11Rendering(HWND hWindow)
{
    ID3D11Device* pDevice = engine_ptr->GetD3D11Renderer()->GetDevice();
    if (pDevice == nullptr)
    {
        MessageBox(0, L"D3D11 Device Does Not Exist", 0, 0);
        return FALSE;
    }

    //vertex data
    GameVertexFormat triangleMesh[] =
    {
        {XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
        {XMFLOAT3( 0.0f,  0.5f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
        {XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)}
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
    HRESULT hr = engine_ptr->GetResourceManager()->GenerateHLSLShaderResource();
    if (FAILED(hr))
    {
        MessageBox(0, L"Load HLSL Shaders Failed", 0, 0);
        return FALSE;
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
    ID3D10Blob* pShaderCode = engine_ptr->GetResourceManager()->GetHLSLShader("SampleVertexShader");
    if (pShaderCode == nullptr)
    {
        MessageBox(0, L"Sample Vertex Shader Does Not Exist", 0, 0);
        return FALSE;
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
        return FALSE;
    }

    //create vertex shader object
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
    pDevice->CreateVertexShader(pShaderCode->GetBufferPointer(),
        pShaderCode->GetBufferSize(), NULL, pVertexShader.GetAddressOf());

    //create pixel shader object
    pShaderCode = engine_ptr->GetResourceManager()->GetHLSLShader("SamplePixelShader");
    if (pShaderCode == nullptr)
    {
        MessageBox(0, L"Sample Pixel Shader Does Not Exist", 0, 0);
        return FALSE;
    }
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    pDevice->CreatePixelShader(pShaderCode->GetBufferPointer(),
        pShaderCode->GetBufferSize(), NULL, pPixelShader.GetAddressOf());

    //prep IA
    ID3D11DeviceContext* pContext = engine_ptr->GetD3D11Renderer()->GetDeviceContext();

    ID3D11Buffer* pVertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{};
    UINT strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{};
    UINT offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{};

    pVertexBuffers[0] = pVertexBuffer.Get();
    strides[0] = sizeof(GameVertexFormat);
    pContext->IASetVertexBuffers(0, 1, pVertexBuffers, strides, offsets);

    pContext->IASetInputLayout(pInputLayout.Get());

    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

    // Bind the render target view and depth/stencil view to the pipeline.
    ID3D11RenderTargetView* pRenderTargetViews[]{ engine_ptr->GetD3D11Renderer()->GetRenderTargetView() };
    pContext->OMSetRenderTargets(ARRAYSIZE(pRenderTargetViews), pRenderTargetViews, engine_ptr->GetD3D11Renderer()->GetDepthStencilView());

    return TRUE;
}
int CreateGLSLEffectResource_PassThru()
{
    //instruct resource manager to create glsl effect resource needed for the game
    int iShaderCount = 2;
    int iAttribCount = 2;

    //1. fill in GLSLShader structure for all the shaders
    GLSLShader* pglsPassThru = new GLSLShader[iShaderCount];
    pglsPassThru[0].gluiType = GL_VERTEX_SHADER;
    pglsPassThru[0].lpwFileName = L"triangle.vert";
    pglsPassThru[1].gluiType = GL_FRAGMENT_SHADER;
    pglsPassThru[1].lpwFileName = L"triangle.frag";

    //2. create the resource
    LPWSTR* lppwAttribs = new LPWSTR[iAttribCount];
    //ATTENTION: vertex attribute names have to be exactly the same as the ones declared in the shaders
    //also the order is crucial!!!
    lppwAttribs[0] = L"VertexPosition";
    lppwAttribs[1] = L"VertexColor";
    int iIndex = engine_ptr->GetResourceManager()->GenerateGLSLEffectResource(pglsPassThru, iShaderCount, L"pass through", lppwAttribs, iAttribCount);
    SAFE_DELETEARRAY(lppwAttribs);

    return iIndex;
}
int SetupGLSLEffectProgram_PassThru(int iRM_Index)
{
    //instruct opengl renderer to create a GLSLEffectProgram object based on its resource manager counterpart 
    int iShaderCount = engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).iGLShaderCount;
    int iAttribCount = engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).iAttribCount;
    //ask for a GLSLEffectProgram object in OpenGLRenderer's repository
    int iOR_Index = engine_ptr->GetOpenGLRenderer()->NewEffectProgram(
        engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).iGLShaderCount,
        engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).lpwEffectName,
        engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).iAttribCount,
        engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).lppwAttribNames);

    if (iOR_Index != -1)
    {
        //create shader objects
        for (int i = 0; i < iShaderCount; i++)
        {
            if (engine_ptr->GetOpenGLRenderer()->CreateShader(iOR_Index,
                engine_ptr->GetResourceManager()->GetTextResource(engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).pGLSLShaders[i].iIndex),
                engine_ptr->GetResourceManager()->GetGLSLEffectResource(iRM_Index).pGLSLShaders[i].gluiType) == FALSE)
                return -1;
        }
    }

    return iOR_Index;
}
int LoadMesh()
{
    // Setup triangle vertices
    fTriangle[0] = -0.4f; fTriangle[1] = 0.1f; fTriangle[2] = 0.0f;
    fTriangle[3] = 0.4f;  fTriangle[4] = 0.1f; fTriangle[5] = 0.0f;
    fTriangle[6] = 0.0f;  fTriangle[7] = 0.7f; fTriangle[8] = 0.0f;

    // Setup triangle color
    fTriangleColour[0] = 1.0f; fTriangleColour[1] = 0.0f; fTriangleColour[2] = 0.0f;
    fTriangleColour[3] = 0.0f; fTriangleColour[4] = 1.0f; fTriangleColour[5] = 0.0f;
    fTriangleColour[6] = 0.0f; fTriangleColour[7] = 0.0f; fTriangleColour[8] = 1.0f;

    // Setup quad vertices
    fQuad[0] = -1.0f; fQuad[1] = 1.0f; fQuad[2] = 0.0f;
    fQuad[3] = -1.0f; fQuad[4] = -1.0f; fQuad[5] = 0.0f;
    fQuad[6] = 1.0f; fQuad[7] = 1.0f; fQuad[8] = 0.0f;
    fQuad[9] = 1.0f; fQuad[10] = -1.0f; fQuad[11] = 0.0f;

    // Setup quad color
    fQuadColour[0] = 1.0f; fQuadColour[1] = 0.0f; fQuadColour[2] = 0.0f;
    fQuadColour[3] = 0.0f; fQuadColour[4] = 1.0f; fQuadColour[8] = 0.0f;
    fQuadColour[6] = 0.0f; fQuadColour[7] = 0.0f; fQuadColour[5] = 1.0f;
    fQuadColour[9] = 1.0f; fQuadColour[10] = 1.0f; fQuadColour[11] = 0.0f;

    //store meshes into the resource manager
    Mesh mTemp;
    //the triangle
    mTemp.iVertexCount = 3;
    mTemp.iVertexSize = 3;
    mTemp.pfPositions = new float[mTemp.iVertexCount * mTemp.iVertexSize];
    for (int i = 0; i < mTemp.iVertexCount * mTemp.iVertexSize; i++)
        mTemp.pfPositions[i] = fTriangle[i];
    mTemp.pfColours = new float[mTemp.iVertexCount * mTemp.iVertexSize];
    for (int i = 0; i < mTemp.iVertexCount * mTemp.iVertexSize; i++)
        mTemp.pfColours[i] = fTriangleColour[i];
    iRM_MeshTriangle = engine_ptr->GetResourceManager()->GenerateMeshResource(&mTemp);
    SAFE_DELETEARRAY(mTemp.pfPositions);
    SAFE_DELETEARRAY(mTemp.pfColours);
    if (iRM_MeshTriangle == -1)
        return iRM_MeshTriangle;

    //the quad
    mTemp.iVertexCount = 4;
    mTemp.iVertexSize = 3;
    mTemp.pfPositions = new float[mTemp.iVertexCount * mTemp.iVertexSize];
    for (int i = 0; i < mTemp.iVertexCount * mTemp.iVertexSize; i++)
        mTemp.pfPositions[i] = fQuad[i];
    mTemp.pfColours = new float[mTemp.iVertexCount * mTemp.iVertexSize];
    for (int i = 0; i < mTemp.iVertexCount * mTemp.iVertexSize; i++)
        mTemp.pfColours[i] = fQuadColour[i];
    iRM_MeshQuad = engine_ptr->GetResourceManager()->GenerateMeshResource(&mTemp);
    SAFE_DELETEARRAY(mTemp.pfPositions);
    SAFE_DELETEARRAY(mTemp.pfColours);
    if (iRM_MeshQuad == -1)
        return iRM_MeshQuad;

    //create VAO for each mesh
    //the triangle
    //first, ask for an instance of VertexArrayObject structure
    iOR_VAOTriangle = engine_ptr->GetOpenGLRenderer()->NewVertexArrayObject(2, L"triangle");
    if (iOR_VAOTriangle == -1)
        return iOR_VAOTriangle;
    //create VBO, one attribute one VBO. MIND THE ATTRIB ORDER!!!!!
    //1.VertexPosition
    engine_ptr->GetOpenGLRenderer()->CreateVBO(iOR_VAOTriangle, 0,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshTriangle).pfPositions,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshTriangle).iVertexCount *
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshTriangle).iVertexSize *
        sizeof(float));
    //2.VertexColour
    engine_ptr->GetOpenGLRenderer()->CreateVBO(iOR_VAOTriangle, 1,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshTriangle).pfColours,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshTriangle).iVertexCount *
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshTriangle).iVertexSize *
        sizeof(float));

    //now VAO
    engine_ptr->GetOpenGLRenderer()->CreateVAO(iOR_VAOTriangle);

    //the quad
    //first, ask for an instance of VertexArrayObject structure
    iOR_VAOQuad = engine_ptr->GetOpenGLRenderer()->NewVertexArrayObject(2, L"quad");
    if (iOR_VAOQuad == -1)
        return iOR_VAOQuad;
    //create VBO, one attribute one VBO. MIND THE ATTRIB ORDER!!!!!
    //1.VertexPosition
    engine_ptr->GetOpenGLRenderer()->CreateVBO(iOR_VAOQuad, 0,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshQuad).pfPositions,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshQuad).iVertexCount *
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshQuad).iVertexSize *
        sizeof(float));
    //2.VertexColour
    engine_ptr->GetOpenGLRenderer()->CreateVBO(iOR_VAOQuad, 1,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshQuad).pfColours,
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshQuad).iVertexCount *
        engine_ptr->GetResourceManager()->GetMeshResource(iRM_MeshQuad).iVertexSize *
        sizeof(float));

    //now VAO
    engine_ptr->GetOpenGLRenderer()->CreateVAO(iOR_VAOQuad);

    return 1;//temporary return value for success
}
BOOL GameInitialise(HINSTANCE hInstance) {
    engine_ptr = new GameEngine(hInstance, TEXT("Game Skeleton"), TEXT("Game Skeleton"), NULL, NULL);
    if (engine_ptr)
        return TRUE;
    else
        return FALSE;
}
void GameStart(HWND hWindow)
{
    if (USED3D11)
    {
        SetupD3D11Rendering(hWindow);
    }
    else//use opengl
    {
        iRM_GLSLEffectPassThru = CreateGLSLEffectResource_PassThru();
        if (iRM_GLSLEffectPassThru == -1)
        {
            PostMessage(engine_ptr->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        iOR_GLSLEffectPassThru = SetupGLSLEffectProgram_PassThru(iRM_GLSLEffectPassThru);
        if (iOR_GLSLEffectPassThru == -1)
        {
            PostMessage(engine_ptr->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        if (engine_ptr->GetOpenGLRenderer()->CreateShaderProgram(iOR_GLSLEffectPassThru) == FALSE)
        {
            PostMessage(engine_ptr->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        if (LoadMesh() == -1)
        {
            PostMessage(engine_ptr->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }

        if (engine_ptr->GetResourceManager()->GenerateTexture2DResource(L"wow.jpg", L"jpeg") == -1)
        {
            PostMessage(engine_ptr->GetWindow(), WM_CLOSE, NULL, NULL);
            return;
        }
    }
}
void GameEnd() 
{ 
    SAFE_DELETE(engine_ptr); 
}
void GameActivate(HWND hWindow) {}
void GameDeactivate(HWND hWindow) {}
HRESULT GamePaint()
{
    if (USED3D11)
    {
        ID3D11DeviceContext* pDeviceContext = engine_ptr->GetD3D11Renderer()->GetDeviceContext();
        const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
        pDeviceContext->ClearRenderTargetView(
            engine_ptr->GetD3D11Renderer()->GetRenderTargetView(),
            teal
        );
        pDeviceContext->ClearDepthStencilView(
            engine_ptr->GetD3D11Renderer()->GetDepthStencilView(),
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f,
            0);
        pDeviceContext->Draw(3, 0);
        engine_ptr->GetD3D11Renderer()->GetSwapChain()->Present(0, 0);

        return S_OK;
    }
    else//use opengl
    {
        engine_ptr->GetOpenGLRenderer()->RenderScene();
        return S_OK;
    }
}
void GameCycle()
{
    GamePaint();
}