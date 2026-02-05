//-----------------------------------------------------------------
// Import mesh data from an fbx file and make it engine ready
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "FbxMeshImporter.h"
#include "StringUtilities.h"

using namespace Emerald;

FbxMeshImporter::FbxMeshImporter()
{

}

FbxMeshImporter::~FbxMeshImporter()
{

}

bool FbxMeshImporter::LoadAssetFromFile(LPCWSTR pFileName)
{
    FbxScene* pImportedScene = nullptr;
    std::string charFileName = StringUtilities::ConvertWideStringToChar(std::wstring(pFileName));
    if (!fbxSdk.LoadScene(charFileName.c_str()))
    {
        return false;
    }

    return true;
}

void FbxMeshImporter::ParseScene()
{
    FbxManager* pManager = fbxSdk.GetManager();
    if (!pManager)
    {
        return;
    }
    FbxScene* pScene = fbxSdk.GetScene();
    if (!pScene)
    {
        return;
    }

    FbxGeometryConverter lGeomConverter(pManager);
    if (!lGeomConverter.Triangulate(pScene, true))
    {
        MessageBox(NULL, TEXT("Triangulating meshes failed!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    ExtractMesh(pScene->GetRootNode());
}

void FbxMeshImporter::ExtractMesh(FbxNode* pNode)
{
    FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
    if (pNodeAttribute)
    {
        if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            FbxMesh* pMesh = pNode->GetMesh();
            int triangleCount = pMesh->GetPolygonCount();
            int controlPointCount = pMesh->GetControlPointsCount();
            if (triangleCount * 3 != controlPointCount)
            {
                MessageBox(NULL, TEXT("Triangle vertices are not equal to control points!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
            }
            FbxVector4* controlPoints = pMesh->GetControlPoints();
        }
    }

    const int childNodeCount = pNode->GetChildCount();
    for (int childIndex = 0; childIndex < childNodeCount; ++childIndex)
    {
        ExtractMesh(pNode->GetChild(childIndex));
    }
}
