//-----------------------------------------------------------------
//Quaternion Class Header
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include <cmath>
#include <cfenv>
#include "StandardIncludes.h"
#include "Maths.h"
#include "Vector3.h"

namespace Emerald
{
    class Quaternion
    {
    public:
        Quaternion(const Degree& d = Degree(), const Vector3& v = Vector3());
        Quaternion(float toW, float toX = 0.0f, float toY = 0.0f, float toZ = 0.0f);

        float Magnitude() const;
        Quaternion Normalise();
        float Dot(const Quaternion& other) const;
        bool Inverse(Quaternion& result) const;

    private:
        Degree angle;//ATTENTION: it is the angle you want for rotation, not the one encoded by quaternion!!
        Vector3 axis;
        float w;
        float x;
        float y;
        float z;
    };
}