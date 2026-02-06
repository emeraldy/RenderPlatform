//-----------------------------------------------------------------
// Quaternion Class
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Quaternion.h"

using namespace Emerald;

Quaternion::Quaternion(Error& err, const Degree& d, const Vector3& v)
{
    Radian theta = d;
    Vector3 axis(v);
    axis.Normalise(err);
    std::feclearexcept(FE_ALL_EXCEPT);
    float sineHalfTheta = std::sin(theta / 2.0f);
    w = std::cos(theta / 2.0f);
    if (std::fetestexcept(FE_INVALID)) 
    {
        err += L"Construct quaternion from angle and axis failed.";
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

float Quaternion::Magnitude(Error& err) const
{
    float result = 0;
    std::feclearexcept(FE_ALL_EXCEPT);
    result = std::sqrt(w * w + x * x + y * y + z * z);
    if (fetestexcept(FE_INVALID))
    {
        err += L"Quaternion Magnitude error.";
        return 0;
    }
    else
    {
        return result;
    }
}

bool Quaternion::Normalise(Error& err)
{
    float mag = Magnitude(err);
    if (mag < ZEROTHRESHOLD)
    {
        err += L"mag in Quaternion Normalise is zero.";
        return false;
    }
    w /= mag;
    x /= mag;
    y /= mag;
    z /= mag;

    return true;
}

Degree Quaternion::GetAngle(Error& err) const
{
    float mag = Magnitude(err);
    if (mag < ZEROTHRESHOLD)
    {
        err += L"mag in Quaterion GetAngle is zero.";
        return Degree();
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float halfTheta = std::acos(w / mag);
    if (std::fetestexcept(FE_INVALID)) 
    {
        err += L"Quaternion GetAngle halfTheta failed.";
        return Degree();
    }
    return Degree(Radian(halfTheta * 2));
}

Vector3 Quaternion::GetAxis(Error& err) const
{
    Vector3 axis;
    Quaternion unitQ = *this;
    if (!unitQ.Normalise(err))
    {
        return axis;
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float halfTheta = std::acos(unitQ.w);
    float sineHalfTheta = std::sin(halfTheta);
    if (std::fetestexcept(FE_INVALID)) 
    {
        err += L"Quaternion GetAxis halfTheta failed.";
        return axis;
    }
    axis.x = unitQ.x / sineHalfTheta;
    axis.y = unitQ.y / sineHalfTheta;
    axis.z = unitQ.z / sineHalfTheta;
    axis = axis.Normalise(err);

    return axis;
}

float Quaternion::Dot(const Quaternion& other) const
{
    return w * other.w + x * other.x + y * other.y + z * other.z;
}

Quaternion Quaternion::Inverse(Error& err) const
{
    Quaternion result(w, -x, -y, -z);
    float mag = Magnitude(err);
    if (mag < ZEROTHRESHOLD)
    {
        return result;
    }

    result = result / mag;
    return true;
}

Quaternion Quaternion::Exp(Error& err) const
{
    //Q = [w, V], where V = [x, y, z]
    std::feclearexcept(FE_ALL_EXCEPT);
    float magV = std::sqrt(x * x + y * y + z * z);
    float powerEW = std::pow(EULERSNUMBER, w);
    float cosMagV = std::cos(magV);
    float sinMagV = std::sin(magV);
    if (fetestexcept(FE_DIVBYZERO | FE_INVALID) || magV < ZEROTHRESHOLD)
    {
        err += L"Quaternion Exp failed.";
        return Quaternion();
    }

    float coefficient = powerEW * sinMagV / magV;
    Quaternion result(powerEW * cosMagV, coefficient * x, coefficient * y, coefficient * z);

    return result;
}

Quaternion Quaternion::Log(Error& err) const
{
    //Q = [w, V], where V = [x, y, z]
    float magQ = Magnitude(err);
    if (magQ < ZEROTHRESHOLD)
    {
        return Quaternion();
    }
    std::feclearexcept(FE_ALL_EXCEPT);
    float magV = std::sqrt(x * x + y * y + z * z);
    float acosWOverMagQ = std::acos(w / magQ);
    float logMagQ = std::log(magQ);
    if (fetestexcept(FE_INVALID | FE_DIVBYZERO) || magV < ZEROTHRESHOLD)
    {
        err += L"Quaternion Log failed.";
        return Quaternion();
    }

    float coefficient = acosWOverMagQ / magV;
    Quaternion result(logMagQ, coefficient * x, coefficient * y, coefficient * z);

    return result;
}

Matrix3 Quaternion::ToMatrix3(Error& err) const
{
    
    Matrix3 result;
    Vector3 col0(1 - 2 * y * y - 2 * z * z, 2 * x * y + 2 * w * z, 2 * x * z - 2 * w * y);
    Vector3 col1(2 * x * y - 2 * w * z, 1 - 2 * x * x - 2 * z * z, 2 * y * z + 2 * w * x);
    Vector3 col2(2 * x * z + 2 * w * y, 2 * y * z - 2 * w * x, 1 - 2 * x * x - 2 * y * y);
    result.SetColumn(0, col0, err);
    result.SetColumn(1, col1, err);
    result.SetColumn(2, col2, err);
    
    return result;
}

Quaternion Quaternion::FromMatrix3(const Matrix3& source, Error& err)
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
        err += L"Quaternion FromMatrix3 largest component failed.";
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

bool Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, Quaternion& result, float t, Error& err)
{
    Quaternion unitQ0 = q0;
    Quaternion unitQ1 = q1;

    if (!unitQ0.Normalise(err) || !unitQ1.Normalise(err))
    {
        err += L"Quaternion Slerp Normalisation failed.";
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
            err += L"Quaternion Slerp failed.";
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