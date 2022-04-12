#pragma once
#include <windows.h>
namespace Emerald
{
    class GameApp
    {
    public:
        virtual bool GameInitialise(HINSTANCE hInstance, GameApp* pGA) = 0;
        virtual void GameStart() = 0;
        virtual void GameEnd() = 0;
        virtual void GameActivate() = 0;
        virtual void GameDeactivate() = 0;
        virtual void GamePaint() = 0;
        virtual void GameCycle() = 0;

        virtual ~GameApp() {};
    };
}