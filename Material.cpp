//-----------------------------------------------------------------
// Material Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------

#include "Material.h"

using namespace Emerald;

Material::Material(Resource::ResourceID id, std::wstring name) : Resource(id, name)
{

}

Material::~Material()
{

}

const size_t Material::CalculateSize()
{
    return 0;
}

RenderingPass& Material::GetPass(const std::wstring& name, Error& err)
{
    for (RenderingPass& p : passes)
    {
        if (p.GetName() == name)
        {
            return p;
        }
    }

    err += L"Failed to get rendering pass " + name + L" in " + StringUtilities::ConvertCharStringToWide(__func__) +
        L" in " + StringUtilities::ConvertCharStringToWide(__FILE__);
    return RenderingPass();
}

void Material::AddPass(const RenderingPass& newPass)
{
    passes.push_back(newPass);
}
