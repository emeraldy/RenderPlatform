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

#define NO_LAYER 0x1
#define NON_INDEXEDPOSITION 0x2
#define ILL_NORMAL 0x4
#define ILL_UV 0x8
#define ILL_BINORMAL 0x10
#define ILL_TANGENT 0x20

namespace Emerald
{
    class Mesh;
    class FbxMeshImporter : public IAssetImporter
    {
    public:
        std::vector<Mesh*> extractedMeshes;//TODO: don't delete the stored pointers yet. they are managed by MeshManager for now.

        bool LoadAssetFromFile(LPCWSTR pFileName) override;
        void ParseScene();

        FbxMeshImporter();
        virtual ~FbxMeshImporter();
    private:
        /*32 bits:
        * 31...7 6 5 4 3 2 1 0
        * 
        * 31: reserved
        * .
        * .
        * .
        * 7: reserved
        * 6: reserved
        * 5: ILL_TANGENT
        * 4: ILL_BINORMAL
        * 3: ILL_UV
        * 2: ILL_NORMAL
        * 1: NON_INDEXEDPOSITION
        * 0: NO_LAYER
        */
        unsigned int anomalyFlags;

        FbxSdkCommon fbxSdk;
        size_t serial;

        void ExtractMesh(FbxNode* pNode);
        size_t GetNextSerial();
        void ClearAnomaly();
    };
}