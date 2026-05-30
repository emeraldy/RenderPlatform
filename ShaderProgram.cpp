//-----------------------------------------------------------------
// Shader Program Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderProgram.h"

using namespace Emerald;

ShaderProgram::ShaderProgram(Resource::ResourceID id, std::wstring name) : pShaderFactory(nullptr), Resource(id, name)
{

}

ShaderProgram::~ShaderProgram()
{

}

