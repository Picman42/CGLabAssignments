#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

//#include <GL/gl.h>
#include "GL/freeglut.h"

#include "ray.h"
#include "hit.h"
#include "material.h"

class Object3D
{
public:
    Object3D() {}
    ~Object3D() {}

    virtual bool intersect(const Ray &r, Hit &h, float tmin) = 0;
    virtual void paint() = 0;

    Material *Mat;
};

#endif