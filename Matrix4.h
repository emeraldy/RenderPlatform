//-----------------------------------------------------------------
// 4x4 Matrix Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include "StandardIncludes.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Vector4.h"

namespace Emerald
{
    /*
    * The convention is column major with right handedness.
    * m0 m4 m8  m12
    * m1 m5 m9  m13
    * m2 m6 m10 m14
    * m3 m7 m11 m15
    */
    class Matrix4
    {
    public:
        static const Matrix4 IDENTITY;
        static Matrix4 MakeTranslation(const Vector3& vec);
        static Matrix4 MakeScale(const Vector3& vec);

        Matrix4(float e0 = 0.0f, float e1 = 0.0f, float e2 = 0.0f, float e3 = 0.0f,
                float e4 = 0.0f, float e5 = 0.0f, float e6 = 0.0f, float e7 = 0.0f,
                float e8 = 0.0f, float e9 = 0.0f, float e10 = 0.0f, float e11 = 0.0f,
                float e12 = 0.0f, float e13 = 0.0f, float e14 = 0.0f, float e15 = 0.0f)
        {
            m[0] = e0; m[4] = e4; m[8] = e8;   m[12] = e12;
            m[1] = e1; m[5] = e5; m[9] = e9;   m[13] = e13;
            m[2] = e2; m[6] = e6; m[10] = e10; m[14] = e14;
            m[3] = e3; m[7] = e7; m[11] = e11; m[15] = e15;
        }

        Matrix4(const Matrix3& mat3)
        {
            m[0] = mat3[0]; m[4] = mat3[3]; m[8] =  mat3[6]; m[12] = 0;
            m[1] = mat3[1]; m[5] = mat3[4]; m[9] =  mat3[7]; m[13] = 0;
            m[2] = mat3[2]; m[6] = mat3[5]; m[10] = mat3[8]; m[14] = 0;
            m[3] = 0;       m[7] = 0;       m[11] = 0;       m[15] = 1.0f;
        }

        //0-indexed column
        Vector4 GetColumn(size_t col) const;
        void SetColumn(size_t col, const Vector4& values);
        //0-indexed row
        Vector4 GetRow(size_t row) const;
        void SetRow(size_t row, const Vector4& values);
        Matrix3 Retrieve3x3() const;
        Vector3 GetTranslation() const;
        Vector3 GetScale() const;

        //extract a 3x3 submatrix by eliminating row and col from m
        Matrix3 SubMatrix3(size_t row, size_t col) const;
        Matrix4 Transpose() const;
        float Determinant() const;
        bool Inverse(Matrix4& result, float tolerance = 1e-6) const;

        float operator [] (size_t index) const;
        float& operator [] (size_t index);
        Matrix4 operator + (const Matrix4& other) const;
        Matrix4 operator - (const Matrix4& other) const;
        Matrix4 operator * (const Matrix4& other) const;
        Vector4 operator * (const Vector4& vec) const;
        Matrix4 operator * (float scalar) const;
        friend Matrix4 operator * (float scalar, const Matrix4& mat);
        bool operator == (const Matrix4& other) const;
        bool operator != (const Matrix4& other) const;

        friend std::ostream& operator << (std::ostream& o, const Matrix4& mat);

    private:
        float m[16];
    };
}