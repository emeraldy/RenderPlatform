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
    class IAssetLoader
    {
    public:
        virtual bool LoadAsset(const BYTE* rawData, const unsigned int size) = 0;
    };
}