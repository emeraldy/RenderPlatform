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
BOOL USED3D11 = FALSE;
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
    static int  tickTrigger = 0;
    int         tickCount;

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
                    tickCount = GetTickCount();
                    if (tickCount > tickTrigger)
                    {
                        tickTrigger = tickCount +
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
    LPTSTR szTitle, WORD icon, WORD smallIcon, int width, int height)
{
    // Set the member variables for the game engine
    m_pGameEngine = this;
    m_hInstance = hInstance;
    m_hWindow = NULL;
    m_pWindowClass = new WCHAR[32];
    m_pTitle = new WCHAR[32];
    unsigned int stringSize;
    if (StringCbLength(szWindowClass, 32 * sizeof(WCHAR), &stringSize) == S_OK)//test incoming string length
    {
        if (StringCbCopy(m_pWindowClass, 32 * sizeof(WCHAR), szWindowClass) != S_OK)
        {
            m_pWindowClass = L"default class name";
        }
    }
    else
    {
        m_pWindowClass = L"default class name";
    }

    if (StringCbLength(szTitle, 32 * sizeof(WCHAR), &stringSize) == S_OK)
    {
        if (StringCbCopy(m_pTitle, 32 * sizeof(WCHAR), szTitle) != S_OK)
        {
            m_pTitle = L"default title";
        }
    }
    else
    {
        m_pTitle = L"default title";
    }

    m_icon = icon;
    m_smallIcon = smallIcon;
    m_clientWidth = width;
    m_clientHeight = height;
    m_frameDelay = 50;   // 20 FPS default
    m_sleep = TRUE;
    m_windowed = ISWINDOWED;

    m_appPaused = false;
    m_minimised = false;
    m_maxmised = false;
    m_resizing = false;

    m_pD3D11Renderer = nullptr;
    m_pOpenGLRenderer = nullptr;

    m_pResourceManager = nullptr;
}

GameEngine::~GameEngine()
{
    SAFE_DELETEARRAY(m_pWindowClass);
    SAFE_DELETEARRAY(m_pTitle);
    SAFE_DELETE(m_pD3D11Renderer);

    if (!USED3D11 && !ISWINDOWED)//opengl full screen
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
    wndclass.lpszClassName = m_pWindowClass;

    // Register the window class
    if (!RegisterClassEx(&wndclass))
    {
        return FALSE;
    }

    //opengl full screen
    DWORD windowStyle;
    if (!USED3D11 && !ISWINDOWED)
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
            windowStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
        }
        else
        {
            windowStyle = WS_POPUP;
        }
    }
    else//D3D9 fullscreen also comes here but these styles take no effects
    {
        windowStyle = WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX;
    }

    // Calculate the window size and position based upon the game size
    int windowWidth = m_clientWidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
        windowHeight = m_clientHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 +
        GetSystemMetrics(SM_CYCAPTION);
    if (wndclass.lpszMenuName != NULL)
    {
        windowHeight += GetSystemMetrics(SM_CYMENU);
    }
    int windowPosX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
    int windowPosY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;

    // Create the window
    m_hWindow = CreateWindow(m_pWindowClass, m_pTitle, windowStyle, windowPosX, windowPosY,
        windowWidth, windowHeight, NULL, NULL, m_hInstance, NULL);
    if (!m_hWindow)
    {
        return FALSE;
    }

    //Initiate the renderer
    if (USED3D11)
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
        {
            return FALSE;
        }
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
                m_appPaused = true;
                m_minimised = true;
                m_maxmised = false;
            }
            else if (wParam == SIZE_MAXIMIZED)
            {
                m_appPaused = false;
                m_minimised = false;
                m_maxmised = true;
                m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
            }
            else if (wParam == SIZE_RESTORED)
            {
                if (m_minimised)// Restoring from minimised state?
                {
                    m_appPaused = false;
                    m_minimised = false;
                    m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
                }
                else if (m_maxmised)// Restoring from maximised state?
                {
                    m_appPaused = false;
                    m_maxmised = false;
                    m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
                }
                else if (m_resizing)
                {
                    //wait for user done dragging the resize handler
                }
                else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
                {
                    if (m_pD3D11Renderer != nullptr)
                    {
                        m_pD3D11Renderer->OnWindowResize(m_clientWidth, m_clientHeight);
                    }
                }
            }
            return 0;
    }
    return DefWindowProc(hWindow, msg, wParam, lParam);
}
