#ifndef _MARCHINGINFO_H_
#define _MARCHINGINFO_H_

#include "vectors.h"
#include <cmath>

class MarchingInfo
{
public:
    MarchingInfo(): Tmin(INFINITY), GI(-1), GJ(-1), GK(-1), 
        Normal(Vec3f(256.0f, 0, 0)) {}
    void NextCell();

    float Tmin;
    int GI, GJ, GK;
    Vec3f Tnext;
    Vec3f dt;
    Vec3f Sign;
    Vec3f Normal;
};

#endif