//-----------------------------------------------------------------
// Quaternion Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Quaternion.h"

using namespace Emerald;

Quaternion::Quaternion(const Degree& d, const Vector3& v)
{
    angle = d;
    axis = v;
    axis.Normalise();

    Radian theta = d;
    std::feclearexcept(FE_ALL_EXCEPT);
    float sineHalfTheta = std::sin(theta / 2.0f);
    w = std::cos(theta / 2.0f);
    if (std::fetestexcept(FE_INVALID))
    {
        w = 1.0f;
        sineHalfTheta = 0;
        angle = 0;
        axis = Vector3();
    }
    x = sineHalfTheta * axis.x;
    y = sineHalfTheta * axis.y;
    z = sineHalfTheta * axis.z;
}

Quaternion::Quaternion(float toW, float toX, float toY, float toZ)
{
    w = toW;
    x = toX;
    y = toY;
    z = toZ;

    std::feclearexcept(FE_ALL_EXCEPT);
    Radian halfTheta = std::acos(w);
    float sineHalfTheta = std::sin(halfTheta);
    if (std::fetestexcept(FE_INVALID) || sineHalfTheta < ZEROTHRESHOLD)
    {
        w = 1.0f;
        x = 0;
        y = 0;
        z = 0;
        angle = 0;
        axis = Vector3();
    }
    else
    {
        angle = halfTheta * 2.0f;
        axis = Vector3(x / sineHalfTheta, y / sineHalfTheta, z / sineHalfTheta);
        axis.Normalise();
    }
}

float Quaternion::Magnitude() const
{
    return std::sqrt(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::Normalise()
{
    Radian theta = angle;
    std::feclearexcept(FE_ALL_EXCEPT);
    float sineHalfTheta = std::sin(theta / 2.0f);
    if (std::fetestexcept(FE_INVALID))
    {
        return *this;
    }
    
    axis.Normalise();
    x = sineHalfTheta * axis.x;
    y = sineHalfTheta * axis.y;
    z = sineHalfTheta * axis.z;

    return *this;
}

float Quaternion::Dot(const Quaternion& other) const
{
    return w * other.w + x * other.x + y * other.y + z * other.z;
}

bool Quaternion::Inverse(Quaternion& result) const
{
    result = Quaternion(w, -x, -y, -z);
    float mag = Magnitude();
    if (mag < ZEROTHRESHOLD)
    {
        return false;
    }

    result = result / mag;
    return true;
}