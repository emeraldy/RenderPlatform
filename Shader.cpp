//-----------------------------------------------------------------
// Shader Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Shader.h"

using namespace Emerald;

Shader::Shader() : type(ShaderType::Unknown), name(L""), source(L"")
{

}

ShaderType Shader::GetType() const
{
    return type;
}

std::wstring Shader::GetName() const
{
    return name;
}

const std::wstring& Shader::GetSource() const
{
    return source;
}

void Shader::SetType(ShaderType value)
{
    type = value;
}

void Shader::SetName(const std::wstring& value)
{
    name = value;
}

void Shader::SetSource(const std::wstring& value)
{
    source = value;
}