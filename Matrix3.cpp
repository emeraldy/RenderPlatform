//-----------------------------------------------------------------
// 3x3 Matrix Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Matrix3.h"

using namespace Emerald;

const Matrix3 Matrix3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

//0-indexed column
Vector3 Matrix3::GetColumn(size_t col, Error& err) const
{
    if (col < 0 || col > 2)
    {
        err += L"Matrix3 GetColumn out of range.";
        return Vector3();
    }
    return Vector3(m[col * 3], m[col * 3 + 1], m[col * 3 + 2]);
}

//0-indexed column
void Matrix3::SetColumn(size_t col, const Vector3& values, Error& err)
{
    if (col < 0 || col > 2)
    {
        err += L"Matrix3 SetColumn out of range.";
        return;
    }
    m[col * 3] = values[0];
    m[col * 3 + 1] = values[1];
    m[col * 3 + 2] = values[2];
}

//0-indexed row
Vector3 Matrix3::GetRow(size_t row, Error& err) const
{
    if (row < 0 || row > 2)
    {
        err += L"Matrix3 GetRow out of range.";
        return Vector3();
    }
    return Vector3(m[row], m[row + 3], m[row + 6]);
}

//0-indexed row
void Matrix3::SetRow(size_t row, const Vector3& values, Error& err)
{
    if (row < 0 || row > 2)
    {
        err += L"Matrix3 SetRow out of range.";
        return;
    }
    m[row] = values[0];
    m[row + 3] = values[1];
    m[row + 6] = values[2];
}

void Matrix3::BuildFromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis, Error& err)
{
    SetColumn(0, xAxis, err);
    SetColumn(1, yAxis, err);
    SetColumn(2, zAxis, err);
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

float Matrix3::Determinant() const
{
    return m[0] * (m[4] * m[8] - m[7] * m[5]) +
           m[3] * (m[7] * m[2] - m[1] * m[8]) +
           m[6] * (m[1] * m[5] - m[4] * m[2]);
}

Matrix3 Matrix3::Inverse(Error& err, float tolerance) const
{
    Matrix3 result;
    //Adjoint method: inv(M) = (1 / det(M)) * adj(M)
    
    //adj(M):
    result[0] = m[4] * m[8] - m[7] * m[5];
    result[3] = m[6] * m[5] - m[3] * m[8];
    result[6] = m[3] * m[7] - m[6] * m[4];
    result[1] = m[7] * m[2] - m[1] * m[8];
    result[4] = m[0] * m[8] - m[6] * m[2];
    result[7] = m[6] * m[1] - m[0] * m[7];
    result[2] = m[1] * m[5] - m[4] * m[2];
    result[5] = m[3] * m[2] - m[0] * m[5];
    result[8] = m[0] * m[4] - m[3] * m[1];

    //det(M):
    float det = m[0] * result[0] + m[3] * result[1] + m[6] * result[2];

    if (fabs(det) < tolerance)
    {
        err += L"Matrix3 Inverse failed.";
        return result;
    }
    
    result = result * (1.0 / det);

    return result;
}

void Matrix3::Orthogonalise(Error& err)
{
    Vector3 row0, row1, row2;
    row0 = GetRow(0, err);
    row1 = GetRow(1, err);
    row2 = GetRow(2, err);

    row0 = row0.Normalise(err);
    row1 = row1 - row1.Dot(row0) * row0;
    row1 = row1.Normalise(err);
    row2 = row2 - row2.Dot(row0) * row0 - row2.Dot(row1) * row1;
    row2 = row2.Normalise(err);

    SetRow(0, row0, err);
    SetRow(1, row1, err);
    SetRow(2, row2, err);
}

float Matrix3::operator [] (size_t index) const
{
    if (index < 0 || index > 8)
    {
        throw std::out_of_range("at Matrix3 operator [] const");
    }
    return m[index];
}

float& Matrix3::operator [] (size_t index)
{
    if (index < 0 || index > 8)
    {
        throw std::out_of_range("at Matrix3 operator []");
    }
    return m[index];
}

Matrix3 Matrix3::operator + (const Matrix3& other) const
{
    Matrix3 sum;
    for (int i = 0; i < 9; i++)
    {
        sum[i] = m[i] + other[i];
    }

    return sum;
}

Matrix3 Matrix3::operator - (const Matrix3& other) const
{
    Matrix3 difference;
    for (int i = 0; i < 9; i++)
    {
        difference[i] = m[i] - other[i];
    }

    return difference;
}

Matrix3 Matrix3::operator * (const Matrix3& other) const
{
    Matrix3 product;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            product[row + col * 3] = m[row] * other[col * 3] + m[row + 3] * other[col * 3 + 1] + m[row + 6] * other[col * 3 + 2];
        }
    }

    return product;
}

Vector3 Matrix3::operator * (const Vector3& vec) const
{
    Vector3 product;

    for (int row = 0; row < 3; row++)
    {
        product[row] = m[row] * vec[0] + m[row + 3] * vec[1] + m[row + 6] * vec[2];
    }

    return product;
}

Matrix3 Matrix3::operator * (float scalar) const
{
    Matrix3 result;
    for (size_t i = 0; i < 9; i++)
    {
        result[i] = m[i] * scalar;
    }

    return result;
}

namespace Emerald
{
    Matrix3 operator * (float scalar, const Matrix3& mat)
    {
        return mat * scalar;
    }
}

bool Matrix3::operator == (const Matrix3& other) const
{
    for (int i = 0; i < 9; i++)
    {
        if (m[i] != other[i])
        {
            return false;
        }
    }

    return true;
}

bool Matrix3::operator != (const Matrix3& other) const
{
    return !operator == (other);
}

namespace Emerald
{
    std::ostream& operator << (std::ostream& o, const Matrix3& mat)
    {
        o << mat[0] << "  " << mat[3] << "  " << mat[6] << "\n"
          << mat[1] << "  " << mat[4] << "  " << mat[7] << "\n"
          << mat[2] << "  " << mat[5] << "  " << mat[8];

        o << "\n" << "\n";
        
        return o;
    }
}