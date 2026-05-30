//-----------------------------------------------------------------
// HLSL Shader Program Manager class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "HLSLShaderProgramManager.h"
#include "ShaderInputData.h"
#include "WinFileRawDataReader.h"

using namespace Emerald;

HLSLShaderProgramManager::HLSLShaderProgramManager()
{

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
    HLSLShaderProgram* pProgram = new HLSLShaderProgram(id, name);
    pProgram->CreateShaders(err);
    if (err)
    {
        return nullptr;
    }
    pProgram->PrepareProgram(err);
    if (err)
    {
        return nullptr;
    }
    pProgram->SetProgramEntryPoint(L"main");
    pProgram->SetModelTargetVersion(L"5_0");//remember to prefix it with shader type when use

    return pProgram;
}
