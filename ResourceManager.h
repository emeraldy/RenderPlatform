//-----------------------------------------------------------------
// Resource Manager class
// C++ Header - ResourceManager.h
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "AssetIOHandler.h"
#include <GL/glew.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include "jpeglib.h"
#include "png.h"
#include <map>
#include <string>
#include <Strsafe.h>

namespace Emerald
{
    //-----------------------------------------------------------------
    //data structure definitions
    //-----------------------------------------------------------------
    struct GLSLShader
    {
        GLuint  type;
        LPWSTR  pFileName;
        int     index;//index into the text resource pool
    };

    struct GLSLEffect
    {
        GLSLShader* pGLSLShaders;
        int         glShaderCount;
        LPWSTR      pEffectName;
        int         effectProgramIndex;//index into the GLSLEffectProgram pool maintained by OpenglRenderer
        LPWSTR* ppAttribNames;
        int         attribCount;
    };

    struct Mesh//mesh structure for storing model information, very crude for now
    {
        float* pPositions;
        float* pColours;
        int    vertexCount;
        int    vertexSize;//the dimension of a vertex
    };

    struct Texture2D
    {
        //texture data info
        GLsizei width;
        GLsizei height;
        GLenum  pixelFormatClient;
        GLenum  channelType;
        GLint   pixelDataAlignment;//1, 2, 4 or 8  

        GLint   pixelFormatInternal;

        //texture paramter for openGL
        GLint   minFilter;
        GLint   magFilter;
        GLint   wrapS;
        GLint   wrapT;

        BYTE* pData;
    };

    //-----------------------------------------------------------------
    // Resource Manager Class
    //-----------------------------------------------------------------
    class ResourceManager
    {
    protected:
        AssetIOHandler* m_pAssetIOHandler;

        //text resource pool
        LPWSTR* m_ppTextResources;
        int         m_textResourceCount;
        int         m_currentTextResourceIndex;

        //GLSL shader effect resource pool
        GLSLEffect* m_pGLSLEffectResources;
        int         m_GLSLEffectResourceCount;
        int         m_currentGLSLEffectResourceIndex;

        //D3D11 shader resouce pool
        std::map<std::string, ID3D10Blob*> m_HLSLShaderCodes;

        //mesh resource pool
        Mesh* m_pMeshResources;
        int         m_meshResourceCount;
        int         m_currentMeshResourceIndex;

        //texture resource pool
        Texture2D* m_pTexture2DResources;
        int         m_texture2DResourceCount;
        int         m_currentTexture2DResourceIndex;

    private:
        void DestroyGLSLEffectPool();
        void DestroyHLSLShaderPool();

    public:
        //Constructor/Destructor
        ResourceManager();
        ~ResourceManager();

        //General class functions
        int GenerateTextResource(LPCWSTR pFileName, LPCWSTR pType);
        int GenerateGLSLEffectResource(GLSLShader* pRequiredShaders, int requiredShaderCount, LPWSTR pName, LPWSTR* ppAttribs, int attribNum);
        HRESULT GenerateHLSLShaderResource();
        //the actual signature: int GenerateMeshResource(LPCWSTR fileName, LPCWSTR type); but for now:
        int GenerateMeshResource(Mesh* pMeshData);
        int GenerateTexture2DResource(LPCWSTR pFileName, LPCWSTR pType);


        //Accessors
        LPCWSTR          GetTextResource(int index) { return m_ppTextResources[index]; }//to add: boundary check
        const int        GetTextResourceCount() { return m_textResourceCount; }
        const GLSLEffect GetGLSLEffectResource(int index) { return m_pGLSLEffectResources[index]; }
        const Mesh       GetMeshResource(int index) { return m_pMeshResources[index]; }
        ID3D10Blob* GetHLSLShader(std::string name);
    };
}