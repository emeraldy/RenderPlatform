//-----------------------------------------------------------------
//Quaternion Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include "StandardIncludes.h"
#include "Maths.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"

namespace Emerald
{
    class Quaternion
    {
    public:
        static const Quaternion IDENTITY;
        Quaternion(const Degree& d = Degree(), const Vector3& v = Vector3());//ATTENTION: it is the angle you want for rotation, not the one encoded by quaternion!!
        Quaternion(float toW, float toX = 0.0f, float toY = 0.0f, float toZ = 0.0f);
        Quaternion(const Vector4& v);

        float Magnitude() const;
        bool Normalise();
        bool GetAngle(Degree& angle) const;
        bool GetAxis(Vector3& axis) const;
        float Dot(const Quaternion& other) const;
        bool Inverse(Quaternion& result) const;
        bool Exp(Quaternion& result) const;
        bool Log(Quaternion& result) const;
        Matrix3 ToMatrix3() const;
        static Quaternion FromMatrix3(const Matrix3& source);
        static bool Slerp(const Quaternion& q0, const Quaternion& q1, Quaternion& result, float t);
        friend std::ostream& operator << (std::ostream& o, const Quaternion& q);

        Quaternion operator - () const;
        Quaternion operator * (const Quaternion& other) const;
        Quaternion operator * (float f) const;
        Quaternion operator / (float f) const;
        bool operator == (const Quaternion& other) const;
        bool operator != (const Quaternion& other) const;

    private:
        float w;
        float x;
        float y;
        float z;
    };
}