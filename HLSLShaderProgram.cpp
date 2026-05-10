//-----------------------------------------------------------------
// HLSL Shader Program Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "HLSLShaderProgram.h"

using namespace Emerald;

HLSLShaderProgram::HLSLShaderProgram(Resource::ResourceID id, std::wstring name = L"Default") : ShaderProgram(id, name)
{

}

const std::wstring& HLSLShaderProgram::GetShaderModelTarget() const
{
    return shaderModelTarget;
}

const std::wstring& HLSLShaderProgram::GetProgramEntryPoint() const
{
    return programEntryPoint;
}

ID3DBlob* HLSLShaderProgram::GetCompiledVertexShader() const
{
    return pCompiledVertexShader;
}

ID3DBlob* HLSLShaderProgram::GetCompiledPixelShader() const
{
    return pCompiledPixelShader;
}

void HLSLShaderProgram::SetShaderModelTarget(const std::wstring& target)
{
    shaderModelTarget = target;
}
void HLSLShaderProgram::SetProgramEntryPoint(const std::wstring& entry)
{
    programEntryPoint = entry;
}
