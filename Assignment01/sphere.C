#include "sphere.h"
#include <cmath>
#include <iostream>

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{
    float t;
    //Vec3f ActualOrigin = r.getOrigin() - tmin * r.getDirection();
    Vec3f Ro = r.getOrigin() - Center;
    float dist2 = Ro.Dot3(Ro);          //Squared distance to ray origin
    Vec3f Rd = r.getDirection();
    float tp = -Ro.Dot3(Rd);            //Closest point to sphere center
    //If origin outside and tp < 0: no hit
    /*
    if (dist2 > Radius * Radius && tp < 0)
    {
        return false;
    }
    */
    float d2 = dist2 - tp * tp;   //Squared distance to tp point
    if (d2 > Radius * Radius)
    {
        return false;
    }
    float dt = sqrt(Radius * Radius - d2);
    /*
    if (dist2 <= Radius * Radius)        //inside
    {
        t = tp + dt;
    }
    else
    {
        t = tp - dt;
    }
    */
    t = tp - dt;
    if (t > tmin && t < h.getT())
    {
        h.set(t, Mat, r);
        return true;
    }
    t = tp + dt;
    if (t > tmin && t < h.getT())
    {
        h.set(t, Mat, r);
        return true;
    }
    return false;
}