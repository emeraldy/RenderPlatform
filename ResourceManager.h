//-----------------------------------------------------------------
// Resource Manager base class
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <memory>
#include <unordered_map>
#include "StandardIncludes.h"
#include "Resource.h"

namespace Emerald
{
    enum class ResourceType
    {
        Default,
        Mesh,
        ShaderProgram
    };

    class ResourceManager
    {
    public:
        ResourceManager();
        virtual ~ResourceManager();

        std::shared_ptr<Resource> Load(const std::wstring& name, Error& err);//resource name must be full qualified file name

        void Unload(const std::wstring& name);
        void UnloadAll();

        ResourceType GetResourceType() const;

        size_t GetMemoryUsage() const;
        void SetMemoryBudget(size_t amount);
        size_t GetMemoryBudget() const;
    protected:
        ResourceType resourceType;
        Resource::ResourceID nextID;
        size_t memoryUsage;//TODO: memory budget management
        size_t memoryBudget;

        using NameIDRegistry = std::unordered_map<std::wstring, Resource::ResourceID>;
        using ResourceStorage = std::unordered_map<Resource::ResourceID, std::shared_ptr<Resource>>;
        NameIDRegistry IDRegistry;
        ResourceStorage storage;

        Resource::ResourceID GetNextID();

        virtual Resource* Create(const std::wstring& name, Resource::ResourceID id, Error& err) = 0;
    };
}