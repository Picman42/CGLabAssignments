#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "object3d.h"
#include "vectors.h"
#include "matrix.h"
#include "material.h"

class Transform: public Object3D
{
public:
    Transform() {}
    Transform(Matrix &m, Object3D *o);

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void insertIntoGrid(Grid *g, Matrix *m);
    virtual void paint();

    Matrix M;
    Object3D *Object;
};

#endif