//-----------------------------------------------------------------
// Shader Program Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"
#include "ShaderInputData.h"

namespace Emerald
{
    class ShaderProgram : public Resource
    {
    public:
        ShaderProgram(Resource::ResourceID id, std::wstring name = L"Default");
        virtual ~ShaderProgram();

        void SetVertexShaderName(const std::wstring& name);
        void SetFragmentShaderName(const std::wstring& name);
        const std::wstring& GetVertexShaderName() const;
        const std::wstring& GetFragmentShaderName() const;
        ShaderInputData& GetVertexShaderInputData();
        ShaderInputData& GetFragmentShaderInputData();
        unsigned char* GetVertexShaderSource();
        unsigned char* GetFragmentShaderSource();
        size_t GetVertexShaderSourceSize() const;
        void SetVertexShaderSourceSize(size_t size);
        size_t GetFragmentShaderSourceSize() const;
        void SetFragmentShaderSourceSize(size_t size);
    
    protected:
        std::wstring vertexShaderName;
        std::wstring fragmentShaderName;
        ShaderInputData vertexShaderInputData;
        ShaderInputData fragmentShaderInputData;
        unsigned char* pVertexShaderSource;
        size_t vertexShaderSourceSize;
        unsigned char* pFragmentShaderSource;
        size_t fragmentShaderSourceSize;
    };
}
