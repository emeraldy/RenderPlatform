//-----------------------------------------------------------------
// Resource Base Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"

namespace Emerald
{
    class Resource
    {
    public:
        using ResourceID = unsigned int;

        Resource(ResourceID resID, const std::wstring& resName);
        virtual ~Resource();

        virtual const size_t CalculateSize() = 0;
        std::wstring GetName() const;
        ResourceID GetID() const;
        size_t GetByteSize() const;

    protected:
        std::wstring name;
        ResourceID id;
        size_t size; //in byte
    };
}
