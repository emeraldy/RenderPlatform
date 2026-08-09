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
    pShaderFactory = new HLSLShaderFactory;
}

HLSLShaderProgram::~HLSLShaderProgram()
{
    SAFE_DELETE(pShaderFactory);
}

void HLSLShaderProgram::CreateShaders(Error& err)
{
    pShaderFactory->CreateVertexShader(name, err);//name is the name of this shader program, resource name.
    pShaderFactory->CreateFragmentShader(name, err);
}

void HLSLShaderProgram::PrepareProgram(Error& err)
{
    Microsoft::WRL::ComPtr<ID3DBlob> pCompilationError;
    //vertex shader
    std::string source = StringUtilities::ConvertWideStringToChar(vertexShader.GetSource());
    size_t sourceLength = source.length();
    std::string entryPoint = StringUtilities::ConvertWideStringToChar(programEntryPoint);
    std::string targetVersion = StringUtilities::ConvertWideStringToChar(modelTargetVersion);
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

const std::wstring& HLSLShaderProgram::GetModelTargetVersion() const
{
    return modelTargetVersion;
}

const std::wstring& HLSLShaderProgram::GetProgramEntryPoint() const
{
    return programEntryPoint;
}

Microsoft::WRL::ComPtr<ID3DBlob> HLSLShaderProgram::GetCompiledVertexShader() const
{
    return pCompiledVertexShader;
}

Microsoft::WRL::ComPtr<ID3DBlob> HLSLShaderProgram::GetCompiledPixelShader() const
{
    return pCompiledPixelShader;
}

void HLSLShaderProgram::SetModelTargetVersion(const std::wstring& target)
{
    modelTargetVersion = target;
}
void HLSLShaderProgram::SetProgramEntryPoint(const std::wstring& entry)
{
    programEntryPoint = entry;
}

//++++++++++++++++++ HLSL Shader Factory +++++++++++++++++++++++++
HLSLShaderFactory::HLSLShaderFactory() : shaderProgramDescription(L"")
{

}

std::wstring HLSLShaderFactory::RetrieveShaderSourceFileName(const std::wstring& programName, const ShaderType t, Error& err)
{
    std::wstring shaderType;
    switch (t)
    {
    case ShaderType::Vertex:
        shaderType = L"vertex";
        break;
    case ShaderType::Fragment:
        shaderType = L"fragment";
        break;
    default:
        shaderType = L"unknown";
    }
    using namespace rapidjson;
    GenericDocument<UTF16LE<>> jsonDoc;
    if (jsonDoc.Parse(shaderProgramDescription.c_str()).HasParseError())
    {
        err += L"Shader program " + programName + L" json parse failed.";
        return L"";
    }
    bool legalJson = true;
    if (jsonDoc.HasMember(L"shaders") && jsonDoc[L"shaders"].IsArray())
    {
        for (auto& s : jsonDoc[L"shaders"].GetArray())
        {
            if (s.IsObject() && s.GetObj().HasMember(L"type") && s.GetObj().HasMember(L"name"))
            {
                if (s.GetObj()[L"type"].GetString() == shaderType)
                {
                    return s.GetObj()[L"name"].GetString();//a program should have only one shade of a specific type
                }
            }
            else
            {
                legalJson = false;
                break;
            }
        }
    }
    else
    {
        legalJson = false;
    }
    if (!legalJson)
    {
        err += L"Shader program " + programName + L" has illegal json description.";
        return L"";
    }
}

Shader HLSLShaderFactory::CreateVertexShader(const std::wstring& programName, Error& err)
{
    Shader vertex;
    //locate shader source name via shader program description 
    if (shaderProgramDescription == L"")
    {
        LoadShaderProgramDescription(programName, err);
        if (err)
        {
            return vertex;
        }
    }
    std::wstring sourceName = RetrieveShaderSourceFileName(programName, ShaderType::Vertex, err);
    if (err)
    {
        return vertex;
    }
    vertex.SetType(ShaderType::Vertex);
    vertex.SetName(sourceName);

    //load shader source code
    WinFileRawDataReader fileReader;
    err = fileReader.LoadRawData(L"assets\\shaders\\" + vertex.GetName());
    if (err)
    {
        return vertex;
    }
    std::wstring sourceCode = StringUtilities::DecodeStringBytes(65001, fileReader.GetBuffer(), fileReader.GetBufferSize(), 0, err);
    if (err)
    {
        return vertex;
    }
    vertex.SetSource(sourceCode);

    return vertex;
}

Shader HLSLShaderFactory::CreateFragmentShader(const std::wstring& programName, Error& err)
{
    Shader fragment;
    //locate shader source name via shader program description 
    if (shaderProgramDescription == L"")
    {
        LoadShaderProgramDescription(programName, err);
        if (err)
        {
            return fragment;
        }
    }
    std::wstring sourceName = RetrieveShaderSourceFileName(programName, ShaderType::Fragment, err);
    if (err)
    {
        return fragment;
    }
    fragment.SetType(ShaderType::Fragment);
    fragment.SetName(sourceName);

    //load shader source code
    WinFileRawDataReader fileReader;
    err = fileReader.LoadRawData(L"assets\\shaders\\" + fragment.GetName());
    if (err)
    {
        return fragment;
    }
    std::wstring sourceCode = StringUtilities::DecodeStringBytes(65001, fileReader.GetBuffer(), fileReader.GetBufferSize(), 0, err);
    if (err)
    {
        return fragment;
    }
    fragment.SetSource(sourceCode);

    return fragment;
}

void HLSLShaderFactory::LoadShaderProgramDescription(const std::wstring& programName, Error& err)
{
    WinFileRawDataReader fileReader;
    err = fileReader.LoadRawData(L"assets\\shaderprograms\\" + programName + L".json");
    if (err)
    {
        return;
    }
    shaderProgramDescription = StringUtilities::DecodeStringBytes(65001, fileReader.GetBuffer(), fileReader.GetBufferSize(), 0, err);
    if (err)
    {
        return;
    }
}