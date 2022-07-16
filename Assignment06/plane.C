#include <GL/gl.h>
#include <GL/glut.h>

#include "plane.h"
#include <cmath>
#include "raytracing_stats.h"
#include "grid.h"

Plane::Plane(Vec3f &_normal, float _d, Material *_mat):
    Normal(_normal), Dist2Origin(_d)
{
    Mat = _mat;
    Bbox = nullptr;
}

bool Plane::intersect(const Ray &r, Hit &h, float tmin)
{   
    float dist_to_ray_origin = -Dist2Origin + r.getOrigin().Dot3(Normal);
    float dot = r.getDirection().Dot3(Normal);
    if (dot == 0.0f)
    {
        return false;
    }
    float t = -dist_to_ray_origin / dot;
    if (t > tmin && t < h.getT())
    {
        h.set(t, Mat, Normal, r);
        RayTracingStats::IncrementNumIntersections();
        return true;
    }
    return false;
}

void Plane::insertIntoGrid(Grid *g, Matrix *m)
{
    if (m)
    {
        Transformer = new Transform(*m, this);
        g->AddNonVoxelObject(Transformer);
    }
    else
    {
        g->AddNonVoxelObject(this);
    }
}

void Plane::paint()
{
    if (Mat)
    {
        Mat->glSetMaterial();
    }

    float BIG = 500000;
    Vec3f origin = Dist2Origin * Normal;
    Vec3f v(1.0f, 0.0f, 0.0f);
    if (Normal.y() == 0.0f && Normal.z() == 0.0f)
    {
        v = Vec3f(0.0f, 1.0f, 0.0f);
    }
    Vec3f b1, b2;
    Vec3f::Cross3(b1, v, Normal);
    Vec3f::Cross3(b2, Normal, b1);
    b1.Normalize();
    b2.Normalize();
    Vec3f p0 = origin - BIG * b1 - BIG * b2;
    Vec3f p1 = origin - BIG * b1 + BIG * b2;
    Vec3f p2 = origin + BIG * b1 + BIG * b2;
    Vec3f p3 = origin + BIG * b1 - BIG * b2;

    glBegin(GL_QUADS);
    glNormal3f(Normal.x(), Normal.y(), Normal.z());
    glVertex3f(p0.x(), p0.y(), p0.z());
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glVertex3f(p3.x(), p3.y(), p3.z());
    glEnd();
}
