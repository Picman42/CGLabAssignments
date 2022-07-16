#include "camera.h"

OrthographicCamera::OrthographicCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _img_size):
    Center(_center), Direction(_direction), Up(_up), ImageSize(_img_size)
{
    Direction.Normalize();
    Up = Up - Direction * Up.Dot3(Direction);
    Up.Normalize();
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
    Vec3f horizonal, origin;
    Vec3f::Cross3(horizonal, Direction, Up);
    origin = Center + (point.x() - 0.5f) * ImageSize * horizonal + 
        (point.y() - 0.5f) * ImageSize * Up;

    return Ray(origin, Direction);
}

float OrthographicCamera::getTMin() const
{
    return -99999999.0f;
}