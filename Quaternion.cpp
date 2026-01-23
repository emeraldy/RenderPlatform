//-----------------------------------------------------------------
// Quaternion Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Quaternion.h"

using namespace Emerald;

const Quaternion Quaternion::IDENTITY = Quaternion(1.0f);

Quaternion::Quaternion(const Degree& d, const Vector3& v)
{
    Radian theta = d;
    Vector3 axis(v);
    axis.Normalise();
    std::feclearexcept(FE_ALL_EXCEPT);
    float sineHalfTheta = std::sin(theta / 2.0f);
    w = std::cos(theta / 2.0f);
    if (std::fetestexcept(FE_INVALID))//TODO: engine-wide exception system. how? 
    {
        w = 1.0f;
        sineHalfTheta = 0;
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
}

Quaternion::Quaternion(const Vector4& v)
{
    Quaternion(v.x, v.y, v.z, v.w);
}

float Quaternion::Magnitude() const
{
    return std::sqrt(w * w + x * x + y * y + z * z);
}

bool Quaternion::Normalise()
{
    float mag = Magnitude();
    if (mag < ZEROTHRESHOLD)
    {
        return false;
    }
    w /= mag;
    x /= mag;
    y /= mag;
    z /= mag;

    return true;
}

bool Quaternion::GetAngle(Degree& angle) const
{
    float mag = Magnitude();
    if (mag < ZEROTHRESHOLD)
    {
        return false;
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float halfTheta = std::acos(w / mag);
    if (std::fetestexcept(FE_INVALID))//TODO: engine-wide exception system. how? 
    {
        return false;
    }
    angle = Radian(halfTheta * 2);

    return true;
}

bool Quaternion::GetAxis(Vector3& axis) const
{
    Quaternion unitQ = *this;
    if (!unitQ.Normalise())
    {
        return false;
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float halfTheta = std::acos(unitQ.w);
    float sineHalfTheta = std::sin(halfTheta);
    if (std::fetestexcept(FE_INVALID))//TODO: engine-wide exception system. how? 
    {
        return false;
    }
    axis.x = unitQ.x / sineHalfTheta;
    axis.y = unitQ.y / sineHalfTheta;
    axis.z = unitQ.z / sineHalfTheta;
    axis = axis.Normalise();

    return true;
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

bool Quaternion::Exp(Quaternion& result) const
{
    //Q = [w, V], where V = [x, y, z]
    float magV = std::sqrt(x * x + y * y + z * z);
    if (magV < ZEROTHRESHOLD)
    {
        return false;
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float powerEW = std::pow(EULERSNUMBER, w);
    float cosMagV = std::cos(magV);
    float sinMagV = std::sin(magV);
    if (fetestexcept(FE_DIVBYZERO | FE_INVALID))
    {
        return false;
    }

    float coefficient = powerEW * sinMagV / magV;
    result = Quaternion(powerEW * cosMagV, coefficient * x, coefficient * y, coefficient * z);

    return true;
}

bool Quaternion::Log(Quaternion& result) const
{
    //Q = [w, V], where V = [x, y, z]
    float magQ = Magnitude();
    float magV = std::sqrt(x * x + y * y + z * z);
    if (magQ < ZEROTHRESHOLD || magV < ZEROTHRESHOLD)
    {
        return false;
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float acosWOverMagQ = std::acos(w / magQ);
    float logMagQ = std::log(magQ);
    if (fetestexcept(FE_INVALID | FE_DIVBYZERO))
    {
        return false;
    }

    float coefficient = acosWOverMagQ / magV;
    result = Quaternion(logMagQ, coefficient * x, coefficient * y, coefficient * z);

    return true;
}

bool Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, Quaternion& result, float t)
{
    Quaternion unitQ0 = q0;
    Quaternion unitQ1 = q1;

    if (!unitQ0.Normalise() || !unitQ1.Normalise())
    {
        return false;
    }

    float cosW = unitQ0.Dot(unitQ1);
    if (cosW < 0)
    {
        //take the shortest arc
        unitQ1 = unitQ1 * (-1.0f);
        cosW = -cosW;
    }

    float k0, k1;
    float w;
    if (cosW > 0.9999f)
    {
        k0 = 1.0f - t;
        k1 = t;
    }
    else
    {
        std::feclearexcept(FE_ALL_EXCEPT);
        w = std::acos(cosW);
        k0 = std::sin((1.0f - t) * w) / std::sin(w);
        k1 = std::sin(t * w) / std::sin(w);
        if (fetestexcept(FE_INVALID))
        {
            return false;
        }
    }

    result.w = unitQ0.w * k0 + unitQ1.w * k1;
    result.x = unitQ0.x * k0 + unitQ1.x * k1;
    result.y = unitQ0.y * k0 + unitQ1.y * k1;
    result.z = unitQ0.z * k0 + unitQ1.z * k1;

    return true;
}

Quaternion Quaternion::operator - () const
{
    return Quaternion(-w, -x, -y, -z);
}

Quaternion Quaternion::operator * (const Quaternion& other) const
{
    Vector4 product;
    product.x = w * other.w - x * other.x - y * other.y - z * other.z;
    product.y = w * other.x + x * other.w + y * other.z - z * other.y;
    product.z = w * other.y + y * other.w + z * other.x - x * other.z;
    product.w = w * other.z + z * other.w + x * other.y - y * other.x;

    return Quaternion(product);
}

Quaternion Quaternion::operator * (float f) const
{
    return Quaternion(w * f, x * f, y * f, z * f);
}

Quaternion Quaternion::operator / (float f) const
{
    return Quaternion(w / f, x / f, y / f, z / f);
}

bool Quaternion::operator == (const Quaternion& other) const
{
    return (w == other.w && x == other.x && y == other.y && z == other.z);
}

bool Quaternion::operator != (const Quaternion& other) const
{
    return !operator == (other);
}

namespace Emerald
{
    std::ostream& operator << (std::ostream& o, const Quaternion& q)
    {
        o << "[" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << "]";
        return o;
    }
}