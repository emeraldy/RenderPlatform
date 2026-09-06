//-----------------------------------------------------------------
// HLSL Shader Program Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "HLSLShaderProgram.h"
#include "WinFileRawDataReader.h"
#include "rapidjson/document.h"

using namespace Emerald;

HLSLShaderProgram::HLSLShaderProgram(Resource::ResourceID id, std::wstring name) : ShaderProgram(id, name)
{

}

HLSLShaderProgram::~HLSLShaderProgram()
{

}

void HLSLShaderProgram::CreateShaders(std::shared_ptr<RawDataReader> pFileReader, Error& err)
{
    pShaderFactory = std::make_shared<HLSLShaderFactory>(pFileReader);
    vertexShader = pShaderFactory->CreateVertexShader(programInfo[ShaderType::Vertex], err);
    if (err)
    {
        return;
    }
    fragmentShader = pShaderFactory->CreateFragmentShader(programInfo[ShaderType::Fragment], err);
}

void HLSLShaderProgram::PrepareProgram(Error& err)
{
    Microsoft::WRL::ComPtr<ID3DBlob> pCompilationError;
    //vertex shader
    std::string source = StringUtilities::ConvertWideStringToChar(vertexShader.GetSource());
    size_t sourceLength = source.length();
    std::string entryPoint = StringUtilities::ConvertWideStringToChar(vertexShader.GetEntryPoint());
    std::string targetVersion = StringUtilities::ConvertWideStringToChar(vertexShader.GetVersion());
    HRESULT hr = D3DCompile(source.c_str(), sourceLength, NULL, NULL, NULL, entryPoint.c_str(), ("vs_" + targetVersion).c_str(), 0, 0, pCompiledVertexShader.GetAddressOf(), pCompilationError.GetAddressOf());
    if (FAILED(hr))
    {
        std::string errorMsg(static_cast<char*>(pCompilationError->GetBufferPointer()));
        std::wstring errorMsgWide = StringUtilities::ConvertCharStringToWide(errorMsg);
        err += errorMsgWide;
        return;
    }

    //pixel shader
    source = StringUtilities::ConvertWideStringToChar(fragmentShader.GetSource());
    sourceLength = source.length();
    entryPoint = StringUtilities::ConvertWideStringToChar(fragmentShader.GetEntryPoint());
    targetVersion = StringUtilities::ConvertWideStringToChar(fragmentShader.GetVersion());
    hr = D3DCompile(source.c_str(), sourceLength, NULL, NULL, NULL, entryPoint.c_str(), ("ps_" + targetVersion).c_str(), 0, 0, pCompiledPixelShader.GetAddressOf(), pCompilationError.GetAddressOf());
    if (FAILED(hr))
    {
        std::string errorMsg(static_cast<char*>(pCompilationError->GetBufferPointer()));
        std::wstring errorMsgWide = StringUtilities::ConvertCharStringToWide(errorMsg);
        err += errorMsgWide;
        return;
    }
}

const size_t HLSLShaderProgram::CalculateSize()
{
    size_t objectSize = sizeof(*this);
    objectSize += pCompiledVertexShader->GetBufferSize();
    objectSize += pCompiledPixelShader->GetBufferSize();

    return objectSize;
}

Microsoft::WRL::ComPtr<ID3DBlob> HLSLShaderProgram::GetCompiledVertexShader() const
{
    return pCompiledVertexShader;
}

Microsoft::WRL::ComPtr<ID3DBlob> HLSLShaderProgram::GetCompiledPixelShader() const
{
    return pCompiledPixelShader;
}

//++++++++++++++++++ HLSL Shader Factory +++++++++++++++++++++++++
HLSLShaderFactory::HLSLShaderFactory(std::shared_ptr<RawDataReader> pFileReader) : ShaderFactory(pFileReader)
{

}

Shader HLSLShaderFactory::CreateVertexShader(const std::map<std::wstring, std::wstring>& shaderInfo, Error& err)
{
    Shader vertex;
    vertex.SetType(ShaderType::Vertex);
    vertex.SetName(shaderInfo.at(L"name"));
    vertex.SetEntryPoint(shaderInfo.at(L"entrypoint"));
    vertex.SetVersion(shaderInfo.at(L"modeltarget"));

    //load shader source code
    err = pRawDataReader->LoadRawData(L"assets\\shaders\\" + vertex.GetName());
    if (err)
    {
        return vertex;
    }
    std::wstring sourceCode = StringUtilities::DecodeStringBytes(65001, pRawDataReader->GetBuffer(), pRawDataReader->GetBufferSize(), 0, err);
    if (err)
    {
        return vertex;
    }
    vertex.SetSource(sourceCode);
    vertex.SetExists(true);

    return vertex;
}

Shader HLSLShaderFactory::CreateFragmentShader(const std::map<std::wstring, std::wstring>& shaderInfo, Error& err)
{
    Shader fragment;
    fragment.SetType(ShaderType::Fragment);
    fragment.SetName(shaderInfo.at(L"name"));
    fragment.SetEntryPoint(shaderInfo.at(L"entrypoint"));
    fragment.SetVersion(shaderInfo.at(L"modeltarget"));

    //load shader source code
    err = pRawDataReader->LoadRawData(L"assets\\shaders\\" + fragment.GetName());
    if (err)
    {
        return fragment;
    }
    std::wstring sourceCode = StringUtilities::DecodeStringBytes(65001, pRawDataReader->GetBuffer(), pRawDataReader->GetBufferSize(), 0, err);
    if (err)
    {
        return fragment;
    }
    fragment.SetSource(sourceCode);
    fragment.SetExists(true);

    return fragment;
}