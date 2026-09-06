//-----------------------------------------------------------------
// Material Manager class
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "ResourceManager.h"
#include "Material.h"

namespace Emerald
{
    class MaterialImporter;
    class MaterialManager : public ResourceManager
    {
    public:
        MaterialManager(const MaterialManager& source) = delete;
        MaterialManager operator = (const MaterialManager& source) = delete;
        virtual ~MaterialManager();

        static MaterialManager& GetInstance();
        void SetImporter(MaterialImporter* pImp);

    protected:
        virtual Resource* Create(const std::wstring& name, Resource::ResourceID id, Error& err) override;

    private:
        MaterialManager();

        MaterialImporter* pImporter;
    };
}