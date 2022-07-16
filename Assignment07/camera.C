#include "camera.h"
#include "matrix.h"
#include <cmath>
#include <iostream>

OrthographicCamera::OrthographicCamera(Vec3f _center, Vec3f _direction, Vec3f _up, float _img_size):
    Center(_center), Direction(_direction), Up(_up), ImageSize(_img_size)
{
    Direction.Normalize();
    //Up = Up - Direction * Up.Dot3(Direction);
    Up.Normalize();

    Vec3f horizontal;
    Vec3f::Cross3(horizontal, Direction, Up);
    Vec3f::Cross3(ScreenUp, horizontal, Direction);
    ScreenUp.Normalize();
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
    Vec3f horizonal, origin;
    Vec3f::Cross3(horizonal, Direction, Up);
    horizonal.Normalize();
    origin = Center + (point.x() - 0.5f) * ImageSize * horizonal + 
        (point.y() - 0.5f) * ImageSize * ScreenUp;

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
    //Up = Up - Direction * Up.Dot3(Direction);
    Up.Normalize();

    Vec3f horizontal;
    Vec3f::Cross3(horizontal, Direction, Up);
    Vec3f::Cross3(ScreenUp, horizontal, Direction);
    ScreenUp.Normalize();
}

Ray PerspectiveCamera::generateRay(Vec2f point)
{
    Vec3f horizonal;
    Vec3f::Cross3(horizonal, Direction, Up);
    horizonal.Normalize();
    float isosceles_height = 0.5f / tan(FOVAngle * 0.5f);
    //float dist2vplane = sqrt(isosceles_height * isosceles_height - 0.25f);
    // FIXED
    Vec3f dir = isosceles_height * Direction + 
        ScreenUp * (point.y() - 0.5f) + horizonal * (point.x() - 0.5f);
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

// ====================================================================
// Create an orthographic camera with the appropriate dimensions that
// crops the screen in the narrowest dimension.
// ====================================================================

void OrthographicCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w > h)
    glOrtho(-ImageSize/2.0, ImageSize/2.0, -ImageSize*(float)h/(float)w/2.0, ImageSize*(float)h/(float)w/2.0, 0.5, 40.0);
  else
    glOrtho(-ImageSize*(float)w/(float)h/2.0, ImageSize*(float)w/(float)h/2.0, -ImageSize/2.0, ImageSize/2.0, 0.5, 40.0);
}

// ====================================================================
// Place an orthographic camera within an OpenGL scene
// ====================================================================

void OrthographicCamera::glPlaceCamera(void)
{
  gluLookAt(Center.x(), Center.y(), Center.z(),
            Center.x()+Direction.x(), Center.y()+Direction.y(), Center.z()+Direction.z(),
            Up.x(), Up.y(), Up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void OrthographicCamera::dollyCamera(float dist)
{
  Center += Direction*dist;
  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void OrthographicCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, Direction, Up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, Direction);

  Center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void OrthographicCamera::rotateCamera(float rx, float ry)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, Direction, Up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(Up.Dot3(Direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(Up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(Center);
  rotMat.TransformDirection(Direction);
  
  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// ====================================================================

// ====================================================================
// Create a perspective camera with the appropriate dimensions that
// crops or stretches in the x-dimension as necessary
// ====================================================================

void PerspectiveCamera::glInit(int w, int h)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOVAngle*180.0/3.14159, (float)w/float(h), 0.5, 40.0);
}

// ====================================================================
// Place a perspective camera within an OpenGL scene
// ====================================================================

void PerspectiveCamera::glPlaceCamera(void)
{
  gluLookAt(Center.x(), Center.y(), Center.z(),
            Center.x()+Direction.x(), Center.y()+Direction.y(), Center.z()+Direction.z(),
            Up.x(), Up.y(), Up.z());
}

// ====================================================================
// dollyCamera, truckCamera, and RotateCamera
//
// Asumptions:
//  - up is really up (i.e., it hasn't been changed
//    to point to "screen up")
//  - up and direction are normalized
// Special considerations:
//  - If your constructor precomputes any vectors for
//    use in 'generateRay', you will likely to recompute those
//    values at athe end of the these three routines
// ====================================================================

// ====================================================================
// dollyCamera: Move camera along the direction vector
// ====================================================================

void PerspectiveCamera::dollyCamera(float dist)
{
  Center += Direction*dist;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// truckCamera: Translate camera perpendicular to the direction vector
// ====================================================================

void PerspectiveCamera::truckCamera(float dx, float dy)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, Direction, Up);
  horizontal.Normalize();

  Vec3f screenUp;
  Vec3f::Cross3(screenUp, horizontal, Direction);

  Center += horizontal*dx + screenUp*dy;

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}

// ====================================================================
// rotateCamera: Rotate around the up and horizontal vectors
// ====================================================================

void PerspectiveCamera::rotateCamera(float rx, float ry)
{
  Vec3f horizontal;
  Vec3f::Cross3(horizontal, Direction, Up);
  horizontal.Normalize();

  // Don't let the model flip upside-down (There is a singularity
  // at the poles when 'up' and 'direction' are aligned)
  float tiltAngle = acos(Up.Dot3(Direction));
  if (tiltAngle-ry > 3.13)
    ry = tiltAngle - 3.13;
  else if (tiltAngle-ry < 0.01)
    ry = tiltAngle - 0.01;

  Matrix rotMat = Matrix::MakeAxisRotation(Up, rx);
  rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

  rotMat.Transform(Center);
  rotMat.TransformDirection(Direction);
  Direction.Normalize();

  // ===========================================
  // ASSIGNMENT 3: Fix any other affected values
  // ===========================================


}