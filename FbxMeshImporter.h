//-----------------------------------------------------------------
// Import mesh data from an fbx file and make it engine ready
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <vector>
#include "IAssetImporter.h"
#include "FbxSdkCommon.h"
#include "StringUtilities.h"

namespace Emerald
{
    class Mesh;
    class FbxMeshImporter : public IAssetImporter
    {
    public:
        std::vector<Mesh*> pExtractedMeshes;

        bool LoadAssetFromFile(LPCWSTR pFileName) override;
        void ParseScene();

        FbxMeshImporter();
        virtual ~FbxMeshImporter();
    private:
        FbxSdkCommon fbxSdk;

        void ExtractMesh(FbxNode* pNode);
    };
}