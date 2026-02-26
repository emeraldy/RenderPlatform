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
    using ResourceID = unsigned int;
    class Resource
    {
    public:
        Resource(const std::wstring& resName, ResourceID resID);
        virtual ~Resource();

        std::wstring getName() const;
        ResourceID getID() const;
        size_t getSize() const;
        bool getIsLoaded() const;

    protected:
        std::wstring name;
        ResourceID id;
        size_t size; //in byte
        bool isLoaded;
    };
}