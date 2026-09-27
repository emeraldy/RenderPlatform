//-----------------------------------------------------------------
// Rendering Pass class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "RenderingPass.h"
#include "HLSLShaderProgramManager.h"

using namespace Emerald;

RenderingPass::RenderingPass() : pParentMaterial(nullptr), pShaderProgramManager(nullptr)
{

}

RenderingPass::~RenderingPass()
{

}

void RenderingPass::SetParent(Material* const pMaterial)
{
    pParentMaterial = pMaterial;
}

void RenderingPass::SetName(const std::wstring& toName)
{
    name = toName;
}

void RenderingPass::SetInputData(const std::wstring& name, ShaderInputDataDescriptor::DataType dataType, float* pData, ShaderType shaderType, Error& err)
{
    shaderInputs.Add(name, dataType, pData, shaderType, err);
}

void RenderingPass::SetShaderProgramManager()
{
    if (shaderLanguage == L"HLSL")
    {
        pShaderProgramManager = &HLSLShaderProgramManager::GetInstance();
    }
}

void RenderingPass::SetShaderProgramName(const std::wstring& name)
{
    shaderProgramName = name;
}

void RenderingPass::SetShaderLanguage(const std::wstring& lang)
{
    shaderLanguage = lang;
}

Material* RenderingPass::GetParent() const
{
    return pParentMaterial;
}

const std::wstring RenderingPass::GetName() const
{
    return name;
}

std::shared_ptr<ShaderProgram> RenderingPass::GetShaderProgram(Error& err) const
{
    std::shared_ptr<Resource> pResource = pShaderProgramManager->Load(L"..\\assets\\shaderprograms\\" + shaderProgramName, err);
    if (!pResource)
    {
        return nullptr;
    }
    
    return std::dynamic_pointer_cast<ShaderProgram>(pResource);
}

ShaderInputData& RenderingPass::GetShaderInputData()
{
    return shaderInputs;
}