#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object3d.h"
#include "vectors.h"
#include "matrix.h"
#include "material.h"

class Triangle: public Object3D
{
public:
    //Triangle() {}
    Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m):
        A(a), B(b), C(c) { Mat = m; }

    virtual bool intersect(const Ray &r, Hit &h, float tmin);

    Vec3f A, B, C;

};

#endif