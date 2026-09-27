#pragma once

#include "StandardIncludes.h"
#include "GameApp.h"
#include "GameEngine.h"
#include "Vector3.h"

namespace TestGameApp
{
    class TestGame : public Emerald::GameApp
    {
        public:
            TestGame();
            virtual ~TestGame() {};

            bool GameInitialise(HINSTANCE hInstance, Emerald::GameApp* pGA);
            void GameStart();
            void GameEnd();
            void GameActivate();
            void GameDeactivate();
            void GamePaint();
            void GameCycle();
        private:
            struct GameVertexFormat
            {
                Emerald::Vector3 position;
                Emerald::Vector3 colour;
            };
            Emerald::Error error;
            WCHAR* m_pGameTitle;
            bool m_useD3D11;

            bool SetupD3D11Rendering();
    };
}