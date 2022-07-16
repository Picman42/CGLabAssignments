#include "plane.h"
#include <cmath>

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{
    float dist_to_ray_origin = -Dist2Origin + r.getOrigin().Dot3(Normal);
    float dot = r.getDirection().Dot3(Normal);
    if (dot == 0.0f)
    {
        return false;
    }
    float t = -dist_to_ray_origin / dot;
    if (t > tmin && t < h.getT())
    {
        h.set(t, Mat, Normal, r);
        return true;
    }
    return false;
}

