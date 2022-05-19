//-----------------------------------------------------------------
// OpenGL Renderer Class Header
// C++ Header - OpenGLRenderer.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <Strsafe.h>
#include "glm/glm.hpp"
#include "pch.h"

namespace Emerald
{
    //-----------------------------------------------------------------
    //data structure definitions
    //-----------------------------------------------------------------
    struct GLSLEffectProgram//basically the openglrenderer equivalent of the one in resource manager
    {
        int          shaderCount;
        int          currentShaderIndex;//remember the next available shader slot in this program
        LPWSTR       pEffectName;
        GLuint*      pShaderIDs;
        GLuint       programID;
        LPWSTR*      ppAttribNames;//vertex attribute names declared in shaders, ordered by their attribute index
        int          attribCount;
    };

    struct VertexArrayObject
    {
        GLuint  ID;
        LPWSTR  pModelName;
        GLuint* pVBOs;//important!!!!: use them in the order of corresponding attribute indices
        int     attribCount;
    };

    //-----------------------------------------------------------------
    // OpenGL Renderer Class
    //-----------------------------------------------------------------

    class OpenGLRenderer
    {
        protected:
            HGLRC              m_renderContext;
            HDC                m_hDC;
            BOOL               m_isWindowed;

            //effect program repository
            GLSLEffectProgram* m_pEffectPrograms;
            int                m_effectProgramCount;
            int                m_currentEffectProgramIndex;

            //VAO repository
            VertexArrayObject* m_pVertexArrayObjects;
            int                m_vertexArrayObjectCount;
            int                m_currentVertexArrayObjectIndex;

            void DeleteEffectPrograms();
            void DeleteVertexArrayObjects();

        public:
            //Constructor/Destructor
            OpenGLRenderer();
            ~OpenGLRenderer();

            //General class functions
            BOOL       Initialise(HWND hWindow, HDC hdc, HINSTANCE hInstance, int majorVersion, int minorVersion, int profile);
            int        NewEffectProgram(int shaderCount, LPWSTR pEffectName, int attribCount, LPWSTR* ppAttribs);//grab a new GLSLEffectProgram object from the renderer's repository
            int        NewVertexArrayObject(int attribCount, LPWSTR pModelName);//grab a new VertexArrayObject from the renderer's repository
            BOOL       CreateShader(int effectProgramIndex, LPCWSTR pShaderSource, GLuint shaderType);//create a glsl shader object
            BOOL       CreateShaderProgram(int effectProgramIndex);//create glsl shader program object
            void       CreateVBO(int vaoIndex, int attribIndex, float* pData, int sizeInByte);//only transfer a vertex attribute array data to video memeory
            void       CreateVAO(int vaoIndex);//by using information in the VertexArrayObject
            void       RenderScene();

            //Accessors
            HDC         GetHDC() { return m_hDC; }
            void        SetHDC(HDC hdc) { m_hDC = hdc; }
            BOOL        GetIsWindowed() { return m_isWindowed; }
            void        SetIsWindowed(BOOL isWindowed) { m_isWindowed = isWindowed; }
    };
}