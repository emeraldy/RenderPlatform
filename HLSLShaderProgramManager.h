//-----------------------------------------------------------------
// HLSL Shader Program Manager class
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ResourceManager.h"
#include "HLSLShaderProgram.h"

namespace Emerald
{
    class HLSLShaderProgramImporter;
    class HLSLShaderProgramManager : public ResourceManager
    {
    public:
        HLSLShaderProgramManager(const HLSLShaderProgramManager&) = delete;
        HLSLShaderProgramManager operator = (const HLSLShaderProgramManager&) = delete;
        
        virtual ~HLSLShaderProgramManager();

        static HLSLShaderProgramManager& GetInstance();

    protected:
        Resource* Create(const std::wstring& name, Resource::ResourceID id, Error& err) override;

    private:
        HLSLShaderProgramManager();

        HLSLShaderProgramImporter* pImporter;
    };
}
