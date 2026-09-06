//-----------------------------------------------------------------
// Shader Program Importer Base Class
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderProgram.h"

namespace Emerald
{
    class ShaderProgramImporter
    {
    public:
        virtual ShaderProgram* LoadShaderProgram(const std::wstring& name, Resource::ResourceID id, Error& err) = 0;

    protected:
        virtual ShaderProgram::ParsedProgramDescription LoadProgramDescription(const std::wstring& name, Error& err) = 0;
    };
}