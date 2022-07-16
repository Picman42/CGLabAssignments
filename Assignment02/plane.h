#ifndef _PLANE_H_
#define _PLANE_H_

#include "object3d.h"
#include "vectors.h"
#include "matrix.h"
#include "material.h"

class Plane: public Object3D
{
public:
    //Plane() {}
    Plane(Vec3f &_normal, float _d, Material *_mat):
        Normal(_normal), Dist2Origin(_d) { Mat = _mat; }

    virtual bool intersect(const Ray &r, Hit &h, float tmin);

    Vec3f Normal;
    float Dist2Origin;
};

#endif