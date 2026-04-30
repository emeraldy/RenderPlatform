//-----------------------------------------------------------------
// Shader Program Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderProgram.h"

using namespace Emerald;

void ShaderProgram::SetVertexShaderName(const std::wstring& name)
{
    vertexShaderName = name;
}

void ShaderProgram::SetPixelShaderName(const std::wstring& name)
{
    pixelShaderName = name;
}

const std::wstring& ShaderProgram::GetVertexShaderName() const
{
    return vertexShaderName;
}

const std::wstring& ShaderProgram::GetPixelShaderName() const
{
    return pixelShaderName;
}