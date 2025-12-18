//-----------------------------------------------------------------
// 3x3 Matrix Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include "StandardIncludes.h"
#include "Vector3.h"

namespace Emerald
{
    /*
    * The convention is column major with right handedness.
    * m0 m3 m6
    * m1 m4 m7
    * m2 m5 m8
    */
    class Matrix3
    {
    public:
        Matrix3(float e0 = 0.0f, float e1 = 0.0f, float e2 = 0.0f,
                float e3 = 0.0f, float e4 = 0.0f, float e5 = 0.0f,
                float e6 = 0.0f, float e7 = 0.0f, float e8 = 0.0f)
        {
            m[0] = e0; m[3] = e3; m[6] = e6;
            m[1] = e1; m[4] = e4; m[7] = e7;
            m[2] = e2; m[5] = e5; m[8] = e8;
        }

        //0-indexed column
        Vector3 GetColumn(size_t col) const;
        void SetColumn(size_t col, const Vector3& values);
        //0-indexed row
        Vector3 GetRow(size_t row) const;
        void SetRow(size_t row, const Vector3& values);

        void BuildFromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        Matrix3 Transpose() const;
        float Determinant() const;
        bool Inverse(Matrix3& result, float tolerance = 1e-6) const;
        void Orthogonalise();

        float operator [] (size_t index) const;
        float& operator [] (size_t index);
        Matrix3 operator + (const Matrix3& other) const;
        Matrix3 operator - (const Matrix3& other) const;
        Matrix3 operator * (const Matrix3& other) const;
        Matrix3 operator * (const Vector3& vec) const;
        Matrix3 operator * (float scalar) const;
        friend Matrix3 operator * (float scalar, const Matrix3& mat);
        bool operator == (const Matrix3& other) const;
        bool operator != (const Matrix3& other) const;

    private:
        float m[9];
    };
}