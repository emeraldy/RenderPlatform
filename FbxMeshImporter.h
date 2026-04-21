//-----------------------------------------------------------------
// Import mesh data from an fbx file and make it engine ready
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <vector>
#include "RawDataReader.h"
#include "FbxSdkCommon.h"
#include "StringUtilities.h"

#define NO_LAYER 0x1
#define NON_INDEXEDPOSITION 0x2
#define ILL_NORMAL 0x4
#define ILL_UV 0x8
#define ILL_BINORMAL 0x10
#define ILL_TANGENT 0x20
#define ILL_MESH 0x40

namespace Emerald
{
    class Mesh;
    class FbxMeshImporter : public RawDataReader
    {
    public:
        virtual Error LoadRawData(const std::wstring& sourceName) override;
        virtual const unsigned char* GetBuffer() const override;
        virtual unsigned int GetBufferSize() const override;
        virtual void ClearBuffer() override;
        bool PrepareScene();
        const std::vector<Mesh*>& ExtractMeshes();

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
        * 8: reserved
        * 7: reserved
        * 6: ILL_MESH
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
        std::vector<Mesh*> extractedMeshes;//TODO: don't delete the stored pointers in destructor yet. they are managed by MeshManager for now.


        void ExtractMesh(FbxNode* pNode);
        size_t GetNextSerial();
        void ClearAnomaly();
    };
}