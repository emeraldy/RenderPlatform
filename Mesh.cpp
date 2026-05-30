//-----------------------------------------------------------------
// Mesh Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Mesh.h"
#include "Vector2.h"
#include "Vector3.h"

using namespace Emerald;

Mesh::Mesh(Resource::ResourceID id, std::wstring name) : pIndices(nullptr), Resource(id, name)
{

}

Mesh::~Mesh()
{
    SAFE_DELETEARRAY(pIndices);
}

unsigned int Mesh::GetVertexDataSize() const
{
    return size;
}

Vector3* const Mesh::GetPositions() const
{
    return vertexData.pPositions;
}

Vector3* const Mesh::GetNormals() const
{
    return vertexData.pNormals;
}

Vector3* const Mesh::GetBinormals() const
{
    return vertexData.pBinormals;
}

Vector3* const Mesh::GetTangents() const
{
    return vertexData.pTangents;
}

Vector2* const Mesh::GetUVs() const
{
    return vertexData.pUVs;
}

const VertexData Mesh::GetVertextData() const
{
    return vertexData;
}

void Mesh::SetPositions(Vector3* data, unsigned int size, Error& err)
{
    if (vertexData.size == 0)
    {
        vertexData.size = size;
        SAFE_DELETEARRAY(vertexData.pPositions);
        vertexData.pPositions = data;
    }
    else
    {
        if (size != vertexData.size)
        {
            err += L"Mesh GetPositions size mismatches.";
        }
        else
        {
            SAFE_DELETEARRAY(vertexData.pPositions);
            vertexData.pPositions = data;
        }
    }
}

void Mesh::SetNormals(Vector3* data, unsigned int size, Error& err)
{
    if (vertexData.size == 0)
    {
        vertexData.size = size;
        SAFE_DELETEARRAY(vertexData.pNormals);
        vertexData.pNormals = data;
    }
    else
    {
        if (size != vertexData.size)
        {
            err += L"Mesh GetNormals size mismatches.";
        }
        else
        {
            SAFE_DELETEARRAY(vertexData.pNormals);
            vertexData.pNormals = data;
        }
    }
}

void Mesh::SetBinormals(Vector3* data, unsigned int size, Error& err)
{
    if (vertexData.size == 0)
    {
        vertexData.size = size;
        SAFE_DELETEARRAY(vertexData.pBinormals);
        vertexData.pBinormals = data;
    }
    else
    {
        if (size != vertexData.size)
        {
            err += L"Mesh GetBinormals size mismatches.";
        }
        else
        {
            SAFE_DELETEARRAY(vertexData.pBinormals);
            vertexData.pBinormals = data;
        }
    }
}

void Mesh::SetTangents(Vector3* data, unsigned int size, Error& err)
{
    if (vertexData.size == 0)
    {
        vertexData.size = size;
        SAFE_DELETEARRAY(vertexData.pTangents);
        vertexData.pTangents = data;
    }
    else
    {
        if (size != vertexData.size)
        {
            err += L"Mesh GetTangents size mismatches.";
        }
        else
        {
            SAFE_DELETEARRAY(vertexData.pTangents);
            vertexData.pTangents = data;
        }
    }
}

void Mesh::SetUVs(Vector2* data, unsigned int size, Error& err)
{
    if (vertexData.size == 0)
    {
        vertexData.size = size;
        SAFE_DELETEARRAY(vertexData.pUVs);
        vertexData.pUVs = data;
    }
    else
    {
        if (size != vertexData.size)
        {
            err += L"Mesh GetUVs size mismatches.";
        }
        else
        {
            SAFE_DELETEARRAY(vertexData.pUVs);
            vertexData.pUVs = data;
        }
    }
}

const size_t Mesh::CalculateSize()
{
    const size_t scalarSize = sizeof(Vector2::x);
    size = 0;

    if (vertexData.pPositions == nullptr || vertexData.pNormals == nullptr || vertexData.size == 0)
    {
        return size;
    }
    //sum up positions and normals
    size += 3 * scalarSize * vertexData.size * 2;
    if (vertexData.pBinormals != nullptr)
    {
        size += 3 * scalarSize * vertexData.size;
    }
    if (vertexData.pTangents != nullptr)
    {
        size += 3 * scalarSize * vertexData.size;
    }
    if (vertexData.pUVs != nullptr)
    {
        size += 2 * scalarSize * vertexData.size;
    }

    return size;
}