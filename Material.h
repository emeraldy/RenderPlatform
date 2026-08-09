//-----------------------------------------------------------------
// Material Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Resource.h"
#include "RenderingPass.h"
#include <vector>

namespace Emerald
{
    class Material : public Resource
    {
    private:
        std::vector<RenderingPass> passes;

    public:
        Material(Resource::ResourceID id, std::wstring name = L"Default");
        virtual ~Material();

        const size_t CalculateSize() override;
        RenderingPass& GetPass(const std::wstring& name, Error& err);
        void AddPass(const RenderingPass& newPass);
    };
}