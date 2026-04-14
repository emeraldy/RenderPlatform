//-----------------------------------------------------------------
// Mesh Manager class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "MeshManager.h"
#include "FbxMeshImporter.h"

using namespace Emerald;

MeshManager::~MeshManager()
{

}

MeshManager& MeshManager::GetInstance()
{
    static MeshManager instance;
    
    return instance;
}

Resource* MeshManager::Create(const std::wstring& name, Resource::ResourceID id)
{
    FbxMeshImporter importer;
    importer.LoadAssetFromFile(name.c_str());
    importer.ParseScene();
    return nullptr;
}