#include <GL/gl.h>
#include <GL/glut.h>

#include "triangle.h"
#include <cmath>
#include <iostream>
#include "matrix.h"
#include "vectors.h"

float _TRIANGLE_det3x3(float a1,float a2,float a3,
	    float b1,float b2,float b3,
	    float c1,float c2,float c3);

float _TRIANGLE_det2x2(float a, float b,
	    float c, float d);

Triangle::Triangle(Vec3f &a, Vec3f &b, Vec3f &c, Material *m):
    A(a), B(b), C(c)
{
    Mat = m;
    Type = ObjectType::TRIANGLE;
    Vec3f bbox_min(
        min(min(a.x(), b.x()), c.x()),
        min(min(a.y(), b.y()), c.y()),
        min(min(a.z(), b.z()), c.z())
    );
    Vec3f bbox_max(
        max(max(a.x(), b.x()), c.x()),
        max(max(a.y(), b.y()), c.y()),
        max(max(a.z(), b.z()), c.z())
    );
    Bbox = new BoundingBox(bbox_min, bbox_max);
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin)
{
    //Barycentric Intersection
    Vec3f AC = C - A, AB = B - A;
    Vec3f normal;
    Vec3f::Cross3(normal, AB, AC);
    normal.Normalize();

    float beta, gamma, t;
    Vec3f barycenter = (A+B+C) * 0.33333333f,
        rd = r.getDirection(), ro = r.getOrigin();// - barycenter;

    float det_co = _TRIANGLE_det3x3(
        A.x() - B.x(),  A.x() - C.x(),  rd.x(),
        A.y() - B.y(),  A.y() - C.y(),  rd.y(),
        A.z() - B.z(),  A.z() - C.z(),  rd.z()
    );
    if (det_co == 0.0f)
    {
        return false;
    }

    float det_beta = _TRIANGLE_det3x3(
        A.x() - ro.x(), A.x() - C.x(),  rd.x(),
        A.y() - ro.y(), A.y() - C.y(),  rd.y(),
        A.z() - ro.z(), A.z() - C.z(),  rd.z()
    );
    float det_gamma = _TRIANGLE_det3x3(
        A.x() - B.x(),  A.x() - ro.x(), rd.x(),
        A.y() - B.y(),  A.y() - ro.y(), rd.y(),
        A.z() - B.z(),  A.z() - ro.z(), rd.z()
    );
    float det_T = _TRIANGLE_det3x3(
        A.x() - B.x(),  A.x() - C.x(),  A.x() - ro.x(),
        A.y() - B.y(),  A.y() - C.y(),  A.y() - ro.y(),
        A.z() - B.z(),  A.z() - C.z(),  A.z() - ro.z()
    );
    
    beta = det_beta / det_co;
    gamma = det_gamma / det_co;
    t = det_T / det_co;
    
    if (beta + gamma <= 1 && beta >= 0 && gamma >= 0 &&     // FIXED
        t > tmin && t < h.getT())
    {
        h.set(t, Mat, normal, r);
        //std::cout << beta << " " << gamma << " " << t << " " << normal << std::endl;
        return true;
    }
    return false;
}

void Triangle::paint()
{
    if (Mat)
    {
        Mat->glSetMaterial();
    }

    Vec3f AC = C - A, AB = B - A;
    Vec3f normal;
    Vec3f::Cross3(normal, AB, AC);
    normal.Normalize();

    glBegin(GL_TRIANGLES);
    glNormal3f(normal.x(), normal.y(), normal.z());
    glVertex3f(A.x(), A.y(), A.z());
    glVertex3f(B.x(), B.y(), B.z());
    glVertex3f(C.x(), C.y(), C.z());
    glEnd();
    
}

float _TRIANGLE_det3x3(float a1,float a2,float a3,
    float b1,float b2,float b3,
    float c1,float c2,float c3) 
{
    return
      a1 * _TRIANGLE_det2x2( b2, b3, c2, c3 )
    - b1 * _TRIANGLE_det2x2( a2, a3, c2, c3 )
    + c1 * _TRIANGLE_det2x2( a2, a3, b2, b3 );
}

float _TRIANGLE_det2x2(float a, float b,
    float c, float d) 
{
    return a * d - b * c;
}

void Triangle::insertIntoGrid(Grid *g, Matrix *m)
{
    Vec3f bbox_min = Bbox->getMin(), bbox_max = Bbox->getMax();
    if (m)
    {
        m->Transform(A);     
        m->Transform(B);     
        m->Transform(C); 
        bbox_min = Vec3f(
            min(min(A.x(), B.x()), C.x()),
            min(min(A.y(), B.y()), C.y()),
            min(min(A.z(), B.z()), C.z())
        );
        bbox_max = Vec3f(
            max(max(A.x(), B.x()), C.x()),
            max(max(A.y(), B.y()), C.y()),
            max(max(A.z(), B.z()), C.z())
        );
    }
    
    int x0, y0, z0, x1, y1, z1;
    g->GetIndexFromInternalPoint(Vec3f(-1.0f, -1.0f, -1.0f), bbox_min, x0, y0, z0);
    g->GetIndexFromInternalPoint(Vec3f(1.0f, 1.0f, 1.0f), bbox_max, x1, y1, z1);
    //std::cout << bbox_min << " " << x0 << " " << y0 << " " << z0 << std::endl;
    //std::cout << bbox_max << " " << x1 << " " << y1 << " " << z1 << std::endl;
    for (int i=x0; i<=x1; i++)
    {
        for (int j=y0; j<=y1; j++)
        {
            for (int k=z0; k<=z1; k++)
            {
                g->AddObject(i, j, k, this);
            }
        }
    }
    
}