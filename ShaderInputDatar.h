//-----------------------------------------------------------------
// Shader Input Data Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"
#include <vector>

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
        size_t start;
        size_t length;
    };
    
    class ShaderInputData
    {
    public:
    private:
        std::vector<ShaderInputDataDescriptor> dataInfo;
        std::vector<float> dataStorage;
    };
}