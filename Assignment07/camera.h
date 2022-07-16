#ifndef _CAMERA_H_
#define _CAMERA_H_

//#include <GL/gl.h>
//#include <GL/glu.h>
#include "GL/freeglut.h"

#include "ray.h"
#include "vectors.h"

class Camera
{
public:
    Camera() {}
    ~Camera() {}

    virtual void glInit(int w, int h) = 0;
    virtual void glPlaceCamera(void) = 0;
    virtual void dollyCamera(float dist) = 0;
    virtual void truckCamera(float dx, float dy) = 0;
    virtual void rotateCamera(float rx, float ry) = 0;

    virtual Ray generateRay(Vec2f point) = 0;
    virtual float getTMin() const = 0;
};

class OrthographicCamera: public Camera
{
public:
    OrthographicCamera() {}
    OrthographicCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _img_size);

    virtual void glInit(int w, int h);
    virtual void glPlaceCamera(void);
    virtual void dollyCamera(float dist);
    virtual void truckCamera(float dx, float dy);
    virtual void rotateCamera(float rx, float ry);

    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const;

    Vec3f Center, Direction, Up, ScreenUp;
    float ImageSize;
};

class PerspectiveCamera: public Camera
{
public:
    PerspectiveCamera() {}
    PerspectiveCamera(Vec3f &_center, Vec3f &_direction, Vec3f &_up, float _angle);

    virtual void glInit(int w, int h);
    virtual void glPlaceCamera(void);
    virtual void dollyCamera(float dist);
    virtual void truckCamera(float dx, float dy);
    virtual void rotateCamera(float rx, float ry);

    virtual Ray generateRay(Vec2f point);
    virtual float getTMin() const;

    Vec3f Center, Direction, Up, ScreenUp;
    float FOVAngle;
};

#endif