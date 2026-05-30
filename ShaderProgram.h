//-----------------------------------------------------------------
// Shader Program Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"
#include "Shader.h"

namespace Emerald
{
    class ShaderFactory
    {
    public:
        virtual Shader CreateVertexShader(const std::wstring& programName, Error& err) = 0;
        virtual Shader CreateFragmentShader(const std::wstring& programName, Error& err) = 0;
    };

    class ShaderProgram : public Resource
    {
    public:
        ShaderProgram(Resource::ResourceID id, std::wstring name = L"Default");
        virtual ~ShaderProgram();

        virtual void CreateShaders(Error& err) = 0;
        virtual void PrepareProgram(Error& err) = 0;

    protected:
        ShaderFactory* pShaderFactory;
        Shader vertexShader;
        Shader fragmentShader;
    };
}
