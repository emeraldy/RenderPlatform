//-----------------------------------------------------------------
// Shader Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"

namespace Emerald
{
    enum class ShaderType
    {
        Unknown,
        Vertex,
        Fragment
    };

    class Shader
    {
    public:
        Shader();

        ShaderType GetType() const;
        std::wstring GetName() const;
        std::wstring GetEntryPoint() const;
        std::wstring GetVersion() const;
        const std::wstring& GetSource() const;
        bool GetExists() const;

        void SetType(ShaderType value);
        void SetName(const std::wstring& value);
        void SetEntryPoint(const std::wstring& value);
        void SetVersion(const std::wstring& value);
        void SetSource(const std::wstring& value);
        void SetExists(bool value);

    private:
        ShaderType type;
        std::wstring name;
        std::wstring entryPoint;
        std::wstring version;//target model for HLSL
        std::wstring source;
        bool exists;
    };
}