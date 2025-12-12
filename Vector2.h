//-----------------------------------------------------------------
// Two-demensional Vector Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include "StandardIncludes.h"

namespace Emerald
{
    constexpr float ZEROTHRESHOLD = 1e-8;
    
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

        float operator [] (size_t index) const
        {
            assert(index < 2);
            return  *(&x + index);
        }

        float& operator [] (size_t index)
        {
            assert(index < 2);
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

        Vector2 operator / (float scalar) const
        {
            assert(scalar != 0.0);

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
            assert(scalar != 0.0);

            x /= scalar;
            y /= scalar;

            return *this;
        }

        float length() const
        {
            return sqrt(x * x + y * y);
        }

        Vector2 normalise()
        {
            float len = length();

            if (len > ZEROTHRESHOLD)
            {
                x /= len;
                y /= len;
            }

            return *this;
        }

        float dot(const Vector2& other) const
        {
            return x * other.x + y * other.y;
        }

        friend std::ostream& operator << (std::ostream& o, const Vector2& v)
        {
            o << "[" << v.x << ", " << v.y << "]";
            return o;
        }
    };
}