//-----------------------------------------------------------------
// HLSL Shader Program Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include "ShaderProgram.h"

namespace Emerald
{
    class HLSLShaderFactory : public ShaderFactory
    {
    public:
        HLSLShaderFactory();

        Shader CreateVertexShader(const std::wstring& programName, Error& err) override;
        Shader CreateFragmentShader(const std::wstring& programName, Error& err) override;

    private:
        void LoadShaderProgramDescription(const std::wstring& programName, Error& err);
        std::wstring RetrieveShaderSourceFileName(const std::wstring& programName, const ShaderType t, Error& err);
        
        std::wstring shaderProgramDescription;
    };

    class HLSLShaderProgram : public ShaderProgram
    {
    public:
        HLSLShaderProgram(Resource::ResourceID id, std::wstring name = L"Default");
        virtual ~HLSLShaderProgram();

        void CreateShaders(Error& err) override;
        void PrepareProgram(Error& err) override;
        const size_t CalculateSize() override;

        const std::wstring& GetModelTargetVersion() const;
        const std::wstring& GetProgramEntryPoint() const;
        Microsoft::WRL::ComPtr<ID3DBlob> GetCompiledVertexShader() const;
        Microsoft::WRL::ComPtr<ID3DBlob> GetCompiledPixelShader() const;
        void SetModelTargetVersion(const std::wstring& target);
        void SetProgramEntryPoint(const std::wstring& entry);

    private:
        std::wstring modelTargetVersion;
        std::wstring programEntryPoint;
        Microsoft::WRL::ComPtr<ID3DBlob> pCompiledVertexShader;
        Microsoft::WRL::ComPtr<ID3DBlob> pCompiledPixelShader;
    };
}