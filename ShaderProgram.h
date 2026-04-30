//-----------------------------------------------------------------
// Shader Program Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"

namespace Emerald
{
    class ShaderProgram : public Resource
    {
    public:
        void SetVertexShaderName(const std::wstring& name);
        void SetPixelShaderName(const std::wstring& name);
        const std::wstring& GetVertexShaderName() const;
        const std::wstring& GetPixelShaderName() const;
    protected:
        std::wstring vertexShaderName;
        std::wstring pixelShaderName;
    };
}
