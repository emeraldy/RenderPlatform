//-----------------------------------------------------------------
// Resource Manager base class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ResourceManager.h"
#define NOMINMAX
#include <Shlwapi.h>


using namespace Emerald;

ResourceManager::ResourceManager()
{
    resourceType = ResourceType::Default;
    nextID = 1;
    memoryUsage = 0;
    memoryBudget = std::numeric_limits<size_t>::max();
}

ResourceManager::~ResourceManager()
{
    UnloadAll();
}

std::shared_ptr<Resource> ResourceManager::Load(const std::wstring& name, Error& err)
{
    if (!PathFileExists(name.c_str()))
    {
        err += L"Requested resource " + name + L" does not exist!";
        return nullptr;
    }
    NameIDRegistry::const_iterator IDIter = IDRegistry.find(name);
    if (IDIter == IDRegistry.cend())
    {
        Resource::ResourceID currentID = GetNextID();
        Resource* pNewResource = Create(name, currentID, err);
        if (err)
        {
            return nullptr;
        }
        else
        {
            std::shared_ptr<Resource> spNewResource(pNewResource);
            IDRegistry[name] = currentID;
            storage[currentID] = spNewResource;
            return spNewResource;
        }
    }
    else
    {
        ResourceStorage::const_iterator storageIter = storage.find(IDIter->second);
        return storageIter->second;
    }
}

void ResourceManager::Unload(const std::wstring& name)
{
    NameIDRegistry::const_iterator IDIter = IDRegistry.find(name);
    if (IDIter != IDRegistry.cend())
    {
        Resource::ResourceID targetID = IDIter->second;
        storage.erase(targetID);
        IDRegistry.erase(name);
    }
}

void ResourceManager::UnloadAll()
{
    IDRegistry.clear();
    storage.clear();
}

size_t ResourceManager::GetMemoryUsage() const
{
    return memoryUsage;
}

void ResourceManager::SetMemoryBudget(size_t amount)
{
    memoryBudget = amount;
}

size_t ResourceManager::GetMemoryBudget() const
{
    return memoryBudget;
}

ResourceType ResourceManager::GetResourceType() const
{
    return resourceType;
}

Resource::ResourceID ResourceManager::GetNextID()
{
    return nextID++;
}