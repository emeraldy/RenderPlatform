#pragma once

#include "GameEngine.h"

GameEngine* engine_ptr;

//-----------------------------------------------------------------
//macros and data structure definitions for D3D
//(some may be moved to D3D renderer class later. see opengl 
//renderer.)
//-----------------------------------------------------------------
#define VERTEXNUMBER 3
#define GAMEVERTEXFORMAT (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

struct GameVertexFormat
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 colour;
};

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


