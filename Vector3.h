//-----------------------------------------------------------------
// Three-demensional Vector Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include "Maths.h"
#include "StandardIncludes.h"

namespace Emerald
{
    /*
    * The convention is column vector
    */
    class Vector3
    {
    public:
        float x;
        float y;
        float z;
        
        Vector3(float ax = 0.0f, float ay = 0.0f, float az = 0.0f)
        {
            x = ax;
            y = ay;
            z = az;
        }

        float Length() const
        {
            float result = 0;
            std::feclearexcept(FE_ALL_EXCEPT);
            result = std::sqrt(x * x + y * y + z * z);
            if (fetestexcept(FE_INVALID))
            {
                return 0;
            }
            else
            {
                return result;
            }
        }
        Vector3 Normalise()
        {
            float len = Length();

            if (len > ZEROTHRESHOLD)
            {
                x /= len;
                y /= len;
                z /= len;
            }

            return *this;
        }
        float Dot(const Vector3& other) const
        {
            return x * other.x + y * other.y + z * other.z;
        }
        Vector3 Cross(const Vector3& other) const
        {
            Vector3 product;

            product.x = y * other.z - z * other.y;
            product.y = z * other.x - x * other.z;
            product.z = x * other.y - y * other.x;
        }
        friend std::ostream& operator << (std::ostream& o, const Vector3& v)
        {
            o << "[" << v.x << ", " << v.y << ", " << v.z << "]";
            return o;
        }

        float operator [] (size_t index) const
        {
            assert(index < 3);
            return *(&x + index);
        }
        float& operator [] (size_t index)
        {
            assert(index < 3);
            return *(&x + index);
        }
        bool operator == (const Vector3& other) const
        {
            return (x == other.x && y == other.y && z == other.z);
        }
        bool operator != (const Vector3& other) const
        {
            return (x != other.x || y != other.y || z != other.z);
        }
        Vector3 operator + (const Vector3& other) const
        {
            Vector3 sum;

            sum.x = x + other.x;
            sum.y = y + other.y;
            sum.z = z + other.z;

            return sum;
        }
        inline Vector3 operator - (const Vector3& other) const
        {
            Vector3 difference;

            difference.x = x - other.x;
            difference.y = y - other.y;
            difference.z = z - other.z;

            return difference;
        }
        Vector3 operator * (float scalar) const
        {
            Vector3 product;

            product.x = scalar * x;
            product.y = scalar * y;
            product.z = scalar * z;

            return product;
        }
        friend Vector3 operator * (float scalar, const Vector3& vec)
        {
            return vec * scalar;
        }
        Vector3 operator / (float scalar) const
        {
            assert(scalar != 0.0);

            Vector3 quotient;

            quotient.x = x / scalar;
            quotient.y = y / scalar;
            quotient.z = z / scalar;

            return quotient;
        }
        Vector3 operator - () const
        {
            Vector3 negate;

            negate.x = -x;
            negate.y = -y;
            negate.z = -z;

            return negate;
        }
        Vector3& operator += (const Vector3& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;

            return *this;
        }
        Vector3& operator -= (const Vector3& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }
        Vector3& operator *= (float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;

            return *this;
        }
        Vector3& operator /= (float scalar)
        {
            assert(scalar != 0.0);

            x /= scalar;
            y /= scalar;
            z /= scalar;

            return *this;
        }
    };
}