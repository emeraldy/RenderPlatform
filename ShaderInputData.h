//-----------------------------------------------------------------
// Shader Input Data Class Header
// 
// These data can be constant buffer data for HLSL or uniform data for GLSL.
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <vector>
#include <memory>
#include <utility>
#include "StandardIncludes.h"
#include "ShaderProgram.h"

namespace Emerald
{
    struct ShaderInputDataDescriptor
    {
        enum class DataType
        {
            Unknown,
            Scalar,
            Vector2,
            Vector3,
            Vector4,
            Matrix3,
            Matrix4
        };

        std::wstring name;
        DataType type;
        size_t startIndex;
        size_t length;
        ShaderType targetShaderType;

        ShaderInputDataDescriptor() : type(DataType::Unknown), startIndex(0), length(0), targetShaderType(ShaderType::Unknown)
        {

        }
    };
    
    class ShaderInputData
    {
    public:
        using ConstDescriptorIter = std::vector<ShaderInputDataDescriptor>::const_iterator;
        using ConstStorageIter = std::vector<float>::const_iterator;

        void Add(const std::wstring& name, ShaderInputDataDescriptor::DataType type, std::weak_ptr<float> pValue, ShaderType shader, Error& err);
        std::pair<ShaderInputDataDescriptor, std::shared_ptr<float>> GetByName(const std::wstring& name, Error& err) const;
        ConstDescriptorIter& GetDescriptorBeginIterator() const;
        ConstStorageIter& GetStorageBeginIterator() const;
        ConstDescriptorIter& GetDescriptorEndIterator() const;
        ConstStorageIter& GetStorageEndIterator() const;

    private:
        std::vector<ShaderInputDataDescriptor> dataInfo;
        std::vector<float> dataStorage;
    };
}