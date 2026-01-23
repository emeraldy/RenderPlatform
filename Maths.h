//-----------------------------------------------------------------
// Maths Utility Classes Header
//-----------------------------------------------------------------

#pragma once
#pragma STDC FENV_ACCESS on
//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include <cfenv>
#include "StandardIncludes.h"

namespace Emerald
{
    const float PI = 4.0f * std::atan(1.0f);
    const float EULERSNUMBER = std::exp(1.0f);
    constexpr float ZEROTHRESHOLD = 1e-6;

    class Degree;
    class Radian
    {
    public:
        Radian(float v = 0) { value = v;}
        Radian(const Degree& d);
        
        float ToDegreeValue() const{ return value * 180.0f / PI; }
        operator float() const { return value; }//so we can use a Radian object as float
        const Radian& operator = (const float& f) { value = f; return *this; }
        const Radian& operator = (const Degree& d);
        Radian operator + (const Radian& r) const { return Radian(value + r.value); }
        Radian& operator += (const Radian& r) { value += r.value; return *this; }
        Radian operator - () const { return Radian(-value); }
        Radian operator - (const Radian& r) const { return Radian(value - r.value); }
        Radian& operator -= (const Radian& r) { value -= r.value; return *this; }
        Radian operator * (float f) const { return Radian(value * f); }
        friend Radian operator * (float f, const Radian& r) { return Radian(r.value * f); }
        Radian& operator *= (float f) { value *= f; return *this; }
        Radian operator / (float f) const { return Radian(value / f); }
        Radian& operator /= (float f) { value /= f; return *this; }

        bool operator <  (const Radian& r) const { return value < r.value; }
        bool operator <= (const Radian& r) const { return value <= r.value; }
        bool operator == (const Radian& r) const { return value == r.value; }
        bool operator != (const Radian& r) const { return value != r.value; }
        bool operator >= (const Radian& r) const { return value >= r.value; }
        bool operator >  (const Radian& r) const { return value > r.value; }

    private:
        float value;
    };

    class Degree
    {
    public:
        Degree(float v = 0) { value = v; }
        Degree(const Radian& r) { value = r.ToDegreeValue(); }

        float ToRadianValue() const { return value * PI / 180.0f; }
        operator float() const { return value; }//so we can use a Degree object as float
        const Degree& operator = (const float& f) { value = f; return *this; }
        const Degree& operator = (const Radian& r) { value = r.ToDegreeValue(); return *this; }
        Degree operator + (const Degree& d) const { return Degree(value + d.value); }
        Degree& operator += (const Degree& d) { value += d.value; return *this; }
        Degree operator - () const { return Degree(-value); }
        Degree operator - (const Degree& d) const { return Degree(value - d.value); }
        Degree& operator -= (const Degree& d) { value -= d.value; return *this; }
        Degree operator * (float f) const { return Degree(value * f); }
        friend Degree operator * (float f, const Degree& d) { return Degree(d.value * f); }
        Degree& operator *= (float f) { value *= f; return *this; }
        Degree operator / (float f) const { return Degree(value / f); }
        Degree& operator /= (float f) { value /= f; return *this; }

        bool operator <  (const Degree& d) const { return value < d.value; }
        bool operator <= (const Degree& d) const { return value <= d.value; }
        bool operator == (const Degree& d) const { return value == d.value; }
        bool operator != (const Degree& d) const { return value != d.value; }
        bool operator >= (const Degree& d) const { return value >= d.value; }
        bool operator >  (const Degree& d) const { return value > d.value; }

    private:
        float value;
    };

}