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
        const std::wstring& GetSource() const;

        void SetType(ShaderType value);
        void SetName(const std::wstring& value);
        void SetSource(const std::wstring& value);

    private:
        ShaderType type;
        std::wstring name;
        std::wstring source;
    };
}