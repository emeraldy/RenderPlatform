//-----------------------------------------------------------------
// Material Importer Base Class
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Material.h"

namespace Emerald
{
    class MaterialImporter
    {
    public:
        virtual Material* LoadMaterial(const std::wstring& name, Resource::ResourceID id, Error& err) const = 0;
    };
}