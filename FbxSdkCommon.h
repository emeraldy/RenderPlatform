//-----------------------------------------------------------------
// Perform initialisation, scene loading and destruction of sdk
// objects that are common to all fbx importers. 
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "fbxsdk.h"

namespace Emerald
{
    class FbxSdkCommon
    {
    public:
        FbxManager* GetManager() { return pManager; }
        FbxScene*   GetScene() { return pScene; }

        bool LoadScene(const char* pFilename);
        void ConvertCoordinateSystem(FbxAxisSystem& ourSystem);
        void ConvertUnitSystem(const FbxSystemUnit& ourUnit);

        FbxSdkCommon();
        virtual ~FbxSdkCommon();
    private:
        FbxManager* pManager;
        FbxScene*   pScene;
    };
}
