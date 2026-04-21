//-----------------------------------------------------------------
// Mesh Manager class
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"
#include "ResourceManager.h"
#include "Mesh.h"

namespace Emerald
{
    class MeshManager : public ResourceManager
    {
    public:
        MeshManager(const MeshManager&) = delete;
        MeshManager operator = (const MeshManager&) = delete;

        ~MeshManager();

        static MeshManager& GetInstance();

    protected:
        virtual Resource* Create(const std::wstring& name, Resource::ResourceID id, Error& err) override;

    private:
        MeshManager() {}
    };
}