//-----------------------------------------------------------------
// 4x4 Matrix Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Matrix4.h"

using namespace Emerald;

const Matrix4 Matrix4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

//0-indexed column
Vector4 Matrix4::GetColumn(size_t col) const
{
    assert(0 <= col && col < 4);
    return Vector4(m[col * 4], m[col * 4 + 1], m[col * 4 + 2], m[col * 4 + 3]);
}

//0-indexed column
void Matrix4::SetColumn(size_t col, const Vector4& values)
{
    assert(0 <= col && col < 4);
    m[col * 4] = values[0];
    m[col * 4 + 1] = values[1];
    m[col * 4 + 2] = values[2];
    m[col * 4 + 3] = values[3];
}

//0-indexed row
Vector4 Matrix4::GetRow(size_t row) const
{
    assert(0 <= row && row < 3);
    return Vector4(m[row], m[row + 3], m[row + 6]);
}

//0-indexed row
void Matrix4::SetRow(size_t row, const Vector4& values)
{
    assert(0 <= row && row < 3);
    m[row] = values[0];
    m[row + 3] = values[1];
    m[row + 6] = values[2];
}

