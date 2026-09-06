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
        HLSLShaderFactory(std::shared_ptr<RawDataReader> pFileReader);

        Shader CreateVertexShader(const std::map<std::wstring, std::wstring>& shaderInfo, Error& err) override;
        Shader CreateFragmentShader(const std::map<std::wstring, std::wstring>& shaderInfo, Error& err) override;
    };

    class HLSLShaderProgram : public ShaderProgram
    {
    public:
        HLSLShaderProgram(Resource::ResourceID id, std::wstring name = L"Default");
        virtual ~HLSLShaderProgram();

        void CreateShaders(std::shared_ptr<RawDataReader> pFileReader, Error& err) override;
        void PrepareProgram(Error& err) override;
        const size_t CalculateSize() override;

        Microsoft::WRL::ComPtr<ID3DBlob> GetCompiledVertexShader() const;
        Microsoft::WRL::ComPtr<ID3DBlob> GetCompiledPixelShader() const;

    private:
        Microsoft::WRL::ComPtr<ID3DBlob> pCompiledVertexShader;
        Microsoft::WRL::ComPtr<ID3DBlob> pCompiledPixelShader;
    };
}