//-----------------------------------------------------------------
// Import mesh data from an fbx file and make it engine ready
//-----------------------------------------------------------------


//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <memory>
#include <windows.h>
#include "FbxMeshImporter.h"
#include "StringUtilities.h"
#include "Mesh.h"
#include "Vector2.h"
#include "Vector3.h"

using namespace Emerald;

FbxMeshImporter::FbxMeshImporter()
{
    serial = 1;
    ClearAnomaly();
}

FbxMeshImporter::~FbxMeshImporter()
{

}

size_t FbxMeshImporter::GetNextSerial()
{
    return serial++;
}

void FbxMeshImporter::ClearAnomaly()
{
    anomalyFlags = '\0';
}

Error FbxMeshImporter::LoadRawData(const std::wstring& sourceName)
{
    Error loadError;
    FbxScene* pImportedScene = nullptr;
    std::string charFileName = StringUtilities::ConvertWideStringToChar(std::wstring(sourceName));
    if (!fbxSdk.LoadSceneFromFile(charFileName.c_str()))
    {
        loadError += L"Loading FBX file error.";
    }

    return loadError;
}

bool FbxMeshImporter::PrepareScene()
{
    FbxManager* pManager = fbxSdk.GetManager();
    if (!pManager)
    {
        return false;
    }
    FbxScene* pScene = fbxSdk.GetScene();
    if (!pScene)
    {
        return false;
    }

    fbxSdk.ConvertUnitSystem(FbxSystemUnit::m);
    fbxSdk.ConvertCoordinateSystem(FbxAxisSystem::DirectX);
    FbxGeometryConverter lGeomConverter(pManager);
    if (!lGeomConverter.Triangulate(pScene, true))
    {
        MessageBox(NULL, TEXT("Triangulating meshes failed!"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
        return false;
    }

    return true;
}

const std::vector<Mesh*>& FbxMeshImporter::ExtractMeshes()
{
    ExtractMesh(fbxSdk.GetScene()->GetRootNode());

    return extractedMeshes;
}

void FbxMeshImporter::ExtractMesh(FbxNode* pNode)
{
    Error localError;
    FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
    if (pNodeAttribute && pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        size_t id = GetNextSerial();
        Mesh* pMyMesh = new Mesh(id, std::to_wstring(id));
        FbxMesh* pFbxMesh = pNode->GetMesh();
        Vector3* pPositions = nullptr;
        Vector3* pNormals = nullptr;
        Vector3* pBinormals = nullptr;
        Vector3* pTangents = nullptr;
        Vector2* pUVs = nullptr;

        const int triangleCount = pFbxMesh->GetPolygonCount();
        const int controlPointCount = pFbxMesh->GetControlPointsCount();
        if (triangleCount <= 0 || controlPointCount <= 0)
        {
            anomalyFlags |= ILL_MESH;
            goto ANOMALYCLEANUP;
        }
        if (triangleCount * 3 == controlPointCount)
        {
            anomalyFlags |= NON_INDEXEDPOSITION;
            goto ANOMALYCLEANUP;
        }

        //extract positions
        FbxVector4* controlPoints = pFbxMesh->GetControlPoints();
        pPositions = new Vector3[controlPointCount];
        for (int i = 0; i < controlPointCount; i++)
        {
            pPositions[i].x = controlPoints[i][0];
            pPositions[i].y = controlPoints[i][1];
            pPositions[i].z = controlPoints[i][2];
        }
        pMyMesh->SetPositions(pPositions, controlPointCount, localError);
        if (localError)
        {
            goto ANOMALYCLEANUP;
        }

        //extract normals
        FbxLayer* pLayer = pFbxMesh->GetLayer(0);
        if (!pLayer)
        {
            anomalyFlags |= NO_LAYER;
            goto ANOMALYCLEANUP;
        }
        FbxLayerElementNormal* pFbxNormals = pLayer->GetNormals();
        if (!pFbxNormals || pFbxNormals->GetMappingMode() != FbxLayerElement::eByControlPoint)
        {
            anomalyFlags |= ILL_NORMAL;
            goto ANOMALYCLEANUP;
        }
        bool isIndirect = pFbxNormals->GetReferenceMode() == FbxLayerElement::eIndexToDirect;
        pNormals = new Vector3[controlPointCount];
        for (int i = 0; i < controlPointCount; i++)
        {
            int index = i;
            if (isIndirect)
            {
                //i is index to the layer element's index array instead
                index = pFbxNormals->GetIndexArray().GetAt(index);
            }
            pNormals[i].x = (pFbxNormals->GetDirectArray().GetAt(index))[0];
            pNormals[i].y = (pFbxNormals->GetDirectArray().GetAt(index))[1];
            pNormals[i].z = (pFbxNormals->GetDirectArray().GetAt(index))[2];
        }
        pMyMesh->SetNormals(pNormals, controlPointCount, localError);
        if (localError)
        {
            goto ANOMALYCLEANUP;
        }

        //extract uvs, optional
        FbxLayerElementUV* pFbxUVs = pLayer->GetUVs();
        if (pFbxUVs)
        {
            if (pFbxUVs->GetMappingMode() != FbxLayerElement::eByControlPoint)
            {
                anomalyFlags |= ILL_UV;
                goto ANOMALYCLEANUP;
            }
            isIndirect = pFbxUVs->GetReferenceMode() == FbxLayerElement::eIndexToDirect;
            pUVs = new Vector2[controlPointCount];
            for (int i = 0; i < controlPointCount; i++)
            {
                int index = i;
                if (isIndirect)
                {
                    //i is index to the layer element's index array instead
                    index = pFbxUVs->GetIndexArray().GetAt(index);
                }
                pUVs[i].x = (pFbxUVs->GetDirectArray().GetAt(index))[0];
                pUVs[i].y = (pFbxUVs->GetDirectArray().GetAt(index))[1];
            }
            pMyMesh->SetUVs(pUVs, controlPointCount, localError);
            if (localError)
            {
                goto ANOMALYCLEANUP;
            }
        }

        //extract binormals, optional
        FbxLayerElementBinormal* pFbxBinormals = pLayer->GetBinormals();
        if (pFbxBinormals)
        {
            if (pFbxBinormals->GetMappingMode() != FbxLayerElement::eByControlPoint)
            {
                anomalyFlags |= ILL_BINORMAL;
                goto ANOMALYCLEANUP;
            }
            isIndirect = pFbxBinormals->GetReferenceMode() == FbxLayerElement::eIndexToDirect;
            pBinormals = new Vector3[controlPointCount];
            for (int i = 0; i < controlPointCount; i++)
            {
                int index = i;
                if (isIndirect)
                {
                    //i is index to the layer element's index array instead
                    index = pFbxBinormals->GetIndexArray().GetAt(index);
                }
                pBinormals[i].x = (pFbxBinormals->GetDirectArray().GetAt(index))[0];
                pBinormals[i].y = (pFbxBinormals->GetDirectArray().GetAt(index))[1];
                pBinormals[i].z = (pFbxBinormals->GetDirectArray().GetAt(index))[2];
            }
            pMyMesh->SetBinormals(pBinormals, controlPointCount, localError);
            if (localError)
            {
                goto ANOMALYCLEANUP;
            }
        }

        //extract tangent, optional
        FbxLayerElementTangent* pFbxTangents = pLayer->GetTangents();
        if (pFbxTangents)
        {
            if (pFbxTangents->GetMappingMode() != FbxLayerElement::eByControlPoint)
            {
                anomalyFlags |= ILL_TANGENT;
                goto ANOMALYCLEANUP;
            }
            isIndirect = pFbxTangents->GetReferenceMode() == FbxLayerElement::eIndexToDirect;
            pTangents = new Vector3[controlPointCount];
            for (int i = 0; i < controlPointCount; i++)
            {
                int index = i;
                if (isIndirect)
                {
                    //i is index to the layer element's index array instead
                    index = pFbxTangents->GetIndexArray().GetAt(index);
                }
                pTangents[i].x = (pFbxTangents->GetDirectArray().GetAt(index))[0];
                pTangents[i].y = (pFbxTangents->GetDirectArray().GetAt(index))[1];
                pTangents[i].z = (pFbxTangents->GetDirectArray().GetAt(index))[2];
            }
            pMyMesh->SetTangents(pTangents, controlPointCount, localError);
            if (localError)
            {
                goto ANOMALYCLEANUP;
            }
        }
        
        pMyMesh->CalculateSize();
        pMyMesh->SetIsLoaded(true);

        extractedMeshes.push_back(pMyMesh);

        ANOMALYCLEANUP:
        std::wstring errorText(localError.GetErrorText());
        if (anomalyFlags & ILL_MESH)
        {
            errorText += L"Mesh is ill formed.\n";
        }
        if (anomalyFlags & NON_INDEXEDPOSITION)
        {
            errorText += L"Mesh vertices are not indexed.\n";
        }
        if (anomalyFlags & NO_LAYER)
        {
            errorText += L"Mesh has no layer.\n";
        }
        if (anomalyFlags & ILL_NORMAL)
        {
            errorText += L"Mesh normals are ill formed.\n";
        }
        if (anomalyFlags & ILL_UV)
        {
            errorText += L"Mesh uvs are ill formed.\n";
        }
        if (anomalyFlags & ILL_BINORMAL)
        {
            errorText += L"Mesh binormals are ill formed.\n";
        }
        if (anomalyFlags & ILL_TANGENT)
        {
            errorText += L"Mesh tangents are ill formed.\n";
        }
        if (errorText.length() != 0)
        {
            MessageBox(NULL, errorText.c_str(), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
            SAFE_DELETEARRAY(pPositions);
            SAFE_DELETEARRAY(pNormals);
            SAFE_DELETEARRAY(pBinormals);
            SAFE_DELETEARRAY(pTangents);
            SAFE_DELETEARRAY(pUVs);
            SAFE_DELETE(pMyMesh);
        }
        ClearAnomaly();
    }

    const int childNodeCount = pNode->GetChildCount();
    for (int childIndex = 0; childIndex < childNodeCount; ++childIndex)
    {
        ExtractMesh(pNode->GetChild(childIndex));
    }
}


const unsigned char* FbxMeshImporter::GetBuffer() const
{
    MessageBox(NULL, TEXT("FbxMeshImporter::GetBuffer() not implemented"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
    return nullptr;
}

unsigned int FbxMeshImporter::GetBufferSize() const
{
    MessageBox(NULL, TEXT("FbxMeshImporter::GetBufferSize() not implemented"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
    return 0;
}

void FbxMeshImporter::ClearBuffer()
{
    MessageBox(NULL, TEXT("FbxMeshImporter::GetBuffer() not implemented"), TEXT("INFORMATION"), MB_OK | MB_ICONEXCLAMATION);
}