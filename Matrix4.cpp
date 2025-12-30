//-----------------------------------------------------------------
// 4x4 Matrix Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Matrix4.h"

using namespace Emerald;

const Matrix4 Matrix4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

Matrix4 Matrix4::MakeTranslation(const Vector3& vec)
{
    Matrix4 result = IDENTITY;
    Vector4 values(vec, 1.0f);
    result.SetColumn(3, values);

    return result;
}

Matrix4 Matrix4::MakeScale(const Vector3& vec)
{
    Matrix4 result = IDENTITY;
    result[0] = vec.x;
    result[5] = vec.y;
    result[10] = vec.z;

    return result;
}

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

Matrix3 Matrix4::Retrieve3x3() const
{
    return SubMatrix3(3, 3);
}

Vector3 Matrix4::GetTranslation() const
{
    return Vector3(m[12], m[13], m[14]);
}

Vector3 Matrix4::GetScale() const
{
    return Vector3(m[0], m[5], m[10]);
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

float Matrix4::Determinant() const
{
    Matrix3 sub00 = SubMatrix3(0, 0);
    Matrix3 sub10 = SubMatrix3(1, 0);
    Matrix3 sub20 = SubMatrix3(2, 0);
    Matrix3 sub30 = SubMatrix3(3, 0);

    float det00 = sub00.Determinant();
    float det10 = sub10.Determinant();
    float det20 = sub20.Determinant();
    float det30 = sub30.Determinant();

    return m[0] * det00 - m[1] * det10 + m[2] * det20 - m[3] * det30;
}

bool Matrix4::Inverse(Matrix4& result, float tolerance) const
{
    //Adjoint method: inv(M) = (1 / det(M)) * adj(M)

    //adj(M):
    Matrix3 minors[4][4];
    float minorDets[4][4];
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            minors[row][col] = SubMatrix3(row, col);
            minorDets[row][col] = minors[row][col].Determinant();
        }
    }
    Matrix4 adjugate(minorDets[0][0], -minorDets[0][1], minorDets[0][2], -minorDets[0][3],
                     -minorDets[1][0], minorDets[1][1], -minorDets[1][2], minorDets[1][3],
                     minorDets[2][0], -minorDets[2][1], minorDets[2][2], -minorDets[2][3],
                     -minorDets[3][0], minorDets[3][1], -minorDets[3][2], minorDets[3][3]);
    //det(M)
    float det = m[0] * minorDets[0][0] - m[1] * minorDets[1][0] + m[2] * minorDets[2][0] - m[3] * minorDets[3][0];
    if (fabs(det) < tolerance)
    {
        return false;
    }

    result = adjugate * (1.0 / det);

    return true;
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

Matrix4 Matrix4::operator + (const Matrix4& other) const
{
    Matrix4 sum;
    for (int i = 0; i < 16; i++)
    {
        sum[i] = m[i] + other[i];
    }

    return sum;
}

Matrix4 Matrix4::operator - (const Matrix4& other) const
{
    Matrix4 difference;
    for (int i = 0; i < 16; i++)
    {
        difference[i] = m[i] - other[i];
    }

    return difference;
}

Matrix4 Matrix4::operator * (const Matrix4& other) const
{
    Matrix4 product;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            product[row + col * 4] = m[row] * other[col * 4] + m[row + 4] * other[col * 4 + 1] + m[row + 8] * other[col * 4 + 2] + m[row + 12] * other[col * 4 + 3];
        }
    }

    return product;
}

Vector4 Matrix4::operator * (const Vector4& vec) const
{
    Vector4 product;

    for (int row = 0; row < 4; row++)
    {
        product[row] = m[row] * vec[0] + m[row + 4] * vec[1] + m[row + 8] * vec[2] + m[row + 12] * vec[3];
    }

    return product;
}

Matrix4 Matrix4::operator * (float scalar) const
{
    Matrix4 result;
    for (size_t i = 0; i < 16; i++)
    {
        result[i] = m[i] * scalar;
    }

    return result;
}

namespace Emerald
{
    Matrix4 operator * (float scalar, const Matrix4& mat)
    {
        return mat * scalar;
    }
}

bool Matrix4::operator == (const Matrix4& other) const
{
    for (int i = 0; i < 16; i++)
    {
        if (m[i] != other[i])
        {
            return false;
        }
    }

    return true;
}

bool Matrix4::operator != (const Matrix4& other) const
{
    return !operator == (other);
}

namespace Emerald
{
    std::ostream& operator << (std::ostream& o, const Matrix4& mat)
    {
        o << mat[0] << "  " << mat[4] << "  " << mat[8] << "  "  << mat[12] << "\n"
          << mat[1] << "  " << mat[5] << "  " << mat[9] << "  "  << mat[13] << "\n"
          << mat[2] << "  " << mat[6] << "  " << mat[10] << "  " << mat[14] << "\n"
          << mat[3] << "  " << mat[7] << "  " << mat[11] << "  " << mat[15];

        o << "\n" << "\n";

        return o;
    }
}