//-----------------------------------------------------------------
// 3x3 Matrix Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Matrix3.h"

using namespace Emerald;

//0-indexed column
Vector3 Matrix3::GetColumn(size_t col) const
{
    assert(0 <= col && col < 3);
    return Vector3(m[col * 3], m[col * 3 + 1], m[col * 3 + 2]);
}

//0-indexed column
void Matrix3::SetColumn(size_t col, const Vector3& values)
{
    assert(0 <= col && col < 3);
    m[col * 3] = values[0];
    m[col * 3 + 1] = values[1];
    m[col * 3 + 2] = values[2];
}

void Matrix3::BuildFromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
    SetColumn(0, xAxis);
    SetColumn(1, yAxis);
    SetColumn(2, zAxis);
}

Matrix3 Matrix3::Transpose() const
{
    Matrix3 result;

    result[0] = m[0];
    result[1] = m[3];
    result[2] = m[6];
    result[3] = m[1];
    result[4] = m[4];
    result[5] = m[7];
    result[6] = m[2];
    result[7] = m[5];
    result[8] = m[8];

    return result;
}

float Matrix3::operator[](size_t index) const
{
    assert(0 <= index && index < 9);
    return m[index];
}

float& Matrix3::operator[](size_t index)
{
    assert(0 <= index && index < 9);
    return m[index];
}