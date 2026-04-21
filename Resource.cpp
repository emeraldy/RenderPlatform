//-----------------------------------------------------------------
// Resource Base Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"

using namespace Emerald;

Resource::Resource(Resource::ResourceID resID, const std::wstring& resName)
{
    name = resName;
    id = resID;
    size = 0;
    isLoaded = false;
}

void Resource::SetIsLoaded(bool flag)
{
    isLoaded = flag;
}

std::wstring Resource::GetName() const
{
    return name;
}

Resource::ResourceID Resource::GetID() const
{
    return id;
}

size_t Resource::GetByteSize() const
{
    return size;
}

bool Resource::GetIsLoaded() const
{
    return isLoaded;
}