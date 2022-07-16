#include <GL/gl.h>
#include <GL/glut.h>

#include "sphere.h"
#include <cmath>
#include <iostream>
#include "raytracing_stats.h"

Sphere::Sphere(Vec3f _center, float _radius, Material* _mat):
    Center(_center), Radius(_radius)
{
    Mat = _mat;
    Vec3f bbox_min(_center.x()-Radius, _center.y()-Radius, _center.z()-Radius);
    Vec3f bbox_max(_center.x()+Radius, _center.y()+Radius, _center.z()+Radius);
    Bbox = new BoundingBox(bbox_min, bbox_max);
}

bool Sphere::intersect(const Ray &r, Hit &h, float tmin)
{   
    float t;
    //Vec3f ActualOrigin = r.getOrigin() - tmin * r.getDirection();
    Vec3f Ro = r.getOrigin() - Center;
    float dist2 = Ro.Dot3(Ro);          //Squared distance to ray origin
    Vec3f Rd = r.getDirection();
    float tp = -Ro.Dot3(Rd);            //Closest point to sphere center
    //If origin outside and tp < 0: no hit
    /*
    if (dist2 > Radius * Radius && tp < 0)
    {
        return false;
    }
    */
    float d2 = dist2 - tp * tp;   //Squared distance to tp point
    if (d2 > Radius * Radius)
    {
        return false;
    }
    float dt = sqrt(Radius * Radius - d2);
    /*
    if (dist2 <= Radius * Radius)        //inside
    {
        t = tp + dt;
    }
    else
    {
        t = tp - dt;
    }
    */
    
    t = tp - dt;
    if (t > tmin && t < h.getT())
    {
        Vec3f p = r.pointAtParameter(t);
        Vec3f normal = p - Center;
        normal.Normalize();
        h.set(t, Mat, normal, r);
        RayTracingStats::IncrementNumIntersections();
        return true;
    }
    t = tp + dt;
    if (t > tmin && t < h.getT())
    {
        Vec3f p = r.pointAtParameter(t);
        Vec3f normal = p - Center;
        normal.Normalize();
        h.set(t, Mat, normal, r);
        RayTracingStats::IncrementNumIntersections();
        return true;
    }
    return false;
}

extern int tessel_theta_steps;
extern int tessel_phi_steps;
extern bool gouraud;

void Sphere::paint()
{
    if (Mat)
    {
        Mat->glSetMaterial();
    }
    glBegin(GL_QUADS);
    for (int iPhi=0; iPhi<tessel_phi_steps; iPhi++)
    {
        for (int iTheta=0; iTheta<tessel_theta_steps; iTheta++) {
            // compute appropriate coordinates & normals
            float theta0 = M_PI * 2.0f / tessel_theta_steps * iTheta;
            float theta1 = M_PI * 2.0f / tessel_theta_steps * (iTheta+1);
            float phi0 = M_PI / tessel_phi_steps * iPhi;
            float phi1 = M_PI / tessel_phi_steps * (iPhi+1);
            float cx = Center.x(), cy = Center.y(), cz = Center.z();

            float x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3;
            x0 = cx + Radius * sin(phi0) * cos(theta0);
            x1 = cx + Radius * sin(phi0) * cos(theta1);
            x2 = cx + Radius * sin(phi1) * cos(theta1);
            x3 = cx + Radius * sin(phi1) * cos(theta0);
            y0 = y1 = cy + Radius * cos(phi0);
            y2 = y3 = cy + Radius * cos(phi1);
            z0 = cz + Radius * sin(phi0) * sin(theta0);
            z1 = cz + Radius * sin(phi0) * sin(theta1);
            z2 = cz + Radius * sin(phi1) * sin(theta1);
            z3 = cz + Radius * sin(phi1) * sin(theta0);

            Vec3f normal;
            Vec3f p0(x0, y0, z0), p1(x1, y1, z1), p2(x2, y2, z2), p3(x3, y3, z3);
            if (!gouraud)
            {
                if (iPhi > 0)
                {
                    Vec3f::Cross3(normal, p1-p0, p3-p0);
                }
                else
                {
                    Vec3f::Cross3(normal, p3-p2, p1-p2);
                }
                normal.Normalize();

                // send gl vertex commands
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(x0, y0, z0);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);
            }
            else
            {
                normal = p0 - Center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(x0, y0, z0);
                normal = p1 - Center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(x1, y1, z1);
                normal = p2 - Center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(x2, y2, z2);
                normal = p3 - Center;
                normal.Normalize();
                glNormal3f(normal.x(), normal.y(), normal.z());
                glVertex3f(x3, y3, z3);
            }
            
        }
    }
    glEnd();
}

void Sphere::insertIntoGrid(Grid *g, Matrix *m)
{
    if (m)
    {
        Object3D::insertIntoGrid(g, m);
        return;
    }
    //std::cout << "Sphere inserting..." << std::endl;
    for (int x=0; x < g->NumX; x++)
    {
        for (int y=0; y < g->NumY; y++)
        {
            for (int z=0; z < g->NumZ; z++)
            {
                Vec3f block_center = g->GetBlockCenter(x, y, z);
                float dist = (block_center - Center).Length();
                Vec3f block_size = g->GetBlockSize();
                if (dist - block_size.Length() * 0.5f <= Radius)
                {
                    //g->Set(x, y, z);
                    g->AddObject(x, y, z, this);
                }
            }
        }
    }
}

