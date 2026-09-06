//-----------------------------------------------------------------
// Mesh Manager class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "MeshManager.h"
#include "FbxMeshImporter.h"

using namespace Emerald;

MeshManager::MeshManager()
{
    resourceType = ResourceType::Mesh;
}

MeshManager::~MeshManager()
{

}

MeshManager& MeshManager::GetInstance()
{
    static MeshManager instance;
    
    return instance;
}

Resource* MeshManager::Create(const std::wstring& name, Resource::ResourceID id, Error& err)
{
    FbxMeshImporter importer;
    Error localError;
    localError = importer.LoadRawData(name.c_str());
    if (localError)
    {
        err += localError;
        return nullptr;
    }
    if (importer.PrepareScene())
    {
        err += L"FbxMeshImporter preparing loaded scene failed.";
        return nullptr;
    }
    const std::vector<Mesh*>& allMeshes = importer.ExtractMeshes();
    
    //TODO: in production, this function should only return one mesh as request by name. for now, only the first mesh gets returned
    // so i need to manually delete all the other meshes. 
    Resource* requestedMesh = allMeshes.front();

    for (auto cIter = allMeshes.cbegin() + 1; cIter != allMeshes.cend(); ++cIter)
    {
        delete *cIter;
    }

    return requestedMesh;
}