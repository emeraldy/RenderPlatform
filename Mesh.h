//-----------------------------------------------------------------
// Mesh Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "StandardIncludes.h"
#include "Resource.h"

namespace Emerald
{
    class Vector2;
    class Vector3;
    struct VertexData
    {
        Vector3* pPositions;
        Vector3* pNormals;
        Vector3* pBinormals;
        Vector3* pTangents;
        Vector2* pUVs;

        unsigned int size;

        VertexData()
        {
            pPositions = nullptr;
            pNormals = nullptr;
            pBinormals = nullptr;
            pTangents = nullptr;
            pUVs = nullptr;
            size = 0;
        }
        ~VertexData()
        {
            SAFE_DELETEARRAY(pPositions);
            SAFE_DELETEARRAY(pNormals);
            SAFE_DELETEARRAY(pBinormals);
            SAFE_DELETEARRAY(pTangents);
            SAFE_DELETEARRAY(pUVs);
        }
    };

    class Mesh : public Resource
    {
    public:
        using VertexDataIndex = unsigned int;

        Mesh(Resource::ResourceID id, std::wstring name = L"Default");
        ~Mesh();

        unsigned int GetVertexDataSize() const;
        Vector3* const GetPositions() const;
        Vector3* const GetNormals() const;
        Vector3* const GetBinormals() const;
        Vector3* const GetTangents() const;
        Vector2* const GetUVs() const;
        const VertexData GetVertextData() const;

        void SetPositions(Vector3* data, unsigned int size, Error& err);
        void SetNormals(Vector3* data, unsigned int size, Error& err);
        void SetBinormals(Vector3* data, unsigned int size, Error& err);
        void SetTangents(Vector3* data, unsigned int size, Error& err);
        void SetUVs(Vector2* data, unsigned int size, Error& err);

        virtual const size_t CalculateSize() override;
    private:
        VertexData vertexData;
        VertexDataIndex* indices;

        //Submesh*
    };
}