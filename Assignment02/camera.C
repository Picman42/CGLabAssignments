#include "camera.h"
#include <cmath>
#include <iostream>

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

PerspectiveCamera::PerspectiveCamera(Vec3f &_center, Vec3f &_direction, Vec3f &_up, float _angle):
    Center(_center), Direction(_direction), Up(_up), FOVAngle(_angle)
{
    Direction.Normalize();
    Up = Up - Direction * Up.Dot3(Direction);
    Up.Normalize();
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
    Vec3f horizonal;
    Vec3f::Cross3(horizonal, Direction, Up);
    float isosceles_height = 0.5f / tan(FOVAngle * 0.5f);
    float dist2vplane = sqrt(isosceles_height * isosceles_height - 0.25f);
    Vec3f dir = dist2vplane * Direction + 
        Up * (point.y() - 0.5f) + horizonal * (point.x() - 0.5f);
    dir.Normalize();
    /*
    if (point.x() == 0.5f && point.y() == 0.5f || point.x() < 0.002f && point.y() < 0.002f)
    {
        std::cout << isosceles_height << " " << dist2vplane << std::endl;
        std::cout << point.x() << " " << point.y() << " " << dir << std::endl;
    }
    */
    return Ray(Center, dir);
}


float PerspectiveCamera::getTMin() const
{
    return 0;
}