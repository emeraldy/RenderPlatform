//-----------------------------------------------------------------
// HLSL Shader Program Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include "ShaderProgram.h"

namespace Emerald
{
    class HLSLShaderProgram : public ShaderProgram
    {
    public:
        HLSLShaderProgram(Resource::ResourceID id, std::wstring name = L"Default");

        const std::wstring& GetShaderModelTarget() const;
        const std::wstring& GetProgramEntryPoint() const;
        ID3DBlob* GetCompiledVertexShader() const;
        ID3DBlob* GetCompiledPixelShader() const;

        void SetShaderModelTarget(const std::wstring& target);
        void SetProgramEntryPoint(const std::wstring& entry);

    private:
        std::wstring shaderModelTarget;
        std::wstring programEntryPoint;
        ID3DBlob* pCompiledVertexShader;
        ID3DBlob* pCompiledPixelShader;
    };
}