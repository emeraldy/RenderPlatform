//-----------------------------------------------------------------
// Shader Input Data Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ShaderInputData.h"

using namespace Emerald;

void ShaderInputData::Add(const std::wstring& name, ShaderInputDataDescriptor::DataType type, std::weak_ptr<float> pValue, Error& err)
{
    for (const auto& ele : dataInfo)
    {
        if (ele.name == name)
        {
            return;
        }
    }

    ShaderInputDataDescriptor descriptor;
    descriptor.name = name;
    descriptor.type = type;
    descriptor.startIndex = dataStorage.size();
    switch (type)
    {
    case ShaderInputDataDescriptor::DataType::Scalar:
        descriptor.length = 1;
        break;
    case ShaderInputDataDescriptor::DataType::Vector2:
        descriptor.length = 2;
        break;
    case ShaderInputDataDescriptor::DataType::Vector3:
        descriptor.length = 3;
        break;
    case ShaderInputDataDescriptor::DataType::Vector4:
        descriptor.length = 4;
        break;
    case ShaderInputDataDescriptor::DataType::Matrix3:
        descriptor.length = 9;
        break;
    case ShaderInputDataDescriptor::DataType::Matrix4:
        descriptor.length = 16;
        break;
    }
    dataInfo.push_back(descriptor);

    std::shared_ptr<float> pValueStrong = pValue.lock();
    if (pValueStrong == nullptr)
    {
        err += L"Shader input data has lost.";
        return;
    }
    float* pFloat = pValueStrong.get();
    if (!pFloat)
    {
        err += L"Shader input data to add has lost in function " + StringUtilities::ConvertCharStringToWide(__func__) + 
            L" in " + StringUtilities::ConvertCharStringToWide(__FILE__);
        return;
    }
    for (int i = 0; i < descriptor.length; i++)
    {
        dataStorage.push_back(pFloat[i]);
    }
}

std::pair<ShaderInputDataDescriptor, std::shared_ptr<float>> ShaderInputData::GetByName(const std::wstring& name, Error& err) const
{
    ShaderInputDataDescriptor descriptor;
    auto cIter = dataInfo.cbegin();
    for (; cIter != dataInfo.cend(); ++cIter)
    {
        if (cIter->name == name)
        {
            descriptor = *cIter;
            break;
        }
    }
    if (cIter == dataInfo.cend())
    {
        err += L"Function " + StringUtilities::ConvertCharStringToWide(__func__) + L" in " + 
            StringUtilities::ConvertCharStringToWide(__FILE__) + L" can not find shader input data " + name;
        return std::make_pair(descriptor, nullptr);
    }
    float* pData = new float[descriptor.length];
    if (!pData)
    {
        err += L"can not allocate enough memory for obtained data in function " +
            StringUtilities::ConvertCharStringToWide(__func__) + L" in " + StringUtilities::ConvertCharStringToWide(__FILE__);
        return std::make_pair(descriptor, nullptr);
    }
    for (int i = 0; i < descriptor.length; i++)
    {
        pData[i] = dataStorage[descriptor.startIndex + i];
    }
    return std::make_pair(descriptor, std::shared_ptr<float>(pData, std::default_delete<float[]>()));
}

ShaderInputData::ConstDescriptorIter& ShaderInputData::GetDescriptorBeginIterator() const
{
    return dataInfo.cbegin();
}

ShaderInputData::ConstStorageIter& ShaderInputData::GetStorageBeginIterator() const
{
    return dataStorage.cbegin();
}

ShaderInputData::ConstDescriptorIter& ShaderInputData::GetDescriptorEndIterator() const
{
    return dataInfo.cend();
}

ShaderInputData::ConstStorageIter& ShaderInputData::GetStorageEndIterator() const
{
    return dataStorage.cend();
}