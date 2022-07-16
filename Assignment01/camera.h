#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "ray.h"
#include "vectors.h"

class Camera
{
public:
    Camera() {}
    ~Camera() {}

    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
};

class OrthographicCamera: public Camera
{
public:
    OrthographicCamera() {}
    OrthographicCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _img_size);

    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const;

    Vec3f Center, Direction, Up;
    float ImageSize;
};

#endif