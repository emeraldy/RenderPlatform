#pragma once

#include "GameEngine.h"

GameEngine *engine_ptr;

//-----------------------------------------------------------------
//macros and data structure definitions for D3D
//(some may be moved to D3D renderer class later. see opengl 
//renderer.)
//-----------------------------------------------------------------
#define VERTEXNUMBER 3
#define GAMEVERTEXFORMAT (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

struct GameVertexFormat
{
	FLOAT x,y,z,rhw;
	DWORD colour;
};

//-----------------------------------------------------------------
//Indices generated from various engine components for the game
//-----------------------------------------------------------------
int iRM_GLSLEffectPassThru;
int iRM_MeshTriangle;
int iRM_MeshQuad;
int iOR_GLSLEffectPassThru;
int iOR_VAOTriangle;
int iOR_VAOQuad;
//-----------------------------------------------------------------
//mesh data storage for the game
//-----------------------------------------------------------------
float fTriangle[9]; // Data to render triangle (3 vertices, each has 3 floats)
float fQuad[12]; // Data to render quad using triangle strips (4 vertices, each has 3 floats)
float fTriangleColour[9];
float fQuadColour[12];
