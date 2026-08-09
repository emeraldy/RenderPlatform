//-----------------------------------------------------------------
// Windows Material Importer Class
// 
// Derived class for importing material descripton from JSON and make them manager ready
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "MaterialImporter.h"
#include "StringUtilities.h"

namespace Emerald
{
    class WinMaterialImporter : public MaterialImporter
    {
    public:
        WinMaterialImporter();
        ~WinMaterialImporter();

        Material* LoadMaterial(const std::wstring& matName, Resource::ResourceID matID, Error& err) const override;
    };
}