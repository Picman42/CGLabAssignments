#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

//#include <GL/gl.h>
#include "GL/freeglut.h"

#include "ray.h"
#include "hit.h"
#include "material.h"
#include "boundingbox.h"
#include "matrix.h"

enum ObjectType
{
    TRIANGLE,
    OTHER
};

class Object3D
{
public:
    Object3D(): Mat(nullptr), Bbox(nullptr), Type(ObjectType::OTHER) {}
    ~Object3D() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual void paint() = 0;

    BoundingBox* getBoundingBox() const { return Bbox; }
    ObjectType getType() const { return Type; }
    virtual void insertIntoGrid(Grid *g, Matrix *m);

    Material *Mat;
    BoundingBox *Bbox;
    ObjectType Type;
};

#endif