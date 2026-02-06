//-----------------------------------------------------------------
// Two-demensional Vector Class Header
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
    class Vector2
    {
    public:
        float x;
        float y;

        Vector2(float ax = 0.0f, float ay = 0.0f) 
        {
            x = ax;
            y = ay;
        }
        float Length(Error& err) const
        {
            float result = 0;
            std::feclearexcept(FE_ALL_EXCEPT);
            result = std::sqrt(x * x + y * y);
            if (fetestexcept(FE_INVALID))
            {
                err += L"Vector2 [" + std::to_wstring(x) + L" ," + std::to_wstring(y) + L"] length invalid!";
                result = 0;
            }

            return result;
        }
        Vector2 Normalise(Error& err)
        {
            Error localErr;
            float len = Length(localErr);

            if (len > ZEROTHRESHOLD && !localErr)
            {
                x /= len;
                y /= len;
            }

            err += localErr;

            return *this;
        }
        float Dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }
        friend std::ostream& operator << (std::ostream& o, const Vector2& v)
        {
            o << "[" << v.x << ", " << v.y << "]";
            return o;
        }
        float operator [] (size_t index) const
        {
            if (index < 0 || index > 1)
            {
                throw std::out_of_range("at Vector2 operator [] const");
            }
            return  *(&x + index);
        }
        float& operator [] (size_t index)
        {
            if (index < 0 || index > 1)
            {
                throw std::out_of_range("at Vector2 operator []");
            }
            return *(&x + index);
        }
        bool operator == (const Vector2& other) const
        {
            return (x == other.x && y == other.y);
        }
        bool operator != (const Vector2& other) const
        {
            return (x != other.x || y != other.y);
        }
        Vector2 operator + (const Vector2& other) const
        {
            Vector2 sum;

            sum.x = x + other.x;
            sum.y = y + other.y;

            return sum;
        }
        inline Vector2 operator - (const Vector2& other) const
        {
            Vector2 difference;

            difference.x = x - other.x;
            difference.y = y - other.y;

            return difference;
        }
        Vector2 operator * (float scalar) const
        {
            Vector2 product;

            product.x = scalar * x;
            product.y = scalar * y;

            return product;
        }
        friend Vector2 operator * (float scalar, const Vector2& vec)
        {
            return vec * scalar;
        }
        Vector2 operator / (float scalar) const
        {
            if (scalar < ZEROTHRESHOLD)
            {
                throw std::invalid_argument("at Vector2 operator /");
            }

            Vector2 quotient;

            quotient.x = x / scalar;
            quotient.y = y / scalar;

            return quotient;
        }
        Vector2 operator - () const
        {
            Vector2 negate;

            negate.x = -x;
            negate.y = -y;

            return negate;
        }
        Vector2& operator += (const Vector2& other)
        {
            x += other.x;
            y += other.y;

            return *this;
        }
        Vector2& operator -= (const Vector2& other)
        {
            x -= other.x;
            y -= other.y;

            return *this;
        }
        Vector2& operator *= (float scalar)
        {
            x *= scalar;
            y *= scalar;

            return *this;
        }
        Vector2& operator /= (float scalar)
        {
            if (scalar < ZEROTHRESHOLD)
            {
                throw std::invalid_argument("at Vector2 operator /=");
            }

            x /= scalar;
            y /= scalar;

            return *this;
        }
    };
}