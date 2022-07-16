#include "group.h"
#include <iostream>

Group::Group(int _n): ObjectNumber(_n)
{
    Objects = new Object3D*[_n];
}

Group::~Group()
{
    delete[] Objects;
}

void Group::addObject(int index, Object3D *obj)
{
    Objects[index] = obj;
}

bool Group::intersect(const Ray &r, Hit &h, float tmin)
{
    bool ret_hit = false;
    for (int i=0; i<ObjectNumber; i++)
    {   
        //if (r.getOrigin().x() <= 0.002f && r.getOrigin().x() >= -0.002f && r.getOrigin().y() <= 0.002f && r.getOrigin().y() >= -0.002f) std::cout << i << std::endl;
        Object3D *obj = Objects[i];
        bool hit = obj->intersect(r, h, tmin);
        if (hit && !ret_hit)
        {
            ret_hit = true;
            //if (r.getOrigin().x() <= 0.002f && r.getOrigin().x() >= -0.002f && r.getOrigin().y() <= 0.002f && r.getOrigin().y() >= -0.002f) std::cout << r.getDirection() << " " << i << " " << h << std::endl;
            //if (r.getDirection().x() <= -0.2588f && r.getDirection().x() >= -0.2589f) std::cout << i << " " << h << std::endl;
        }
    }
    return ret_hit;
}

bool Group::intersectShadow(const Ray &r, Hit &h, float tmin)
{
    // Only check for one intersection
    bool ret_hit = false;
    for (int i=0; i<ObjectNumber; i++)
    {   
        Object3D *obj = Objects[i];
        bool hit = obj->intersect(r, h, tmin);
        if (hit && !ret_hit)
        {
            ret_hit = true;
            break;
        }
    }
    return ret_hit;
}

void Group::paint()
{
    for (int i=0; i<ObjectNumber; i++)
    {
        Object3D *obj = Objects[i];
        obj->paint();
    }
}