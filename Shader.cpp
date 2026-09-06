//-----------------------------------------------------------------
// Shader Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Shader.h"

using namespace Emerald;

Shader::Shader() : type(ShaderType::Unknown), name(L""), source(L""), exists(false)
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

std::wstring Shader::GetEntryPoint() const
{
    return entryPoint;
}

std::wstring Shader::GetVersion() const
{
    return version;
}

const std::wstring& Shader::GetSource() const
{
    return source;
}

bool Shader::GetExists() const
{
    return exists;
}

void Shader::SetType(ShaderType value)
{
    type = value;
}

void Shader::SetName(const std::wstring& value)
{
    name = value;
}

void Shader::SetEntryPoint(const std::wstring& value)
{
    entryPoint = value;
}

void Shader::SetVersion(const std::wstring& value)
{
    version = value;
}

void Shader::SetSource(const std::wstring& value)
{
    source = value;
}

void Shader::SetExists(bool value)
{
    exists = value;
}