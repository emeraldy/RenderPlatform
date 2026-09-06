//-----------------------------------------------------------------
// HLSL Shader Program Manager class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "HLSLShaderProgramManager.h"
#include "HLSLShaderProgramImporter.h"

using namespace Emerald;

HLSLShaderProgramManager::HLSLShaderProgramManager() : pImporter(nullptr)
{
    resourceType = ResourceType::ShaderProgram;
}

HLSLShaderProgramManager::~HLSLShaderProgramManager()
{

}

HLSLShaderProgramManager& HLSLShaderProgramManager::GetInstance()
{
    static HLSLShaderProgramManager instance;

    return instance;
}

Resource* HLSLShaderProgramManager::Create(const std::wstring& name, Resource::ResourceID id, Error& err)
{
    ShaderProgram* product = pImporter->LoadShaderProgram(name, id, err);
    if (err)
    {
        return nullptr;
    }
    else
    {
        return product;
    }
}
