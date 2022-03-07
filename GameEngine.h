//-----------------------------------------------------------------
// Game Engine Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <DirectXMath.h>
#include "D3D11Renderer.h"
#include "OpenGLRenderer.h"
#include "ResourceManager.h"
#include <Strsafe.h>

//-----------------------------------------------------------------
// Windows Function Declarations
//-----------------------------------------------------------------
int WINAPI        WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                          PSTR szCmdLine, int iCmdShow);
LRESULT CALLBACK  WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-----------------------------------------------------------------
// Game Engine Function Declarations
//-----------------------------------------------------------------
BOOL GameInitialise(HINSTANCE hInstance);
void GameStart(HWND hWindow);
void GameEnd();
void GameActivate(HWND hWindow);
void GameDeactivate(HWND hWindow);
HRESULT GamePaint();//rendering
void GameCycle();//game control logic

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------



//-----------------------------------------------------------------
// Game Engine Class
//-----------------------------------------------------------------
class GameEngine
{
    protected:
        static GameEngine*     m_pGameEngine;
        HINSTANCE              m_hInstance;
        HWND                   m_hWindow;
        WCHAR* m_szWindowClass;
        WCHAR* m_szTitle;
        WORD                   m_wIcon, m_wSmallIcon;
        int                    m_clientWidth, m_clientHeight;
        int                    m_iFrameDelay;
        BOOL                   m_fSleep;
        BOOL                   m_fWindowed;

        //window size states
        bool                   m_appPaused;
        bool                   m_minimised;
        bool                   m_maxmised;
        bool                   m_resizing;

        //subsystems
        D3D11Renderer*    m_pD3D11Renderer;
        OpenGLRenderer*   m_pOpenGLRenderer;

        ResourceManager*  m_pResourceManager;

    public:
        // Constructor(s)/Destructor
        GameEngine(HINSTANCE hInstance, LPTSTR szWindowClass, LPTSTR szTitle,
            WORD wIcon, WORD wSmallIcon, int iWidth = 800, int iHeight = 600);
        ~GameEngine();

        // General Methods

        BOOL                Initialise(int iCmdShow);
        LRESULT             HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

        // Accessor Methods
        static GameEngine*  GetEngine() { return m_pGameEngine; };
        HINSTANCE           GetInstance() { return m_hInstance; };
        HWND                GetWindow() { return m_hWindow; };
        void                SetWindow(HWND hWindow) { m_hWindow = hWindow; };
        LPTSTR              GetTitle() { return m_szTitle; };
        WORD                GetIcon() { return m_wIcon; };
        WORD                GetSmallIcon() { return m_wSmallIcon; };
        int                 GetClientWidth() { return m_clientWidth; };
        void                SetClientWidth(int width) { m_clientWidth = width; }
        int                 GetClientHeight() { return m_clientHeight; };
        void                SetClientHeight(int height) { m_clientHeight = height; }
        int                 GetFrameDelay() { return m_iFrameDelay; };
        void                SetFrameRate(int iFrameRate) { m_iFrameDelay = 1000 / iFrameRate; };
        BOOL                GetSleep() { return m_fSleep; };
        void                SetSleep(BOOL fSleep) { m_fSleep = fSleep; };
        BOOL                GetWindowed() { return m_fWindowed; }
        D3D11Renderer*      GetD3D11Renderer() { return m_pD3D11Renderer; }
        OpenGLRenderer*     GetOpenGLRenderer() { return m_pOpenGLRenderer; }
        ResourceManager*    GetResourceManager() { return m_pResourceManager; }
};
