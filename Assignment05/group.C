#include "object3d.h"
#include "group.h"
#include "boundingbox.h"
#include <iostream>
#include <cmath>

Group::Group(int _n): ObjectNumber(_n)
{
    Objects = new Object3D*[_n];
    Bbox = nullptr;
}

Group::~Group()
{
    delete[] Objects;
}

void Group::addObject(int index, Object3D *obj)
{
    Objects[index] = obj;
    BoundingBox *bbox_new = obj->getBoundingBox();
    if (!Bbox)
    {
        Bbox = new BoundingBox(*bbox_new);
    }
    else if (bbox_new)
    {
        Vec3f bbox_min(
            min(Bbox->getMin().x(), bbox_new->getMin().x()),
            min(Bbox->getMin().y(), bbox_new->getMin().y()),
            min(Bbox->getMin().z(), bbox_new->getMin().z())
        );
        Vec3f bbox_max(
            max(Bbox->getMax().x(), bbox_new->getMax().x()),
            max(Bbox->getMax().y(), bbox_new->getMax().y()),
            max(Bbox->getMax().z(), bbox_new->getMax().z())
        );
        Bbox = new BoundingBox(bbox_min, bbox_max);
    }
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

void Group::insertIntoGrid(Grid *g, Matrix *m)
{
    for (int i=0; i<ObjectNumber; i++)
    {
        Object3D *obj = Objects[i];
        obj->insertIntoGrid(g, m);
    }
}

void Group::paint()
{
    for (int i=0; i<ObjectNumber; i++)
    {
        Object3D *obj = Objects[i];
        obj->paint();
    }
}