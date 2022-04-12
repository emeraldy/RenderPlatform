#pragma once

#include "GameApp.h"
#include "GameEngine.h"
namespace TestGameApp
{
    struct GameVertexFormat
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 colour;
    };

    class TestGame : public Emerald::GameApp
    {
        public:
            TestGame();
            ~TestGame() {};

            bool GameInitialise(HINSTANCE hInstance, Emerald::GameApp* pGA);
            void GameStart();
            void GameEnd();
            void GameActivate();
            void GameDeactivate();
            void GamePaint();
            void GameCycle();
        private:
            WCHAR* m_pGameTitle;
            bool m_useD3D11;

            //-----------------------------------------------------------------
            //Indices generated from various engine components for the game
            //-----------------------------------------------------------------
            int RM_GLSLEffectPassThru;
            int RM_MeshTriangle;
            int RM_MeshQuad;
            int OR_GLSLEffectPassThru;
            int OR_VAOTriangle;
            int OR_VAOQuad;
            //-----------------------------------------------------------------
            //mesh data storage for the game
            //-----------------------------------------------------------------
            float triangle[9]; // Data to render triangle (3 vertices, each has 3 floats)
            float quad[12]; // Data to render quad using triangle strips (4 vertices, each has 3 floats)
            float triangleColour[9];
            float quadColour[12];

            bool SetupD3D11Rendering();
            int  CreateGLSLEffectResource_PassThru();
            int  SetupGLSLEffectProgram_PassThru(int RM_Index);
            int  LoadMesh();
    };
}