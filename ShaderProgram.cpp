//-----------------------------------------------------------------
// Shader Program Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderProgram.h"
#include "ShaderProgramImporter.h"
#include "RawDataReader.h"

using namespace Emerald;

ShaderFactory::ShaderFactory(std::shared_ptr<RawDataReader> pFileReader) : pRawDataReader(pFileReader)
{

}

ShaderFactory::~ShaderFactory()
{

}

ShaderProgram::ShaderProgram(Resource::ResourceID id, std::wstring name) : pShaderFactory(nullptr), Resource(id, name)
{

}

ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::SetProgramInfo(const ParsedProgramDescription& content)
{
    programInfo = content;
}

const Shader& ShaderProgram::GetVertexShader(Error& err) const
{
    if (!vertexShader.GetExists())
    {
        err += L"Vertex shader does not exist!";
    }
    return vertexShader;
}

const Shader& ShaderProgram::GetFragmentShader(Error& err) const
{
    if (!fragmentShader.GetExists())
    {
        err += L"Fragment shader does not exist!";
    }
    return fragmentShader;
}