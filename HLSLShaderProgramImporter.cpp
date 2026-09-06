//-----------------------------------------------------------------
// HLSL Shader Program Importer Class
// 
// Derived class for importing HLSL shader program descripton from JSON and make them manager ready
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "rapidjson/document.h"
#include "WinFileRawDataReader.h"
#include "HLSLShaderProgramImporter.h"
#include "HLSLShaderProgram.h"

using namespace Emerald;

HLSLShaderProgramImporter::HLSLShaderProgramImporter()
{

}

HLSLShaderProgramImporter::~HLSLShaderProgramImporter()
{

}

ShaderProgram* HLSLShaderProgramImporter::LoadShaderProgram(const std::wstring& name, Resource::ResourceID id, Error& err)
{
    ShaderProgram* pProgram = new HLSLShaderProgram(id, name);
    pProgram->SetProgramInfo(LoadProgramDescription(name, err));
    if (err)
    {
        return nullptr;
    }
    
    std::shared_ptr<WinFileRawDataReader> pWinFileReader(new WinFileRawDataReader());
    pProgram->CreateShaders(pWinFileReader, err);
    if (err)
    {
        return nullptr;
    }
    pProgram->PrepareProgram(err);
    if (err)
    {
        return nullptr;
    }

    return pProgram;
}

ShaderProgram::ParsedProgramDescription HLSLShaderProgramImporter::LoadProgramDescription(const std::wstring& name, Error& err)
{
    ShaderProgram::ParsedProgramDescription shaderProgramContent;
    WinFileRawDataReader fileReader;
    Error localErr;
    localErr = fileReader.LoadRawData(L"assets\\shaderprograms\\" + name + L".program");
    if (localErr)
    {
        err += localErr;
        return shaderProgramContent;
    }
    std::wstring shaderProgramSource = StringUtilities::DecodeStringBytes(65001, fileReader.GetBuffer(), fileReader.GetBufferSize(), 0, localErr);
    if (localErr)
    {
        err += localErr;
        return shaderProgramContent;
    }

    using namespace rapidjson;
    GenericDocument<UTF16LE<>> jsonDoc;
    if (jsonDoc.Parse(shaderProgramSource.c_str()).HasParseError())
    {
        err += L"Shader program " + name + L" json parse failed.";
        return shaderProgramContent;
    }

    std::map<std::wstring, std::wstring> shaderProperties;
    ShaderType shaderType;
    for (auto& s : jsonDoc[L"shaders"].GetArray())
    {
        if (s.GetObj()[L"type"].GetString() == L"vertex")
        {
            shaderType = ShaderType::Vertex;
        }
        else if (s.GetObj()[L"type"].GetString() == L"fragment")
        {
            shaderType = ShaderType::Fragment;
        }
        shaderProperties[L"name"] = s.GetObj()[L"name"].GetString();
        shaderProperties[L"modeltarget"] = s.GetObj()[L"modeltarget"].GetString();
        shaderProperties[L"entrypoint"] = s.GetObj()[L"entrypoint"].GetString();

        shaderProgramContent[shaderType] = shaderProperties;
    }

    return shaderProgramContent;
}