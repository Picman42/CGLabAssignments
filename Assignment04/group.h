#ifndef _GROUP_H_
#define _GROUP_H_

#include "object3d.h"

class Group: public Object3D
{
public:
    Group(): ObjectNumber(0) { };
    Group(int _n);
    ~Group();

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    bool intersectShadow(const Ray &r, Hit &h, float tmin);
    virtual void paint();

    void addObject(int index, Object3D *obj);

    int ObjectNumber;
    Object3D **Objects;

};

#endif