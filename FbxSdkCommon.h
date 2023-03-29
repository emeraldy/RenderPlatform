//-----------------------------------------------------------------
// Perform initialisation, scene loading and destruction of sdk
// objects that are common to all fbx importers. 
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "fbxsdk.h"
#include <windows.h>
#include "StringUtilities.h"


namespace Emerald
{
    class FbxSdkCommon
    {
    public:
        void InitializeSdkObjects();
        void DestroySdkObjects();
        bool LoadScene(const char* pFilename);
    private:
        FbxManager* pManager;
        FbxScene*   pScene;
    };
}
