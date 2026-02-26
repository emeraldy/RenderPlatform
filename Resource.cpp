//-----------------------------------------------------------------
// Resource Base Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"

using namespace Emerald;

Resource::Resource(const std::wstring& resName, ResourceID resID)
{
    name = resName;
    id = resID;
    size = 0;
    isLoaded = false;
}

Resource::~Resource()
{

}

std::wstring Resource::getName() const
{
    return name;
}

ResourceID Resource::getID() const
{
    return id;
}

size_t Resource::getSize() const
{
    return size;
}

bool Resource::getIsLoaded() const
{
    return isLoaded;
}