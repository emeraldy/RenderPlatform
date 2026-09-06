//-----------------------------------------------------------------
// Shader Program Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"
#include "Shader.h"
#include <memory>
#include <map>

namespace Emerald
{
    class ShaderProgramImporter;
    class RawDataReader;

    class ShaderFactory
    {
    public:
        ShaderFactory(std::shared_ptr<RawDataReader> pFileReader);
        virtual ~ShaderFactory();

        virtual Shader CreateVertexShader(const std::map<std::wstring, std::wstring>& shaderInfo, Error& err) = 0;
        virtual Shader CreateFragmentShader(const std::map<std::wstring, std::wstring>& shaderInfo, Error& err) = 0;

    protected:
        std::shared_ptr<RawDataReader> pRawDataReader;
    };

    class ShaderProgram : public Resource
    {
    public:
        using ParsedProgramDescription = std::map<ShaderType, std::map<std::wstring, std::wstring>>; //map<ShaderType, map<PropertyName, Value>>

        ShaderProgram(Resource::ResourceID id, std::wstring name = L"Default");
        virtual ~ShaderProgram();

        virtual void CreateShaders(std::shared_ptr<RawDataReader> pFileReader, Error& err) = 0;
        virtual void PrepareProgram(Error& err) = 0;

        void SetProgramInfo(const ParsedProgramDescription& content);
        const Shader& GetVertexShader(Error& err) const;
        const Shader& GetFragmentShader(Error& err) const;

    protected:
        std::shared_ptr<ShaderFactory> pShaderFactory;
        ParsedProgramDescription programInfo;
        Shader vertexShader;
        Shader fragmentShader;
    };
}
