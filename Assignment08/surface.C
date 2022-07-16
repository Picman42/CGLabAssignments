#include "surface.h"
#include <cmath>

#include <GL/gl.h>
#include <GL/glut.h>

TriangleMesh* SurfaceOfRevolution::OutputTriangles(ArgParser *args)
{
    if (Mesh)
    {
        return Mesh;
    }

    int tessel_curve = args->curve_tessellation * (C->getNumVertices() - 3);
    int tessel_rev = args->revolution_tessellation;
    TriangleNet *net = new TriangleNet(tessel_curve, tessel_rev);

    for (int i=0; i<=tessel_curve; i++)
    {
        float t = 1.0f / tessel_curve * i;
        Vec3f p_curve = C->Calculate(t);
        for (int j=0; j<=tessel_rev; j++)
        {
            float angle = M_PI * 2.0f / tessel_rev * j;
            float x = p_curve.x() * sin(angle);
            float z = p_curve.x() * cos(angle);
            net->SetVertex(i, j, Vec3f(x, p_curve.y(), z));
        }
    }

    Mesh = net;
    return Mesh;
}

BezierPatch::BezierPatch()
{
    C = new BezierCurve(16);

    float *b = new float[16] {
        -1,  3, -3,  1,
         3, -6,  3,  0,
        -3,  3,  0,  0,
         1,  0,  0,  0
    };
    B = Matrix(b);
    delete[] b;
}

void BezierPatch::Paint(ArgParser *args)
{
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0, 0, 1.0f);
    for (int i = 0; i < C->getNumVertices() - 1; i++)
    {
        Vec3f v0 = C->getVertex(i), v1 = C->getVertex(i+1);
        glVertex3f(v0.x(), v0.y(), v0.z());
        glVertex3f(v1.x(), v1.y(), v1.z());
    }
    glEnd();
    
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0, 0);
    for (int i = 0; i < C->getNumVertices(); i++)
    {
        Vec3f v = C->getVertex(i);
        glVertex3f(v.x(), v.y(), v.z());
    }
    glEnd();
}

Vec3f BezierPatch::CalculateBezier(const Vec3f &P0, const Vec3f &P1, 
    const Vec3f &P2, const Vec3f &P3, float t) const
{
    float *g = new float[16] {
        P0.x(), P1.x(), P2.x(), P3.x(),
        P0.y(), P1.y(), P2.y(), P3.y(),
        P0.z(), P1.z(), P2.z(), P3.z(),
             0,      0,      0,      0
    };
    Matrix G(g);
    Vec4f T(t*t*t, t*t, t, 1.0f);
    B.Transform(T);
    G.Transform(T);
    return Vec3f(T.x(), T.y(), T.z());
}

TriangleMesh* BezierPatch::OutputTriangles(ArgParser *args)
{
    if (Mesh)
    {
        return Mesh;
    }
    int tessel_patch = args->patch_tessellation;
    TriangleNet *net = new TriangleNet(tessel_patch, tessel_patch);
    for (int i=0; i<=tessel_patch; i++)
    {
        float s = 1.0f / tessel_patch * i;
        for (int j=0; j<=tessel_patch; j++)
        {
            float t = 1.0f / tessel_patch * j;
            Vec3f PP0, PP1, PP2, PP3, P[4];
            for (int k=0; k<4; k++)
            {
                PP0 = C->getVertex(k*4 + 0);  PP1 = C->getVertex(k*4 + 1);
                PP2 = C->getVertex(k*4 + 2);  PP3 = C->getVertex(k*4 + 3);
                P[k] = CalculateBezier(PP0, PP1, PP2, PP3, t);
            }
            net->SetVertex(i, j, CalculateBezier(P[0], P[1], P[2], P[3], s));
        }
    }

    Mesh = net;
    return Mesh;

}