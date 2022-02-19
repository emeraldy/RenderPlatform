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
#include "glm/glm.hpp"
#include <Strsafe.h>

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------
#define SAFE_DELETE(p) {if(p) {delete p; p = NULL;}}
#define SAFE_DELETEARRAY(p) {if(p) {delete[] p; p = NULL;}}

//-----------------------------------------------------------------
//data structure definitions
//-----------------------------------------------------------------
struct GLSLEffectProgram//basically the openglrenderer equivalent of the one in resource manager
{
    int          iShaderCount;
    int          iCurrentShaderIndex;//remember the next available shader slot in this program
    LPWSTR       lpwEffectName;
    GLuint*      pgluiShaderIDs;
    GLuint       gluiProgramID;
    LPWSTR*      lppwAttribNames;//vertex attribute names declared in shaders, ordered by their attribute index
    int          iAttribCount;
};

struct VertexArrayObject
{
    GLuint  gluiID;
    LPWSTR  lpwModelName;
    GLuint* pgluiVBOs;//important!!!!: use them in the order of corresponding attribute indices
    int     iAttribCount;
};

//-----------------------------------------------------------------
// OpenGL Renderer Class
//-----------------------------------------------------------------

class OpenGLRenderer
{
    protected:
        HGLRC              m_hRenderContext;
        HDC                m_hDC;
        BOOL               m_fIsWindowed;

        //effect program repository
        GLSLEffectProgram* m_pEffectPrograms;
        int                m_iEffectProgramCount;
        int                m_iCurrentEffectProgramIndex;

        //VAO repository
        VertexArrayObject* m_pVertexArrayObjects;
        int                m_iVertexArrayObjectCount;
        int                m_iCurrentVertexArrayObjectIndex;

        void DeleteEffectPrograms();
        void DeleteVertexArrayObjects();

    public:
        //Constructor/Destructor
        OpenGLRenderer();
        ~OpenGLRenderer();

        //General class functions
        BOOL       Initialise(HWND hWindow, HDC hdc, HINSTANCE hInstance, int iMajorVersion, int iMinorVersion, int iProfile);
        int        NewEffectProgram(int iToShaderCount, LPWSTR lpwToEffectName, int iToAttribCount, LPWSTR* lppwAttribs);//grab a new GLSLEffectProgram object from the renderer's repository
        int        NewVertexArrayObject(int iToAttribCount, LPWSTR lpwToModelName);//grab a new VertexArrayObject from the renderer's repository
        BOOL       CreateShader(int iEffectProgramIndex, LPCWSTR lpcwShaderSource, GLuint gluiShaderType);//create a glsl shader object
        BOOL       CreateShaderProgram(int iEffectProgramIndex);//create glsl shader program object
        void       CreateVBO(int iVAOIndex, int iAttribIndex, float* pfData, int iSizeInByte);//only transfer a vertex attribute array data to video memeory
        void       CreateVAO(int iVAOIndex);//by using information in the VertexArrayObject
        void       RenderScene();

        //Accessors
        HDC         GetHDC() { return m_hDC; }
        void        SetHDC(HDC hdc) { m_hDC = hdc; }
        BOOL        GetIsWindowed() { return m_fIsWindowed; }
        void        SetIsWindowed(BOOL fIsWindowed) { m_fIsWindowed = fIsWindowed; }
};