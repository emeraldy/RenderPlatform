//-----------------------------------------------------------------
// Game engine class
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
#include "pch.h"
#include "GameApp.h"



//-----------------------------------------------------------------
// Game Engine Class
//-----------------------------------------------------------------
namespace Emerald
{
    class GameEngine
    {
    protected:
        HINSTANCE              m_hInstance;
        HWND                   m_hWindow;
        WCHAR*                 m_pWindowClass;
        WCHAR*                 m_pTitle;
        WORD                   m_icon, m_smallIcon;
        int                    m_clientWidth, m_clientHeight;
        int                    m_frameDelay;
        BOOL                   m_sleep;
        BOOL                   m_windowed;
        GameApp*               m_pGameApp;

        //window size states
        bool                   m_appPaused;
        bool                   m_minimised;
        bool                   m_maxmised;
        bool                   m_resizing;

        //subsystems
        D3D11Renderer*         m_pD3D11Renderer;
        OpenGLRenderer*        m_pOpenGLRenderer;

        ResourceManager*       m_pResourceManager;

    public:
        // Constructor(s)/Destructor
        GameEngine(HINSTANCE hInstance, GameApp* pGameApp, LPTSTR szWindowClass, LPTSTR szTitle,
            WORD icon, WORD smallIcon, int width = 800, int height = 600);
        virtual ~GameEngine();

        // General Methods
        BOOL                Initialise(int iCmdShow);
        LRESULT             HandleEvent(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam);

        // Accessor Methods
        HINSTANCE           GetInstance() { return m_hInstance; };
        HWND                GetWindow() { return m_hWindow; };
        void                SetWindow(HWND hWindow) { m_hWindow = hWindow; };
        LPTSTR              GetTitle() { return m_pTitle; };
        WORD                GetIcon() { return m_icon; };
        WORD                GetSmallIcon() { return m_smallIcon; };
        int                 GetClientWidth() { return m_clientWidth; };
        void                SetClientWidth(int width) { m_clientWidth = width; }
        int                 GetClientHeight() { return m_clientHeight; };
        void                SetClientHeight(int height) { m_clientHeight = height; }
        int                 GetFrameDelay() { return m_frameDelay; };
        void                SetFrameRate(int frameRate) { m_frameDelay = 1000 / frameRate; };
        BOOL                GetSleep() { return m_sleep; };
        void                SetSleep(BOOL isSleep) { m_sleep = isSleep; };
        BOOL                GetWindowed() { return m_windowed; }
        void                SetGameApp(GameApp* pGameApp) { m_pGameApp = pGameApp; }
        D3D11Renderer*      GetD3D11Renderer() { return m_pD3D11Renderer; }
        OpenGLRenderer*     GetOpenGLRenderer() { return m_pOpenGLRenderer; }
        ResourceManager*    GetResourceManager() { return m_pResourceManager; }
    };
}
