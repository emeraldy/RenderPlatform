//-----------------------------------------------------------------
// HLSL Shader Program Importer Class
// 
// Derived class for importing shader program descripton from JSON and make them manager ready
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderProgramImporter.h"
#include <map>

namespace Emerald
{
    class RawDataReader;
    class HLSLShaderProgramImporter : public ShaderProgramImporter
    {
    public:
        HLSLShaderProgramImporter();
        ~HLSLShaderProgramImporter();

        virtual ShaderProgram* LoadShaderProgram(const std::wstring& name, Resource::ResourceID id, Error& err) override;

    protected:
        ShaderProgram::ParsedProgramDescription LoadProgramDescription(const std::wstring& name, Error& err) override;
    };
}