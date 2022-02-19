#include"testApp.h"

extern BOOL USED3D9;
void SetupD3D9Rendering(HWND hWindow)
{
    if (!(engine_ptr->GetD3D9Renderer()->EnumDisplayModes(D3DFMT_X8R8G8B8, engine_ptr->GetWindowed())))//full screen, X8=8 bits unknown
    {
        MessageBox(hWindow, TEXT("display modes not found for the given D3DFORMAT"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }

    //set resolution, the highest one supported
    D3DDISPLAYMODE* pDisplayModes = engine_ptr->GetD3D9Renderer()->GetDisplayModes();
    engine_ptr->SetHeight(pDisplayModes[engine_ptr->GetD3D9Renderer()->GetDisplayModeCount() - 1].Height);
    engine_ptr->SetWidth(pDisplayModes[engine_ptr->GetD3D9Renderer()->GetDisplayModeCount() - 1].Width);

    //fill in present parameters
    ZeroMemory(engine_ptr->GetD3D9Renderer()->GetPresentParam(), sizeof(D3DPRESENT_PARAMETERS));
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->BackBufferFormat = pDisplayModes[engine_ptr->GetD3D9Renderer()->GetDisplayModeCount() - 1].Format;
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->BackBufferHeight = engine_ptr->GetHeight();
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->BackBufferWidth = engine_ptr->GetWidth();
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->BackBufferCount = 1;
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->SwapEffect = D3DSWAPEFFECT_DISCARD;
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->hDeviceWindow = hWindow;
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->Windowed = engine_ptr->GetWindowed();
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->EnableAutoDepthStencil = TRUE;
    engine_ptr->GetD3D9Renderer()->GetPresentParam()->AutoDepthStencilFormat = D3DFMT_D16;

    //create the d3d device
    if (!(engine_ptr->GetD3D9Renderer()->CreateD3DDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING)))
    {
        MessageBox(hWindow, TEXT("error in creating D3D device"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }

    //create a vertex buffer
    if (!(engine_ptr->GetD3D9Renderer()->CreateVertexBuffer(VERTEXNUMBER * sizeof(GameVertexFormat), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
        GAMEVERTEXFORMAT, D3DPOOL_DEFAULT)))
    {
        MessageBox(hWindow, TEXT("error in creating vertex buffer"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }

    //fill the vertex buffer,since now it is just a demo,a static quad,the filling code only needs to type once here.
    GameVertexFormat* buffer;
    if (FAILED(engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer()->Lock(0, 0, (VOID**)&buffer, D3DLOCK_DISCARD)))
    {
        MessageBox(hWindow, TEXT("error in filling vertex buffer"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }

    for (long Index = 0; Index < VERTEXNUMBER; Index++)
    {
        float Angle = 2.0f * D3DX_PI * (float)Index / VERTEXNUMBER;

        //mind that d3d uses left hand system so clockwise is front face (back face by default gets culled)
        buffer[0].x = engine_ptr->GetWidth() / 2;
        buffer[0].y = engine_ptr->GetHeight() / 2;
        buffer[0].z = 1;
        buffer[0].colour = 0xffffffff;

        buffer[1].x = 50 + engine_ptr->GetWidth() / 2;
        buffer[1].y = engine_ptr->GetHeight() / 2;
        buffer[1].z = 1;
        buffer[1].colour = 0xffffffff;

        buffer[2].x = engine_ptr->GetWidth() / 2;
        buffer[2].y = engine_ptr->GetHeight() / 2 + 100;
        buffer[2].z = 1;
        buffer[2].colour = 0xffffffff;
    }

    engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer()->Unlock();

    //set the FVF for the device
    engine_ptr->GetD3D9Renderer()->GetD3DDevice()->SetFVF(GAMEVERTEXFORMAT);
    //set the stream source for the device
    engine_ptr->GetD3D9Renderer()->GetD3DDevice()->SetStreamSource(0, engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer(), 0, sizeof(GameVertexFormat));
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
    if (USED3D9)
    {
        SetupD3D9Rendering(hWindow);
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
void GameEnd() { SAFE_DELETE(engine_ptr); }
void GameActivate(HWND hWindow) {}
void GameDeactivate(HWND hWindow) {}
HRESULT GamePaint()
{
    if (USED3D9)
    {
        engine_ptr->GetD3D9Renderer()->GetD3DDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1, 0);
        engine_ptr->GetD3D9Renderer()->GetD3DDevice()->BeginScene();
        engine_ptr->GetD3D9Renderer()->GetD3DDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, VERTEXNUMBER / 3);
        engine_ptr->GetD3D9Renderer()->GetD3DDevice()->EndScene();

        return engine_ptr->GetD3D9Renderer()->GetD3DDevice()->Present(NULL, NULL, NULL, NULL);
    }
    else//use opengl
    {
        engine_ptr->GetOpenGLRenderer()->RenderScene();
        return S_OK;
    }
}
void GameCycle()
{
    if (USED3D9)
    {
        if (D3DERR_DEVICELOST == GamePaint())//device lost detected after rendering
        {
            GamePreDeviceReset();
            engine_ptr->GetD3D9Renderer()->RestoreDevice();
            GamePostDeviceReset();
        }
    }
    else//use opengl
    {
        GamePaint();
    }
}
BOOL GamePreDeviceReset()
{
    if ((engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer()) != NULL)
    {
        engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer()->Release();
        engine_ptr->GetD3D9Renderer()->SetD3DVertexBuffer(NULL);
    }

    return TRUE;
}
BOOL GamePostDeviceReset()
{
    //create a vertex buffer
    if (!(engine_ptr->GetD3D9Renderer()->CreateVertexBuffer(VERTEXNUMBER * sizeof(GameVertexFormat), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
        GAMEVERTEXFORMAT, D3DPOOL_DEFAULT)))
    {
        MessageBox(engine_ptr->GetWindow(), TEXT("error in creating vertex buffer"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }
    //fill the vertex buffer,since now it is just a demo,a static quad,the filling code only needs to type once here.
    GameVertexFormat* buffer;
    if (FAILED(engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer()->Lock(0, 0, (VOID**)&buffer, D3DLOCK_DISCARD)))
    {
        MessageBox(engine_ptr->GetWindow(), TEXT("error in filling vertex buffer"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        exit(0);
    }

    buffer[0].x = 100; buffer[0].y = 200;
    buffer[0].z = 1; buffer[0].rhw = 1;

    buffer[0].colour = 0xffffff00;//32 bits all 1's,white!D3DCOLOR_ARGB(0,255,0,0)

    buffer[1].x = 150; buffer[1].y = 100;
    buffer[1].z = 1; buffer[1].rhw = 1;

    buffer[1].colour = 0xffffff00;//32 bits all 1's,white!

    buffer[2].x = 100; buffer[2].y = 100;
    buffer[2].z = 1; buffer[2].rhw = 1;

    buffer[2].colour = 0xffffff00;//32 bits all 1's,white!

    engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer()->Unlock();
    //set the FVF for the device
    engine_ptr->GetD3D9Renderer()->GetD3DDevice()->SetFVF(GAMEVERTEXFORMAT);
    //set the stream source for the device
    engine_ptr->GetD3D9Renderer()->GetD3DDevice()->SetStreamSource(0, engine_ptr->GetD3D9Renderer()->GetD3DVertexBuffer(), 0, sizeof(GameVertexFormat));

    return TRUE;
}