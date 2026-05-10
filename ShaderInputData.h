//-----------------------------------------------------------------
// Shader Input Data Class Header
// 
// These data can be constant buffer data for HLSL or uniform data for GLSL.
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"
#include <vector>
#include <memory>
#include <utility>

namespace Emerald
{
    struct ShaderInputDataDescriptor
    {
        enum class DataType
        {
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
    };
    
    class ShaderInputData
    {
    public:
        using ConstDescriptorIter = std::vector<ShaderInputDataDescriptor>::const_iterator;
        using ConstStorageIter = std::vector<float>::const_iterator;

        void Add(const std::wstring& name, ShaderInputDataDescriptor::DataType type, std::weak_ptr<float> pValue, Error& err);
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