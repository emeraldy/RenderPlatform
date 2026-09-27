//-----------------------------------------------------------------
// HLSL Shader Program Manager class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "HLSLShaderProgramManager.h"
#include "ShaderProgramImporter.h"

using namespace Emerald;

HLSLShaderProgramManager::HLSLShaderProgramManager() : pProgramImporter(nullptr)
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

void HLSLShaderProgramManager::SetImporter(ShaderProgramImporter* const pImporter)
{
    pProgramImporter = pImporter;
}

Resource* HLSLShaderProgramManager::Create(const std::wstring& name, Resource::ResourceID id, Error& err)
{
    ShaderProgram* product = pProgramImporter->LoadShaderProgram(name, id, err);
    if (err)
    {
        return nullptr;
    }
    else
    {
        return product;
    }
}
