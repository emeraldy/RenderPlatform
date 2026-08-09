//-----------------------------------------------------------------
// Material Manager class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "MaterialManager.h"
#include "MaterialImporter.h"

using namespace Emerald;

MaterialManager::MaterialManager() : pImporter(nullptr)
{

}

MaterialManager::~MaterialManager()
{

}

MaterialManager& MaterialManager::GetInstance()
{
    static MaterialManager instance;

    return instance;
}

void MaterialManager::SetImporter(MaterialImporter* pImp)
{
    pImporter = pImp;
}

Resource* MaterialManager::Create(const std::wstring& name, Resource::ResourceID id, Error& err)
{
    Material* product = pImporter->LoadMaterial(name, id, err);
    if (err)
    {
        return nullptr;
    }
    else
    {
        return product;
    }
}