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
#include "jpeglib.h"
#include "png.h"

//-----------------------------------------------------------------
// Macros Definition
//-----------------------------------------------------------------
#define SAFE_DELETE(p) {if(p) {delete p; p = NULL;}}
#define SAFE_DELETEARRAY(p) {if(p) {delete[] p; p = NULL;}}

//-----------------------------------------------------------------
//data structure definitions
//-----------------------------------------------------------------
struct GLSLShader
{
	GLuint  gluiType;
	LPWSTR  lpwFileName;
	int     iIndex;//index into the text resource pool
};

struct GLSLEffect
{
	GLSLShader* pGLSLShaders;
	int         iGLShaderCount;
	LPWSTR      lpwEffectName;
	int         iEffectProgramIndex;//index into the GLSLEffectProgram pool maintained by OpenglRenderer
	LPWSTR*     lppwAttribNames;
	int         iAttribCount;
};

struct Mesh//mesh structure for storing model information, very crude for now
{
	float* pfPositions;
	float* pfColours;
	int    iVertexCount;
	int    iVertexSize;//the dimension of a vertex
};

struct Texture2D
{
	//texture data info
	GLsizei glsizeWidth;
	GLsizei glsizeHeight;
	GLenum  glenumPixelFormatClient;
	GLenum  glenumChannelType;
	GLint   iPixelDataAlignment;//1, 2, 4 or 8  

	GLint   iPixelFormatInternal;

	//texture paramter for openGL
	GLint   iMinFilter;
	GLint   iMagFilter;
	GLint   iWrapS;
	GLint   iWrapT;

	BYTE*   data;
};

//-----------------------------------------------------------------
// Resource Manager Class
//-----------------------------------------------------------------
class ResourceManager
{
	protected:
		AssetIOHandler *m_pAssetIOHandler;

		//text resource pool
		LPWSTR*     m_lppwTextResources;
		int         m_iTextResourceCount;
		int         m_iCurrentTextResourceIndex;

		//GLSL shader effect resource pool
		GLSLEffect* m_pGLSLEffectResources;
		int         m_iGLSLEffectResourceCount;
		int         m_iCurrentGLSLEffectResourceIndex;

		//mesh resource pool
		Mesh*       m_pMeshResources;
		int         m_iMeshResourceCount;
		int         m_iCurrentMeshResourceIndex;

		//texture resource pool
		Texture2D*  m_pTexture2DResources;
		int         m_iTexture2DResourceCount;
		int         m_iCurrentTexture2DResourceIndex;

	private:
		void DestroyGLSLEffectPool();

	public:
		//Constructor/Destructor
		ResourceManager();
		~ResourceManager();

		//General class functions
		int GenerateTextResource(LPCWSTR lpcwFileName, LPCWSTR lpcwType);
		int GenerateGLSLEffectResource(GLSLShader* pglsRequiredShaders, int iRequiredShaderCount, LPWSTR lpwToName, LPWSTR* lppwAttribs, int iAttribNum);
		//the actual signature: int GenerateMeshResource(LPCWSTR lpcwFileName, LPCWSTR lpcwType); but for now:
		int GenerateMeshResource(Mesh* pMeshData);
		int GenerateTexture2DResource(LPCWSTR lpcwFileName, LPCWSTR lpcwType);


		//Accessors
		LPCWSTR			 GetTextResource(int iIndex) { return m_lppwTextResources[iIndex]; }//to add: boundary check
		const int        GetTextResourceCount() {return m_iTextResourceCount; }
		const GLSLEffect GetGLSLEffectResource(int iIndex) { return m_pGLSLEffectResources[iIndex]; }
		const Mesh       GetMeshResource(int iIndex) { return m_pMeshResources[iIndex]; }
};
