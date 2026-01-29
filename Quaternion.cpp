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
    float result = 0;
    std::feclearexcept(FE_ALL_EXCEPT);
    result = std::sqrt(w * w + x * x + y * y + z * z);
    if (fetestexcept(FE_INVALID))
    {
        return 0;
    }
    else
    {
        return result;
    }
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
    std::feclearexcept(FE_ALL_EXCEPT);
    float magV = std::sqrt(x * x + y * y + z * z);
    float powerEW = std::pow(EULERSNUMBER, w);
    float cosMagV = std::cos(magV);
    float sinMagV = std::sin(magV);
    if (fetestexcept(FE_DIVBYZERO | FE_INVALID) || magV < ZEROTHRESHOLD)
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
    if (magQ < ZEROTHRESHOLD)
    {
        return false;
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float magV = std::sqrt(x * x + y * y + z * z);
    float acosWOverMagQ = std::acos(w / magQ);
    float logMagQ = std::log(magQ);
    if (fetestexcept(FE_INVALID | FE_DIVBYZERO) || magV < ZEROTHRESHOLD)
    {
        return false;
    }

    float coefficient = acosWOverMagQ / magV;
    result = Quaternion(logMagQ, coefficient * x, coefficient * y, coefficient * z);

    return true;
}

Matrix3 Quaternion::ToMatrix3() const
{
    
    Matrix3 result;
    Vector3 col0(1 - 2 * y * y - 2 * z * z, 2 * x * y + 2 * w * z, 2 * x * z - 2 * w * y);
    Vector3 col1(2 * x * y - 2 * w * z, 1 - 2 * x * x - 2 * z * z, 2 * y * z + 2 * w * x);
    Vector3 col2(2 * x * z + 2 * w * y, 2 * y * z - 2 * w * x, 1 - 2 * x * x - 2 * y * y);
    result.SetColumn(0, col0);
    result.SetColumn(1, col1);
    result.SetColumn(2, col2);
    
    return result;
}

Quaternion Quaternion::FromMatrix3(const Matrix3& source)
{
    float wTerm = source[0] + source[4] + source[8];
    float xTerm = source[0] - source[4] - source[8];
    float yTerm = source[4] - source[0] - source[8];
    float zTerm = source[8] - source[0] - source[4];

    int largestIndex = 0;
    float largestTerm = wTerm;
    if (xTerm > largestTerm)
    {
        largestTerm = xTerm;
        largestIndex = 1;
    }
    if (yTerm > largestTerm)
    {
        largestTerm = yTerm;
        largestIndex = 2;
    }
    if (zTerm > largestTerm)
    {
        largestTerm = zTerm;
        largestIndex = 3;
    }

    std::feclearexcept(FE_ALL_EXCEPT);
    float largestComponent = std::sqrt(largestTerm + 1.0f) * 0.5f;
    if (fetestexcept(FE_INVALID) || largestTerm < ZEROTHRESHOLD)
    {
        return Quaternion();
    }

    float coefficient = 0.25f / largestTerm;
    Quaternion result;
    switch (largestIndex)
    {
    case 0:
        result.w = largestComponent;
        result.x = (source[5] - source[7]) * coefficient;
        result.y = (source[6] - source[2]) * coefficient;
        result.z = (source[1] - source[3]) * coefficient;
        break;
    case 1:
        result.x = largestComponent;
        result.w = (source[5] - source[7]) * coefficient;
        result.y = (source[1] + source[3]) * coefficient;
        result.z = (source[6] + source[2]) * coefficient;
        break;
    case 2:
        result.y = largestComponent;
        result.w = (source[6] - source[2]) * coefficient;
        result.x = (source[1] - source[3]) * coefficient;
        result.z = (source[5] + source[7]) * coefficient;
        break;
    case 3:
        result.z = largestComponent;
        result.w = (source[1] - source[3]) * coefficient;
        result.x = (source[2] + source[2]) * coefficient;
        result.y = (source[5] + source[7]) * coefficient;
        break;
    }

    return result;
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