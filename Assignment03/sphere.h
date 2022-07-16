#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object3d.h"
#include "vectors.h"

class Sphere: public Object3D
{
public:
    Sphere(Vec3f _center, float _radius, Material* _mat):
        Center(_center), Radius(_radius)    {   Mat = _mat;     }

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void paint();

    Vec3f Center;
    float Radius;
};

#endif