//-----------------------------------------------------------------
// Shader Program Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderProgram.h"

using namespace Emerald;

ShaderProgram::ShaderProgram(Resource::ResourceID id, std::wstring name) : Resource(id, name)
{

}

ShaderProgram::~ShaderProgram()
{
    SAFE_DELETEARRAY(pVertexShaderSource);
    SAFE_DELETEARRAY(pFragmentShaderSource);
}

void ShaderProgram::SetVertexShaderName(const std::wstring& name)
{
    vertexShaderName = name;
}

void ShaderProgram::SetFragmentShaderName(const std::wstring& name)
{
    fragmentShaderName = name;
}

const std::wstring& ShaderProgram::GetVertexShaderName() const
{
    return vertexShaderName;
}

const std::wstring& ShaderProgram::GetFragmentShaderName() const
{
    return fragmentShaderName;
}

ShaderInputData& ShaderProgram::GetVertexShaderInputData()
{
    return vertexShaderInputData;
}

ShaderInputData& ShaderProgram::GetFragmentShaderInputData()
{
    return fragmentShaderInputData;
}

unsigned char* ShaderProgram::GetVertexShaderSource()
{
    return pVertexShaderSource;
}

unsigned char* ShaderProgram::GetFragmentShaderSource()
{
    return pFragmentShaderSource;
}

size_t ShaderProgram::GetVertexShaderSourceSize() const
{
    return vertexShaderSourceSize;
}

void ShaderProgram::SetVertexShaderSourceSize(size_t size)
{
    vertexShaderSourceSize = size;
}

size_t ShaderProgram::GetFragmentShaderSourceSize() const
{
    return fragmentShaderSourceSize;
}

void ShaderProgram::SetFragmentShaderSourceSize(size_t size)
{
    fragmentShaderSourceSize = size;
}