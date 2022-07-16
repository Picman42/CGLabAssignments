#include "marchinginfo.h"
#include <cmath>

void MarchingInfo::NextCell()
{
    if (Tnext.x() < min(Tnext.y(), Tnext.z()))
    {
        GI += Sign.x();
        Tmin = Tnext.x();
        Tnext += Vec3f(dt.x(), 0, 0);
        Normal = Sign.x() > 0 ? Vec3f(-1.0f, 0, 0) : Vec3f(1.0f, 0, 0);
    }
    else if (Tnext.y() < min(Tnext.x(), Tnext.z()))
    {
        GJ += Sign.y();
        Tmin = Tnext.y();
        Tnext += Vec3f(0, dt.y(), 0);
        Normal = Sign.y() > 0 ? Vec3f(0, -1.0f, 0) : Vec3f(0, 1.0f, 0);
    }
    else
    {
        GK += Sign.z();
        Tmin = Tnext.z();
        Tnext += Vec3f(0, 0, dt.z());
        Normal = Sign.z() > 0 ? Vec3f(0, 0, -1.0f) : Vec3f(0, 0, 1.0f);
    }
}