//-----------------------------------------------------------------
// Loader for each game asset type should inherit from this class
// and provide necessary implmentations. 
//-----------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <windows.h>

namespace Emerald
{
    class IAssetImporter
    {
    public:
        virtual bool LoadAssetFromFile(LPCWSTR pFileName) = 0;
    };
}