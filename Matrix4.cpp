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
    assert(0 <= row && row < 4);
    return Vector4(m[row], m[row + 4], m[row + 8], m[row + 12]);
}

//0-indexed row
void Matrix4::SetRow(size_t row, const Vector4& values)
{
    assert(0 <= row && row < 4);
    m[row] = values[0];
    m[row + 4] = values[1];
    m[row + 8] = values[2];
    m[row + 12] = values[3];
}

Matrix3 Matrix4::SubMatrix3(size_t row, size_t col) const
{
    assert(0 <= row && row < 4);
    assert(0 <= col && col < 4);

    Matrix3 result;

    size_t currentRow = 0;
    size_t currentCol = 0;
    size_t resultIndex = 0;
    for (int i = 0; i < 16; i++)
    {
        currentRow = i % 4;
        currentCol = i / 4;
        if (currentRow != row && currentCol != col)
        {
            result[resultIndex] = m[i];
            resultIndex++;
        }
    }

    return result;
}

Matrix4 Matrix4::Transpose() const
{
    Matrix4 result;

    result[0] = m[0];
    result[1] = m[4];
    result[2] = m[8];
    result[3] = m[12];
    result[4] = m[1];
    result[5] = m[5];
    result[6] = m[9];
    result[7] = m[13];
    result[8] = m[2];
    result[9] = m[6];
    result[10] = m[10];
    result[11] = m[14];
    result[12] = m[3];
    result[13] = m[7];
    result[14] = m[11];
    result[15] = m[15];

    return result;
}



float Matrix4::operator [] (size_t index) const
{
    assert(0 <= index && index < 16);
    return m[index];
}

float& Matrix4::operator [] (size_t index)
{
    assert(0 <= index && index < 16);
    return m[index];
}
