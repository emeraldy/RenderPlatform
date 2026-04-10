//-----------------------------------------------------------------
// Resource Manager base class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ResourceManager.h"

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

std::shared_ptr<Resource> ResourceManager::Load(const std::wstring& name)
{
    NameIDRegistry::const_iterator IDIter = IDRegistry.find(name);
    if (IDIter == IDRegistry.cend())
    {
        std::shared_ptr<Resource> newResource(Create(name, GetNextID()));
        Resource::ResourceID currentID = newResource->GetID();
        IDRegistry[name] = currentID;
        storage[currentID] = newResource;

        return newResource;
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