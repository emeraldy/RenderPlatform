//-----------------------------------------------------------------
// OpenGL Renderer Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "OpenGLRenderer.h"

extern BOOL ISWINDOWED;

LRESULT CALLBACK dummyWndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (uiMsg)
    {
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    default:
        return DefWindowProc(hWnd, uiMsg, wParam, lParam); // Default window procedure
    }
    return 0;
}

//-----------------------------------------------------------------
// Class constructor/destructor definitions
//-----------------------------------------------------------------
OpenGLRenderer::OpenGLRenderer()
{
    m_hDC = NULL;
    m_hRenderContext = NULL;
    m_fIsWindowed = NULL;

    m_iEffectProgramCount = 1;
    m_pEffectPrograms = new GLSLEffectProgram[m_iEffectProgramCount];
    for (int i = 0; i < m_iEffectProgramCount; i++)
    {
        m_pEffectPrograms[i].pgluiShaderIDs = NULL;
        m_pEffectPrograms[i].lppwAttribNames = NULL;
    }
    m_iCurrentEffectProgramIndex = 0;

    m_iVertexArrayObjectCount = 2;
    m_pVertexArrayObjects = new VertexArrayObject[m_iVertexArrayObjectCount];
    for (int i = 0; i < m_iVertexArrayObjectCount; i++)
    {
        m_pVertexArrayObjects[i].pgluiVBOs = NULL;
    }
    m_iCurrentVertexArrayObjectIndex = 0;
}

OpenGLRenderer::~OpenGLRenderer()
{
    wglMakeCurrent(NULL, NULL);
    if (m_hRenderContext != NULL)
        wglDeleteContext(m_hRenderContext);

    DeleteEffectPrograms();

    DeleteVertexArrayObjects();
}

//-----------------------------------------------------------------
// Class general function definitions
//-----------------------------------------------------------------
BOOL OpenGLRenderer::Initialise(HWND hWindow, HDC hdc, HINSTANCE hInstance, int iMajorVersion, int iMinorVersion, int iProfile)
{
    //create a dummy window to get two opengl extension function addresses
    //1. registrate dummy class
    WNDCLASSEX dummyWC;

    dummyWC.cbSize = sizeof(dummyWC);
    dummyWC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    dummyWC.lpfnWndProc = dummyWndProc;
    dummyWC.cbClsExtra = 0;
    dummyWC.cbWndExtra = 0;
    dummyWC.hInstance = hInstance;
    dummyWC.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    dummyWC.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    dummyWC.hCursor = LoadCursor(NULL, IDC_ARROW);
    dummyWC.hbrBackground = (HBRUSH)(COLOR_MENUBAR + 1);
    dummyWC.lpszMenuName = NULL;
    dummyWC.lpszClassName = TEXT("DUMMY_CLASS");

    RegisterClassEx(&dummyWC);

    //2. create dummy window
    HWND dummyWindow;

    dummyWindow = CreateWindow(TEXT("DUMMY_CLASS"), TEXT("DUMMY"), WS_POPUPWINDOW |
        WS_CAPTION | WS_MINIMIZEBOX, 0, 0, CW_USEDEFAULT,
        CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (dummyWindow == NULL)
    {
        MessageBox(hWindow, TEXT("dummy window creation failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    //3. fill in dummy pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    //4. choose and set desired pixel format
    HDC dummyDC;

    dummyDC = GetDC(dummyWindow);
    int iPixelFormat = ChoosePixelFormat(dummyDC, &pfd);
    if (iPixelFormat == 0)
    {
        MessageBox(hWindow, TEXT("dummy pixel format choosing failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!SetPixelFormat(dummyDC, iPixelFormat, &pfd))
    {
        MessageBox(hWindow, TEXT("dummy pixel format setting failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    //5. Create dummy render context and make it current
    HGLRC dummyRC;

    dummyRC = wglCreateContext(dummyDC);
    if (dummyRC == NULL)
    {
        MessageBox(hWindow, TEXT("dummy rendering context creation failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    wglMakeCurrent(dummyDC, dummyRC);

    //unsigned char* ver = (unsigned char*)glGetString(GL_VERSION);
    //6. Query extension function addresses
    //adding my_ before the function names is because the function name is already defined in wglew.h
    PFNWGLCHOOSEPIXELFORMATARBPROC my_wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
        wglGetProcAddress("wglChoosePixelFormatARB");

    PFNWGLCREATECONTEXTATTRIBSARBPROC my_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");

    //now create the actual rendering context
    const int iPixelFormatAttribList[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0 // End of attributes list
    };
    int iContextAttribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, iMajorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, iMinorVersion,
        WGL_CONTEXT_PROFILE_MASK_ARB, iProfile,
        0 // End of attributes list
    };

    int iNumFormats;
    my_wglChoosePixelFormatARB(hdc, iPixelFormatAttribList, NULL, 1, &iPixelFormat, (UINT*)&iNumFormats);

    if (!SetPixelFormat(hdc, iPixelFormat, &pfd))
    {
        MessageBox(hWindow, TEXT("actual dc pixel format setting failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    m_hRenderContext = my_wglCreateContextAttribsARB(hdc, 0, iContextAttribs);
    if (m_hRenderContext == NULL)
    {
        MessageBox(hWindow, TEXT("actual rendering context creation failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    //destroy everything dummy
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(dummyRC);
    DestroyWindow(dummyWindow);
    UnregisterClass(TEXT("DUMMY_CLASS"), hInstance);

    //make current the actual rc and initialise glew
    wglMakeCurrent(hdc, m_hRenderContext);
    glewExperimental = TRUE;//a bug in glew makes later call of glGenVertexArrays spawn assess violation error. this is the workaround
    GLenum err = glewInit();
    glGetError();//glewInit has error (which i can't do anything about it). call glGetError just to reset the error flag in case i need to debug my opengl code
    if (err != GLEW_OK)
    {
        MessageBox(hWindow, TEXT("glew initialisation failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    glEnable(GL_DEPTH_TEST);

    //set clear values
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    return TRUE;
}

//grab a new GLSLEffectProgram object from the renderer's repository
int OpenGLRenderer::NewEffectProgram(int iToShaderCount, LPWSTR lpwToEffectName, int iToAttribCount, LPWSTR* lppwAttribs)
{
    if (m_iCurrentEffectProgramIndex > m_iEffectProgramCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new glsl effect program. Maximum allowed number reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    m_pEffectPrograms[m_iCurrentEffectProgramIndex].iCurrentShaderIndex = 0;
    m_pEffectPrograms[m_iCurrentEffectProgramIndex].iShaderCount = iToShaderCount;
    m_pEffectPrograms[m_iCurrentEffectProgramIndex].pgluiShaderIDs = new GLuint[iToShaderCount];
    m_pEffectPrograms[m_iCurrentEffectProgramIndex].lpwEffectName = lpwToEffectName;
    m_pEffectPrograms[m_iCurrentEffectProgramIndex].iAttribCount = iToAttribCount;
    m_pEffectPrograms[m_iCurrentEffectProgramIndex].lppwAttribNames = new LPWSTR[iToAttribCount];
    for (int i = 0; i < iToAttribCount; i++)
        m_pEffectPrograms[m_iCurrentEffectProgramIndex].lppwAttribNames[i] = lppwAttribs[i];

    m_iCurrentEffectProgramIndex++;
    return m_iCurrentEffectProgramIndex - 1;
}
//grab a new VertexArrayObject from the renderer's repository
int OpenGLRenderer::NewVertexArrayObject(int iToAttribCount, LPWSTR lpwToModelName)
{
    if (m_iCurrentVertexArrayObjectIndex > m_iVertexArrayObjectCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new vertex array object. Maximum allowed number reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    //request a VAO ID from opengl
    glGenVertexArrays(1, &(m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].gluiID));
    m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].iAttribCount = iToAttribCount;
    m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].lpwModelName = lpwToModelName;
    //request VBO ID's from opengl
    m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].pgluiVBOs =
        new GLuint[m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].iAttribCount];
    glGenBuffers(m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].iAttribCount,
        m_pVertexArrayObjects[m_iCurrentVertexArrayObjectIndex].pgluiVBOs);

    m_iCurrentVertexArrayObjectIndex++;
    return m_iCurrentVertexArrayObjectIndex - 1;
}
void OpenGLRenderer::CreateVBO(int iVAOIndex, int iAttribIndex, float* pfData, int iSizeInByte)
{
    GLuint gluiVBOID = m_pVertexArrayObjects[iVAOIndex].pgluiVBOs[iAttribIndex];
    glBindBuffer(GL_ARRAY_BUFFER, gluiVBOID);
    glBufferData(GL_ARRAY_BUFFER, iSizeInByte, pfData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void OpenGLRenderer::CreateVAO(int iVAOIndex)
{
    glBindVertexArray(m_pVertexArrayObjects[iVAOIndex].gluiID);
    //connect VBO with vertex attributes in the current shader program
    for (int i = 0; i < m_pVertexArrayObjects[iVAOIndex].iAttribCount; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_pVertexArrayObjects[iVAOIndex].pgluiVBOs[i]);
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(i);
    }

    glBindVertexArray(0);
}
void OpenGLRenderer::RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_pEffectPrograms[0].gluiProgramID);

    //glBindVertexArray(m_pVertexArrayObjects[0].gluiID);
    //glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(m_pVertexArrayObjects[1].gluiID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    SwapBuffers(m_hDC);
}
//create glsl shader object
BOOL OpenGLRenderer::CreateShader(int iEffectProgramIndex, LPCWSTR lpcwShaderSource, GLuint gluiShaderType)
{
    //request a shader id from opengl
    int iCurrentShader = m_pEffectPrograms[iEffectProgramIndex].iCurrentShaderIndex;
    GLuint gluiShaderID = glCreateShader(gluiShaderType);
    m_pEffectPrograms[iEffectProgramIndex].pgluiShaderIDs[iCurrentShader] = gluiShaderID;
    if (gluiShaderID == 0)
    {
        //error message generation
        unsigned int uiErrorInfoLength = (unsigned int)lstrlen(m_pEffectPrograms[iEffectProgramIndex].lpwEffectName);
        uiErrorInfoLength = lstrlen(TEXT("GLSL shader effect ")) + uiErrorInfoLength
            + lstrlen(TEXT(" shader id generation failed!!")) + 1;//for the NULL terminator
        LPWSTR lpwErrorInfo = new WCHAR[uiErrorInfoLength];

        if (StringCbCopy(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), TEXT("GLSL shader effect ")) != S_OK)//first part of the error text
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShader String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else if (StringCbCat(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), m_pEffectPrograms[iEffectProgramIndex].lpwEffectName) != S_OK)//second part
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShader String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else if (StringCbCat(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), TEXT(" shader id generation failed!!")) != S_OK)//third part
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShader String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else
            //error message display
            MessageBox(NULL, lpwErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);

        SAFE_DELETEARRAY(lpwErrorInfo);
        return FALSE;
    }
    //move to the next available shader slot in this program
    m_pEffectPrograms[iEffectProgramIndex].iCurrentShaderIndex++;

    //load shader source
    int iShaderCodeCharNum = lstrlen(lpcwShaderSource);
    GLchar* lpstrShaderCode = new GLchar[iShaderCodeCharNum + 1];//add 1 for '\0'
    //glsl only uses 8bit char string
    WideCharToMultiByte(CP_ACP, NULL, lpcwShaderSource, -1, lpstrShaderCode, iShaderCodeCharNum + 1, NULL, NULL);
    glShaderSource(gluiShaderID, 1, (const GLchar**)&lpstrShaderCode, NULL);
    SAFE_DELETEARRAY(lpstrShaderCode);

    //compile the shader
    glCompileShader(gluiShaderID);
    GLint gliCompilingResult;
    glGetShaderiv(gluiShaderID, GL_COMPILE_STATUS, &gliCompilingResult);
    if (GL_FALSE == gliCompilingResult)
    {
#ifdef _DEBUG//print the compiling errors if in debug build
        //output string: "shader effect name vertex shader compilation failed!" 
        OutputDebugString(m_pEffectPrograms[iEffectProgramIndex].lpwEffectName);
        if (gluiShaderType == GL_VERTEX_SHADER)
            OutputDebugString(L" vertex shader compilation failed!\n");
        else if (gluiShaderType == GL_FRAGMENT_SHADER)
            OutputDebugString(L" fragment shader compilation failed!\n");
        GLint gliLogCharNum;
        glGetShaderiv(gluiShaderID, GL_INFO_LOG_LENGTH, &gliLogCharNum);//'\0' is included
        if (gliLogCharNum > 0)
        {
            char* lpstrLog = new char[gliLogCharNum];
            GLsizei glsizeReturnedCharNum;//'\0' is not included
            glGetShaderInfoLog(gluiShaderID, gliLogCharNum, &glsizeReturnedCharNum, lpstrLog);
            LPWSTR lpwLog = new WCHAR[glsizeReturnedCharNum + 1];//add 1 for '\0'
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpstrLog, -1, lpwLog, glsizeReturnedCharNum + 1);
            OutputDebugString(L"Shader log: \n");
            OutputDebugString(lpwLog);
            OutputDebugString(L"\n");
            SAFE_DELETEARRAY(lpstrLog);
            SAFE_DELETEARRAY(lpwLog);
        }
#else
        MessageBox(NULL, TEXT("OpenGL shader compilation failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
#endif

        return FALSE;
    }

    return TRUE;
}
//create a glsl shader program object using a given GLSLEffect struct
//once a program is created, its ID will be store in the same struct
BOOL OpenGLRenderer::CreateShaderProgram(int iEffectProgramIndex)
{
    //request a program id from opengl
    GLuint gluiProgramID = glCreateProgram();
    m_pEffectPrograms[iEffectProgramIndex].gluiProgramID = gluiProgramID;
    if (gluiProgramID == 0)
    {
        //error message generation
        unsigned int uiErrorInfoLength = (unsigned int)lstrlen(m_pEffectPrograms[iEffectProgramIndex].lpwEffectName);
        uiErrorInfoLength = lstrlen(TEXT("GLSL shader effect ")) + uiErrorInfoLength
            + lstrlen(TEXT(" program id generation failed!!")) + 1;//for the NULL terminator
        LPWSTR lpwErrorInfo = new WCHAR[uiErrorInfoLength];

        if (StringCbCopy(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), TEXT("GLSL shader effect ")) != S_OK)//first part of the error text
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShaderProgram String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else if (StringCbCat(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), m_pEffectPrograms[iEffectProgramIndex].lpwEffectName) != S_OK)//second part
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShaderProgram String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else if (StringCbCat(lpwErrorInfo, uiErrorInfoLength * sizeof(WCHAR), TEXT(" program id generation failed!!")) != S_OK)//third part
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShaderProgram String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        else
            //error message display
            MessageBox(NULL, lpwErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);

        SAFE_DELETEARRAY(lpwErrorInfo);
        return FALSE;
    }

    //attach all shaders
    for (int i = 0; i < m_pEffectPrograms[iEffectProgramIndex].iShaderCount; i++)
        glAttachShader(gluiProgramID, m_pEffectPrograms[iEffectProgramIndex].pgluiShaderIDs[i]);

    //bind all the attributes with user-defined indices
    for (int i = 0; i < m_pEffectPrograms[iEffectProgramIndex].iAttribCount; i++)
    {
        //convert attribute names to 8bit char strings
        int iNameCharNum = lstrlen(m_pEffectPrograms[iEffectProgramIndex].lppwAttribNames[i]);
        GLchar* lpstrName = new GLchar[iNameCharNum + 1];//add 1 for '\0'
        WideCharToMultiByte(CP_ACP, NULL, m_pEffectPrograms[iEffectProgramIndex].lppwAttribNames[i],
            -1, lpstrName, iNameCharNum + 1, NULL, NULL);
        glBindAttribLocation(gluiProgramID, i, lpstrName);//that's why order in the lppwAttribNames is very important!!!!
        SAFE_DELETEARRAY(lpstrName);
    }

    //link the program
    glLinkProgram(gluiProgramID);
    GLint gliStatus;
    glGetProgramiv(gluiProgramID, GL_LINK_STATUS, &gliStatus);
    if (GL_FALSE == gliStatus)
    {
#ifdef _DEBUG//print the link errors if in debug build
        OutputDebugString(m_pEffectPrograms[iEffectProgramIndex].lpwEffectName);
        OutputDebugString(L" program failed to link!\n");
        GLint gliLogCharNum;
        glGetProgramiv(gluiProgramID, GL_INFO_LOG_LENGTH, &gliLogCharNum);//'\0' is included
        if (gliLogCharNum > 0)
        {
            char* lpstrLog = new char[gliLogCharNum];
            GLsizei glsizeReturnedCharNum;//'\0' is not included
            glGetProgramInfoLog(gluiProgramID, gliLogCharNum, &glsizeReturnedCharNum, lpstrLog);
            LPWSTR lpwLog = new WCHAR[glsizeReturnedCharNum + 1];//add 1 for '\0'
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpstrLog, -1, lpwLog, glsizeReturnedCharNum + 1);
            OutputDebugString(L"Program log: \n");
            OutputDebugString(lpwLog);
            OutputDebugString(L"\n");
            SAFE_DELETEARRAY(lpstrLog);
            SAFE_DELETEARRAY(lpwLog);
        }
#else
        MessageBox(NULL, TEXT("OpenGL shader program compilation failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
#endif

        return FALSE;
    }

    return TRUE;
}

//due to the limitation of the current data structure used, this is only a helper function called by destructor
void OpenGLRenderer::DeleteEffectPrograms()
{
    for (int i = 0; i < m_iEffectProgramCount; i++)
    {
        if (m_pEffectPrograms[i].pgluiShaderIDs != NULL)//only if this GLSLEffectProgram array element is used
        {
            //delete shaders
            for (int j = 0; j < m_pEffectPrograms[i].iShaderCount; j++)
            {
                glDetachShader(m_pEffectPrograms[i].gluiProgramID,
                    m_pEffectPrograms[i].pgluiShaderIDs[j]);
                glDeleteShader(m_pEffectPrograms[i].pgluiShaderIDs[j]);
            }
            SAFE_DELETEARRAY(m_pEffectPrograms[i].pgluiShaderIDs);

            //delete program
            glDeleteProgram(m_pEffectPrograms[i].gluiProgramID);

            SAFE_DELETEARRAY(m_pEffectPrograms[i].lppwAttribNames);
        }
    }

    SAFE_DELETEARRAY(m_pEffectPrograms);
}

//due to the limitation of the current data structure used, this is only a helper function called by destructor
void OpenGLRenderer::DeleteVertexArrayObjects()
{
    for (int i = 0; i < m_iVertexArrayObjectCount; i++)
    {
        if (m_pVertexArrayObjects[i].pgluiVBOs != NULL)
        {
            glDeleteBuffers(m_pVertexArrayObjects[i].iAttribCount, m_pVertexArrayObjects[i].pgluiVBOs);
            SAFE_DELETEARRAY(m_pVertexArrayObjects[i].pgluiVBOs);
            glDeleteVertexArrays(1, &(m_pVertexArrayObjects[i].gluiID));
        }
    }

    SAFE_DELETEARRAY(m_pVertexArrayObjects);
}
