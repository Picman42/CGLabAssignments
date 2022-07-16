#include "group.h"

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
        
        Object3D *obj = Objects[i];
        bool hit = obj->intersect(r, h, tmin);
        if (hit && !ret_hit)
        {
            ret_hit = true;
        }
    }
    return ret_hit;
}