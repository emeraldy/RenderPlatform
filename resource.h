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

        std::wstring GetName() const;
        ResourceID GetID() const;
        size_t GetByteSize() const;
        bool GetIsLoaded() const;

    protected:
        std::wstring name;
        ResourceID id;
        size_t size; //in byte
        bool isLoaded;
    };
}