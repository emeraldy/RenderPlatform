//-----------------------------------------------------------------
// Maths Utility Classes
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "Maths.h"

using namespace Emerald;

Radian::Radian(const Degree& d) 
{ 
    value = d.ToRadianValue(); 
}

const Radian& Radian::operator = (const Degree& d) 
{ 
    value = d.ToRadianValue(); 
    return *this;
}