//-----------------------------------------------------------------
// Game Engine Class
// C++ Source - GameEngine.cpp
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "GameEngine.h"

//-----------------------------------------------------------------
// Global parameters (some system setting variables will later be 
//replaced by reading from e.g., ini files)
//-----------------------------------------------------------------

BOOL ISWINDOWED = TRUE;
BOOL USED3D9 = FALSE;
int OPENGLMAJORVERSION = 4;
int OPENGLMINORVERSION = 0;
GameEngine* GameEngine::m_pGameEngine = NULL;


//-----------------------------------------------------------------
// Windows Functions
//-----------------------------------------------------------------
int WINAPI  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    MSG         msg;
    static int  iTickTrigger = 0;
    int         iTickCount;

    if (GameInitialise(hInstance))//Instantiate an engine
    {
        // Initialise the game engine (bring subsystems online)
        if (!GameEngine::GetEngine()->Initialise(iCmdShow))
            return FALSE;

        // Enter the main message loop
        while (TRUE)
        {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                // Process the message
                if (msg.message == WM_QUIT)
                    break;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                // Make sure the game engine isn't sleeping
                if (!GameEngine::GetEngine()->GetSleep())
                {
                    // Check the tick count to see if a game cycle has elapsed
                    iTickCount = GetTickCount();
                    if (iTickCount > iTickTrigger)
                    {
                        iTickTrigger = iTickCount +
                            GameEngine::GetEngine()->GetFrameDelay();
                        GameCycle();
                    }
                }
            }
        }
    }

    // End the game
    GameEnd();

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Route all Windows messages to the game engine
    return GameEngine::GetEngine()->HandleEvent(hWindow, msg, wParam, lParam);
}

//-----------------------------------------------------------------
// GameEngine Constructor(s)/Destructor
//-----------------------------------------------------------------
GameEngine::GameEngine(HINSTANCE hInstance, LPTSTR szWindowClass,
    LPTSTR szTitle, WORD wIcon, WORD wSmallIcon, int iWidth, int iHeight)
{
    // Set the member variables for the game engine
    m_pGameEngine = this;
    m_hInstance = hInstance;
    m_hWindow = NULL;
    m_szWindowClass = new WCHAR[32];
    m_szTitle = new WCHAR[32];
    unsigned int uiStringSize;
    if (StringCbLength(szWindowClass, 32 * sizeof(WCHAR), &uiStringSize) == S_OK)//test incoming string length
    {
        if (StringCbCopy(m_szWindowClass, 32 * sizeof(WCHAR), szWindowClass) != S_OK)
            m_szWindowClass = L"default class name";
    }
    else
        m_szWindowClass = L"default class name";

    if (StringCbLength(szTitle, 32 * sizeof(WCHAR), &uiStringSize) == S_OK)
    {
        if (StringCbCopy(m_szTitle, 32 * sizeof(WCHAR), szTitle) != S_OK)
            m_szTitle = L"default title";
    }
    else
        m_szTitle = L"default title";

    m_wIcon = wIcon;
    m_wSmallIcon = wSmallIcon;
    m_clientWidth = iWidth;
    m_clientHeight = iHeight;
    m_iFrameDelay = 50;   // 20 FPS default
    m_fSleep = TRUE;
    m_fWindowed = ISWINDOWED;

    m_pD3D11Renderer = NULL;
    m_pOpenGLRenderer = NULL;

    m_pResourceManager = NULL;
}

GameEngine::~GameEngine()
{
    SAFE_DELETEARRAY(m_szWindowClass);
    SAFE_DELETEARRAY(m_szTitle);
    SAFE_DELETE(m_pD3D9Renderer);

    if (!USED3D9 && !ISWINDOWED)//opengl full screen
    {
        ChangeDisplaySettings(NULL, 0);//set back to the desktop
    }
    SAFE_DELETE(m_pOpenGLRenderer);

    SAFE_DELETE(m_pResourceManager);
}

//-----------------------------------------------------------------
// Game Engine General Methods
//-----------------------------------------------------------------
BOOL GameEngine::Initialise(int iCmdShow)
{
    WNDCLASSEX    wndclass;

    // Create the window class for the main window
    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;//owndc is because of opengl (private dc)
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = m_hInstance;
    wndclass.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(GetIcon()));
    wndclass.hIconSm = LoadIcon(m_hInstance, MAKEINTRESOURCE(GetSmallIcon()));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;//(HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = m_szWindowClass;

    // Register the window class
    if (!RegisterClassEx(&wndclass))
        return FALSE;

    //opengl full screen
    DWORD dwWindowStyle;
    if (!USED3D9 && !ISWINDOWED)
    {
        DEVMODE dmScreenSettings;                               // Device Mode
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // Makes Sure Memory's Cleared
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);     // Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth = m_clientWidth;           // Selected Screen Width
        dmScreenSettings.dmPelsHeight = m_clientHeight;         // Selected Screen Height
        dmScreenSettings.dmBitsPerPel = 32;                     // Selected Bits Per Pixel (Must be the same as set in the pixel format attrib list
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            MessageBox(NULL, TEXT("Sorry, can\'t play in full screen. Continue with window mode."), TEXT("ERROR"), MB_OK | MB_ICONSTOP);
            dwWindowStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
        }
        else
        {
            dwWindowStyle = WS_POPUP;
        }
    }
    else//D3D9 fullscreen also comes here but these styles take no effects
    {
        dwWindowStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
    }

    // Calculate the window size and position based upon the game size
    int iWindowWidth = m_clientWidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
        iWindowHeight = m_clientHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
        GetSystemMetrics(SM_CYCAPTION);
    if (wndclass.lpszMenuName != NULL)
        iWindowHeight += GetSystemMetrics(SM_CYMENU);
    int iXWindowPos = (GetSystemMetrics(SM_CXSCREEN) - iWindowWidth) / 2,
        iYWindowPos = (GetSystemMetrics(SM_CYSCREEN) - iWindowHeight) / 2;

    // Create the window
    m_hWindow = CreateWindow(m_szWindowClass, m_szTitle, dwWindowStyle, iXWindowPos, iYWindowPos,
        iWindowWidth, iWindowHeight, NULL, NULL, m_hInstance, NULL);
    if (!m_hWindow)
        return FALSE;

    //Initiate the renderer
    if (USED3D9)
    {
        m_pD3D11Renderer = new D3D11Renderer;
        if (m_pD3D11Renderer == NULL)
        {
            MessageBox(NULL, TEXT("error in initiating D3D9Renderer"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
        }
        if (!(m_pD3D11Renderer->Initialise(m_hWindow, m_clientWidth, m_clientHeight)))
            return FALSE;
    }
    else
    {
        m_pOpenGLRenderer = new OpenGLRenderer;
        m_pOpenGLRenderer->SetHDC(GetDC(m_hWindow));
        if (!(m_pOpenGLRenderer->Initialise(m_hWindow, m_pOpenGLRenderer->GetHDC(),
            m_hInstance, OPENGLMAJORVERSION, OPENGLMINORVERSION, WGL_CONTEXT_CORE_PROFILE_BIT_ARB)))
            return FALSE;
    }

    //Initialise the resource manager
    m_pResourceManager = new ResourceManager;

    // Set the game window and start the game
    SetWindow(m_hWindow);
    GameStart(m_hWindow);

    // Show and update the window
    ShowWindow(m_hWindow, iCmdShow);
    UpdateWindow(m_hWindow);

    return TRUE;
}

LRESULT GameEngine::HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Route Windows messages to game engine member functions
    switch (msg)
    {
        case WM_ACTIVATE:
            // Activate/deactivate the game and update the Sleep status
            if (wParam != WA_INACTIVE)
            {
                GameActivate(hWindow);
                SetSleep(FALSE);
            }
            else
            {
                GameDeactivate(hWindow);
                SetSleep(TRUE);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_MBUTTONUP://simulate the situation where player select the quit game menu item in game.
            PostMessage(hWindow, WM_CLOSE, NULL, NULL);
            return 0;

        case WM_KEYUP:
            if (wParam == VK_ESCAPE)
                PostMessage(hWindow, WM_CLOSE, NULL, NULL);
            return 0;

        case WM_SIZE:
            m_clientWidth = LOWORD(lParam);
            m_clientHeight = HIWORD(lParam);

            if (wParam == SIZE_MINIMIZED)
            {
                mAppPaused = true;
                mMinimized = true;
                mMaximized = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                mAppPaused = false;
                mMinimized = false;
                mMaximized = true;
                m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
            }
            else if (wParam == SIZE_RESTORED)
            {
                // Restoring from minimized state?
                if (mMinimized)
                {
                    mAppPaused = false;
                    mMinimized = false;
                    m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
                }

                // Restoring from maximized state?
                else if (mMaximized)
                {
                    mAppPaused = false;
                    mMaximized = false;
                    m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
                }
                else if (mResizing)
                {
                    //wait for user done dragging the resize handler
                }
                else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                {
                    m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
                }
            }
            return 0;
    }
    return DefWindowProc(hWindow, msg, wParam, lParam);
}
