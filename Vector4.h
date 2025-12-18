//-----------------------------------------------------------------
// Four-demensional Vector Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include "StandardIncludes.h"

namespace Emerald
{
    /*
    * The convention is column vector
    */
    class Vector4
    {
    public:
        float x;
        float y;
        float z;
        float w;

        Vector4(float ax = 0.0f, float ay = 0.0f, float az = 0.0f, float aw = 1.0f)
        {
            x = ax;
            y = ay;
            z = az;
            w = aw;
        }

        float dot(const Vector4& other) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }
        friend std::ostream& operator << (std::ostream& o, const Vector4& v)
        {
            o << "[" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]";
            return o;
        }

        float operator [] (size_t index) const
        {
            assert(index < 4);
            return  *(&x + index);
        }
        float& operator [] (size_t index)
        {
            assert(index < 4);
            return *(&x + index);
        }
        bool operator == (const Vector4& other) const
        {
            return (x == other.x && y == other.y && z == other.z && w == other.w);
        }
        bool operator != (const Vector4& other) const
        {
            return (x != other.x || y != other.y || z != other.z || w != other.w);
        }
        Vector4 operator + (const Vector4& other) const
        {
            Vector4 sum;

            sum.x = x + other.x;
            sum.y = y + other.y;
            sum.z = z + other.z;
            sum.w = w + other.w;

            return sum;
        }
        inline Vector4 operator - (const Vector4& other) const
        {
            Vector4 difference;

            difference.x = x - other.x;
            difference.y = y - other.y;
            difference.z = z - other.z;
            difference.w = w - other.w;

            return difference;
        }
        Vector4 operator * (float scalar) const
        {
            Vector4 product;

            product.x = scalar * x;
            product.y = scalar * y;
            product.z = scalar * z;
            product.w = scalar * w;

            return product;
        }
        friend Vector4 operator * (float scalar, const Vector4& vec)
        {
            return vec * scalar;
        }
        Vector4 operator / (float scalar) const
        {
            assert(scalar != 0.0);

            Vector4 quotient;

            quotient.x = x / scalar;
            quotient.y = y / scalar;
            quotient.z = z / scalar;
            quotient.w = w / scalar;

            return quotient;
        }
        Vector4 operator - () const
        {
            Vector4 negate;

            negate.x = -x;
            negate.y = -y;
            negate.z = -z;
            negate.w = -w;

            return negate;
        }
        Vector4& operator += (const Vector4& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this;
        }
        Vector4& operator -= (const Vector4& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return *this;
        }
        Vector4& operator *= (float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;

            return *this;
        }
        Vector4& operator /= (float scalar)
        {
            assert(scalar != 0.0);

            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;

            return *this;
        }
    };
}
