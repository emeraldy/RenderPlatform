//-----------------------------------------------------------------
// Rendering Pass Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"
#include "Vector4.h"
#include "ShaderInputData.h"
#include <memory>

namespace Emerald
{
    class Material;
    class ShaderProgram;
    class ResourceManager;

    class RenderingPass
    {
    private:
        std::wstring name;
        std::wstring shaderProgramName;
        ShaderInputData shaderInputs;
        Material* pParentMaterial;
        ResourceManager* pShaderProgramManager;
    
    public:
        RenderingPass();
        ~RenderingPass();

        void SetParent(Material* const pMaterial);
        void SetName(const std::wstring& toName);
        void SetShaderProgramName(const std::wstring& name);
        void SetInputData(const std::wstring& name, ShaderInputDataDescriptor::DataType dataType, float* values, ShaderType shaderType, Error& err);
        void SetShaderProgramManager(ResourceManager* pManager);

        Material* GetParent() const;
        const std::wstring GetName() const;
        std::shared_ptr<ShaderProgram> GetShaderProgram(Error& err) const;
        ShaderInputData& GetShaderInputData();
    };
}