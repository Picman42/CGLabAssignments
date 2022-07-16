#ifndef _PLANE_H_
#define _PLANE_H_

#include "object3d.h"
#include "vectors.h"
#include "matrix.h"
#include "material.h"
#include "transform.h"

class Plane: public Object3D
{
public:
    //Plane() {}
    Plane(Vec3f &_normal, float _d, Material *_mat);

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void insertIntoGrid(Grid *g, Matrix *m);
    virtual void paint();

    Vec3f Normal;
    float Dist2Origin;
    //Transform *Transformer;
    //Plane *PlaneTransformed;
};

#endif