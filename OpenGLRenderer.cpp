//-----------------------------------------------------------------
// OpenGL Renderer Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "OpenGLRenderer.h"

extern BOOL ISWINDOWED;

LRESULT CALLBACK dummyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (msg)
    {
        case WM_PAINT:
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam); // Default window procedure
    }
    return 0;
}

//-----------------------------------------------------------------
// Class constructor/destructor definitions
//-----------------------------------------------------------------
OpenGLRenderer::OpenGLRenderer()
{
    m_hDC = NULL;
    m_renderContext = NULL;
    m_isWindowed = NULL;

    m_effectProgramCount = 1;
    m_pEffectPrograms = new GLSLEffectProgram[m_effectProgramCount];
    for (int i = 0; i < m_effectProgramCount; i++)
    {
        m_pEffectPrograms[i].pShaderIDs = NULL;
        m_pEffectPrograms[i].ppAttribNames = NULL;
    }
    m_currentEffectProgramIndex = 0;

    m_vertexArrayObjectCount = 2;
    m_pVertexArrayObjects = new VertexArrayObject[m_vertexArrayObjectCount];
    for (int i = 0; i < m_vertexArrayObjectCount; i++)
    {
        m_pVertexArrayObjects[i].pVBOs = NULL;
    }
    m_currentVertexArrayObjectIndex = 0;
}

OpenGLRenderer::~OpenGLRenderer()
{
    wglMakeCurrent(NULL, NULL);
    if (m_renderContext != NULL)
    {
        wglDeleteContext(m_renderContext);
    }

    DeleteEffectPrograms();

    DeleteVertexArrayObjects();
}

//-----------------------------------------------------------------
// Class general function definitions
//-----------------------------------------------------------------
BOOL OpenGLRenderer::Initialise(HWND hWindow, HDC hdc, HINSTANCE hInstance, int majorVersion, int minorVersion, int profile)
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
    int pixelFormat = ChoosePixelFormat(dummyDC, &pfd);
    if (pixelFormat == 0)
    {
        MessageBox(hWindow, TEXT("dummy pixel format choosing failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    if (!SetPixelFormat(dummyDC, pixelFormat, &pfd))
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
    const int pixelFormatAttribList[] =
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
    int contextAttribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
        WGL_CONTEXT_PROFILE_MASK_ARB, profile,
        0 // End of attributes list
    };

    int numFormats;
    my_wglChoosePixelFormatARB(hdc, pixelFormatAttribList, NULL, 1, &pixelFormat, (UINT*)&numFormats);

    if (!SetPixelFormat(hdc, pixelFormat, &pfd))
    {
        MessageBox(hWindow, TEXT("actual dc pixel format setting failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    m_renderContext = my_wglCreateContextAttribsARB(hdc, 0, contextAttribs);
    if (m_renderContext == NULL)
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
    wglMakeCurrent(hdc, m_renderContext);
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
int OpenGLRenderer::NewEffectProgram(int shaderCount, LPWSTR pEffectName, int attribCount, LPWSTR* ppAttribs)
{
    if (m_currentEffectProgramIndex > m_effectProgramCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new glsl effect program. Maximum allowed number reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    m_pEffectPrograms[m_currentEffectProgramIndex].currentShaderIndex = 0;
    m_pEffectPrograms[m_currentEffectProgramIndex].shaderCount = shaderCount;
    m_pEffectPrograms[m_currentEffectProgramIndex].pShaderIDs = new GLuint[shaderCount];
    m_pEffectPrograms[m_currentEffectProgramIndex].pEffectName = pEffectName;
    m_pEffectPrograms[m_currentEffectProgramIndex].attribCount = attribCount;
    m_pEffectPrograms[m_currentEffectProgramIndex].ppAttribNames = new LPWSTR[attribCount];
    for (int i = 0; i < attribCount; i++)
    {
        m_pEffectPrograms[m_currentEffectProgramIndex].ppAttribNames[i] = ppAttribs[i];
    }

    m_currentEffectProgramIndex++;
    return m_currentEffectProgramIndex - 1;
}
//grab a new VertexArrayObject from the renderer's repository
int OpenGLRenderer::NewVertexArrayObject(int attribCount, LPWSTR pModelName)
{
    if (m_currentVertexArrayObjectIndex > m_vertexArrayObjectCount - 1)
    {
        MessageBox(NULL, TEXT("Cannot generate new vertex array object. Maximum allowed number reached!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return -1;
    }

    //request a VAO ID from opengl
    glGenVertexArrays(1, &(m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].ID));
    m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].attribCount = attribCount;
    m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].pModelName = pModelName;
    //request VBO ID's from opengl
    m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].pVBOs =
        new GLuint[m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].attribCount];
    glGenBuffers(m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].attribCount,
        m_pVertexArrayObjects[m_currentVertexArrayObjectIndex].pVBOs);

    m_currentVertexArrayObjectIndex++;
    return m_currentVertexArrayObjectIndex - 1;
}
void OpenGLRenderer::CreateVBO(int VAOIndex, int attribIndex, float* pData, int sizeInByte)
{
    GLuint gluiVBOID = m_pVertexArrayObjects[VAOIndex].pVBOs[attribIndex];
    glBindBuffer(GL_ARRAY_BUFFER, gluiVBOID);
    glBufferData(GL_ARRAY_BUFFER, sizeInByte, pData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void OpenGLRenderer::CreateVAO(int VAOIndex)
{
    glBindVertexArray(m_pVertexArrayObjects[VAOIndex].ID);
    //connect VBO with vertex attributes in the current shader program
    for (int i = 0; i < m_pVertexArrayObjects[VAOIndex].attribCount; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_pVertexArrayObjects[VAOIndex].pVBOs[i]);
        glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(i);
    }

    glBindVertexArray(0);
}
void OpenGLRenderer::RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_pEffectPrograms[0].programID);

    glBindVertexArray(m_pVertexArrayObjects[0].ID);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //glBindVertexArray(m_pVertexArrayObjects[1].ID);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    SwapBuffers(m_hDC);
}
//create glsl shader object
BOOL OpenGLRenderer::CreateShader(int effectProgramIndex, LPCWSTR pShaderSource, GLuint shaderType)
{
    //request a shader id from opengl
    int currentShader = m_pEffectPrograms[effectProgramIndex].currentShaderIndex;
    GLuint shaderID = glCreateShader(shaderType);
    m_pEffectPrograms[effectProgramIndex].pShaderIDs[currentShader] = shaderID;
    if (shaderID == 0)
    {
        //error message generation
        unsigned int errorInfoLength = (unsigned int)lstrlen(m_pEffectPrograms[effectProgramIndex].pEffectName);
        errorInfoLength = lstrlen(TEXT("GLSL shader effect ")) + errorInfoLength
            + lstrlen(TEXT(" shader id generation failed!!")) + 1;//for the NULL terminator
        LPWSTR lpwErrorInfo = new WCHAR[errorInfoLength];

        if (StringCbCopy(lpwErrorInfo, errorInfoLength * sizeof(WCHAR), TEXT("GLSL shader effect ")) != S_OK)//first part of the error text
        {
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShader String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else if (StringCbCat(lpwErrorInfo, errorInfoLength * sizeof(WCHAR), m_pEffectPrograms[effectProgramIndex].pEffectName) != S_OK)//second part
        {
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShader String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else if (StringCbCat(lpwErrorInfo, errorInfoLength * sizeof(WCHAR), TEXT(" shader id generation failed!!")) != S_OK)//third part
        {
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShader String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {
            //error message display
            MessageBox(NULL, lpwErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }

        SAFE_DELETEARRAY(lpwErrorInfo);
        return FALSE;
    }
    //move to the next available shader slot in this program
    m_pEffectPrograms[effectProgramIndex].currentShaderIndex++;

    //load shader source
    int shaderCodeCharNum = lstrlen(pShaderSource);
    GLchar* pShaderCode = new GLchar[shaderCodeCharNum + 1];//add 1 for '\0'
    //glsl only uses 8bit char string
    WideCharToMultiByte(CP_ACP, NULL, pShaderSource, -1, pShaderCode, shaderCodeCharNum + 1, NULL, NULL);
    glShaderSource(shaderID, 1, (const GLchar**)&pShaderCode, NULL);
    SAFE_DELETEARRAY(pShaderCode);

    //compile the shader
    glCompileShader(shaderID);
    GLint compilingResult;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilingResult);
    if (GL_FALSE == compilingResult)
    {
#ifdef _DEBUG//print the compiling errors if in debug build
        //output string: "shader effect name vertex shader compilation failed!" 
        OutputDebugString(m_pEffectPrograms[effectProgramIndex].pEffectName);
        if (shaderType == GL_VERTEX_SHADER)
        {
            OutputDebugString(L" vertex shader compilation failed!\n");
        }
        else if (shaderType == GL_FRAGMENT_SHADER)
        {
            OutputDebugString(L" fragment shader compilation failed!\n");
        }
        GLint logCharNum;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logCharNum);//'\0' is included
        if (logCharNum > 0)
        {
            char* lpstrLog = new char[logCharNum];
            GLsizei glsizeReturnedCharNum;//'\0' is not included
            glGetShaderInfoLog(shaderID, logCharNum, &glsizeReturnedCharNum, lpstrLog);
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
BOOL OpenGLRenderer::CreateShaderProgram(int effectProgramIndex)
{
    //request a program id from opengl
    GLuint programID = glCreateProgram();
    m_pEffectPrograms[effectProgramIndex].programID = programID;
    if (programID == 0)
    {
        //error message generation
        unsigned int errorInfoLength = (unsigned int)lstrlen(m_pEffectPrograms[effectProgramIndex].pEffectName);
        errorInfoLength = lstrlen(TEXT("GLSL shader effect ")) + errorInfoLength
            + lstrlen(TEXT(" program id generation failed!!")) + 1;//for the NULL terminator
        LPWSTR pErrorInfo = new WCHAR[errorInfoLength];

        if (StringCbCopy(pErrorInfo, errorInfoLength * sizeof(WCHAR), TEXT("GLSL shader effect ")) != S_OK)//first part of the error text
        {
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShaderProgram String copy failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else if (StringCbCat(pErrorInfo, errorInfoLength * sizeof(WCHAR), m_pEffectPrograms[effectProgramIndex].pEffectName) != S_OK)//second part
        {
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShaderProgram String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else if (StringCbCat(pErrorInfo, errorInfoLength * sizeof(WCHAR), TEXT(" program id generation failed!!")) != S_OK)//third part
        {
            MessageBox(NULL, TEXT("OpenGLRenderer::CreateShaderProgram String cat failed"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {   
            //error message display
            MessageBox(NULL, pErrorInfo, TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        }

        SAFE_DELETEARRAY(pErrorInfo);
        return FALSE;
    }

    //attach all shaders
    for (int i = 0; i < m_pEffectPrograms[effectProgramIndex].shaderCount; i++)
    {
        glAttachShader(programID, m_pEffectPrograms[effectProgramIndex].pShaderIDs[i]);
    }

    //bind all the attributes with user-defined indices
    for (int i = 0; i < m_pEffectPrograms[effectProgramIndex].attribCount; i++)
    {
        //convert attribute names to 8bit char strings
        int iNameCharNum = lstrlen(m_pEffectPrograms[effectProgramIndex].ppAttribNames[i]);
        GLchar* lpstrName = new GLchar[iNameCharNum + 1];//add 1 for '\0'
        WideCharToMultiByte(CP_ACP, NULL, m_pEffectPrograms[effectProgramIndex].ppAttribNames[i],
            -1, lpstrName, iNameCharNum + 1, NULL, NULL);
        glBindAttribLocation(programID, i, lpstrName);//that's why order in the lppwAttribNames is very important!!!!
        SAFE_DELETEARRAY(lpstrName);
    }

    //link the program
    glLinkProgram(programID);
    GLint gliStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &gliStatus);
    if (GL_FALSE == gliStatus)
    {
#ifdef _DEBUG//print the link errors if in debug build
        OutputDebugString(m_pEffectPrograms[effectProgramIndex].pEffectName);
        OutputDebugString(L" program failed to link!\n");
        GLint gliLogCharNum;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &gliLogCharNum);//'\0' is included
        if (gliLogCharNum > 0)
        {
            char* lpstrLog = new char[gliLogCharNum];
            GLsizei glsizeReturnedCharNum;//'\0' is not included
            glGetProgramInfoLog(programID, gliLogCharNum, &glsizeReturnedCharNum, lpstrLog);
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
    for (int i = 0; i < m_effectProgramCount; i++)
    {
        if (m_pEffectPrograms[i].pShaderIDs != NULL)//only if this GLSLEffectProgram array element is used
        {
            //delete shaders
            for (int j = 0; j < m_pEffectPrograms[i].shaderCount; j++)
            {
                glDetachShader(m_pEffectPrograms[i].programID,
                    m_pEffectPrograms[i].pShaderIDs[j]);
                glDeleteShader(m_pEffectPrograms[i].pShaderIDs[j]);
            }
            SAFE_DELETEARRAY(m_pEffectPrograms[i].pShaderIDs);

            //delete program
            glDeleteProgram(m_pEffectPrograms[i].programID);

            SAFE_DELETEARRAY(m_pEffectPrograms[i].ppAttribNames);
        }
    }

    SAFE_DELETEARRAY(m_pEffectPrograms);
}

//due to the limitation of the current data structure used, this is only a helper function called by destructor
void OpenGLRenderer::DeleteVertexArrayObjects()
{
    for (int i = 0; i < m_vertexArrayObjectCount; i++)
    {
        if (m_pVertexArrayObjects[i].pVBOs != NULL)
        {
            glDeleteBuffers(m_pVertexArrayObjects[i].attribCount, m_pVertexArrayObjects[i].pVBOs);
            SAFE_DELETEARRAY(m_pVertexArrayObjects[i].pVBOs);
            glDeleteVertexArrays(1, &(m_pVertexArrayObjects[i].ID));
        }
    }

    SAFE_DELETEARRAY(m_pVertexArrayObjects);
}
