#include "curve.h"
#include "surface.h"
#include <cmath>
#include <iostream>

#include <GL/gl.h>
#include <GL/glut.h>

Curve::Curve(int n_vertices): NumVertices(n_vertices)
{
    Vertices = vector<Vec3f>(n_vertices);
}

void Curve::Paint(ArgParser *args)
{
    //std::cout << NumVertices << std::endl;

    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0, 0, 1.0f);
    for (int i=0; i<NumVertices-1; i++)
    {
        Vec3f v0 = Vertices[i], v1 = Vertices[i+1];
        glVertex3f(v0.x(), v0.y(), v0.z());
        glVertex3f(v1.x(), v1.y(), v1.z());
    }
    glEnd();

    float step = 1.0f / (args->curve_tessellation * (NumVertices-3));
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(0, 1.0f, 0);
    for (float t = 0; t < 1.0f - 0.0001f; t += step)
    {
        Vec3f v0 = Calculate(t), v1 = Calculate(t + step);
        glVertex3f(v0.x(), v0.y(), v0.z());
        glVertex3f(v1.x(), v1.y(), v1.z());
    }
    glEnd();
    
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0, 0);
    for (int i=0; i<NumVertices; i++)
    {
        Vec3f v = Vertices[i];
        glVertex3f(v.x(), v.y(), v.z());
    }
    glEnd();

}

void Curve::OutputBezier(FILE *file)
{
    if (!file)
    {
        return;
    }
    if (NumVertices < 4)
    {
        std::cout << "Cannot output Bezier curve: vertices fewer than 4." << std::endl;
        return;
    }
    float *b = new float[16] {
        -1,  3, -3,  1,
         3, -6,  3,  0,
        -3,  3,  0,  0,
         1,  0,  0,  0
    };
    bool is_same_B = true;
    for (int i=0; i<16; i++)
    {
        if (b[i] != B.Get(i % 4, i / 4))
        {
            is_same_B = false;
            break;
        }
    }
    if (is_same_B)
    {
        delete[] b;
        fprintf(file, "bezier\n");
        fprintf(file, "num_vertices %d\n", NumVertices);
        for (int i=0; i<NumVertices; i++)
        {
            fprintf(file, "%f %f %f\n", 
                Vertices[i].x(), Vertices[i].y(), Vertices[i].z());
        }
        return;
    }

    Matrix G;
    for (int i=0; i<4; i++)
    {
        Vec3f v = Vertices[i];
        G.Set(i, 0, v.x());
        G.Set(i, 1, v.y());
        G.Set(i, 2, v.z());
        G.Set(i, 3, 0);
    }
    Matrix B_bezier_inv(b);
    delete[] b;
    B_bezier_inv.Inverse();
    Matrix G_bezier = G * B * B_bezier_inv;

    fprintf(file, "bezier\n");
    fprintf(file, "num_vertices 4\n");
    for (int i=0; i<4; i++)
    {
        fprintf(file, "%f %f %f\n", 
            G_bezier.Get(i, 0), G_bezier.Get(i, 1), G_bezier.Get(i, 2));
    }

}

void Curve::OutputBSpline(FILE *file)
{
    if (!file)
    {
        return;
    }
    if (NumVertices < 4)
    {
        std::cout << "Cannot output Bezier curve: vertices fewer than 4." << std::endl;
        return;
    }
    float *b = new float[16] {
        -1.0f/6.0f,  0.5f, -0.5f,  1.0f/6.0f,
         3.0f/6.0f, -1.0f,  0.0f,  4.0f/6.0f,
        -3.0f/6.0f,  0.5f,  0.5f,  1.0f/6.0f,
         1.0f/6.0f,  0.0f,  0.0f,  0.0f,
    };
    bool is_same_B = true;
    for (int i=0; i<16; i++)
    {
        if (b[i] != B.Get(i % 4, i / 4))
        {
            is_same_B = false;
            break;
        }
    }
    if (is_same_B)
    {
        delete[] b;
        fprintf(file, "bspline\n");
        fprintf(file, "num_vertices %d\n", NumVertices);
        for (int i=0; i<NumVertices; i++)
        {
            fprintf(file, "%f %f %f\n", 
                Vertices[i].x(), Vertices[i].y(), Vertices[i].z());
        }
        return;
    }

    Matrix G;
    for (int i=0; i<4; i++)
    {
        Vec3f v = Vertices[i];
        G.Set(i, 0, v.x());
        G.Set(i, 1, v.y());
        G.Set(i, 2, v.z());
        G.Set(i, 3, 0);
    }
    
    Matrix B_bezier_inv(b);
    delete[] b;
    B_bezier_inv.Inverse();
    Matrix G_bezier = G * B * B_bezier_inv;

    fprintf(file, "bspline\n");
    fprintf(file, "num_vertices 4\n");
    for (int i=0; i<4; i++)
    {
        fprintf(file, "%f %f %f\n", 
            G_bezier.Get(i, 0), G_bezier.Get(i, 1), G_bezier.Get(i, 2));
    }

}

void Curve::set(int index, Vec3f v)
{
    if (index >= 0 && index < NumVertices)
    {
        Vertices[index] = v;
    }
}

void Curve::moveControlPoint(int selectedPoint, float x, float y)
{
    Vertices[selectedPoint] = Vec3f(x, y, 0);
}

TriangleMesh* Curve::OutputTriangles(ArgParser *args)
{
    SurfaceOfRevolution rev(this);
    return rev.OutputTriangles(args);
}

BezierCurve::BezierCurve(int n_vertices): Curve(n_vertices)
{
    float *b = new float[16] {
        -1,  3, -3,  1,
         3, -6,  3,  0,
        -3,  3,  0,  0,
         1,  0,  0,  0
    };
    B = Matrix(b);
    delete[] b;
}

Vec3f BezierCurve::Calculate(float t) const
{
    int n_segs = (NumVertices - 1) / 3;
    int seg = max(0, int(floor(t * n_segs - 0.00001f) + 0.00001f));
    //std::cout << t << " " << seg << " ";
    t = (t - (float)seg / n_segs) * n_segs;
    //std::cout << t << std::endl;

    Matrix G;
    for (int i=0; i<4; i++)
    {
        Vec3f v = Vertices[seg * 3 + i];
        G.Set(i, 0, v.x());
        G.Set(i, 1, v.y());
        G.Set(i, 2, v.z());
        G.Set(i, 3, 0);
    }
    Vec4f T(t*t*t, t*t, t, 1.0f);
    B.Transform(T);
    G.Transform(T);
    return Vec3f(T.x(), T.y(), T.z());
}

BSplineCurve::BSplineCurve(int n_vertices): Curve(n_vertices)
{
    float *b = new float[16] {
        -1.0f/6.0f,  0.5f, -0.5f,  1.0f/6.0f,
         3.0f/6.0f, -1.0f,  0.0f,  4.0f/6.0f,
        -3.0f/6.0f,  0.5f,  0.5f,  1.0f/6.0f,
         1.0f/6.0f,  0.0f,  0.0f,  0.0f,
    };
    B = Matrix(b);
    delete[] b;
}

Vec3f BSplineCurve::Calculate(float t) const
{
    int pe = min(NumVertices-1, int(t * (NumVertices-1) - 0.0001f) + 3);
    int ps = pe - 3;
    t = (t - (float)ps / (NumVertices-1)) / 
        ((float)(ps+1) / (NumVertices-1) - (float)ps / (NumVertices-1));
    t = t > 1.0f ? 1.0f : t;
    Matrix G;
    for (int i=0; i<4; i++)
    {
        Vec3f v = Vertices[ps+i];
        G.Set(i, 0, v.x());
        G.Set(i, 1, v.y());
        G.Set(i, 2, v.z());
        G.Set(i, 3, 0);
    }
    Vec4f T(t*t*t, t*t, t, 1.0f);
    B.Transform(T);
    G.Transform(T);
    return Vec3f(T.x(), T.y(), T.z());
}

void BSplineCurve::addControlPoint(int selectedPoint, float x, float y)
{
    Vertices.insert(Vertices.begin() + selectedPoint, Vec3f(x, y, 0));
    NumVertices ++;
}

void BSplineCurve::deleteControlPoint(int selectedPoint)
{
    Vertices.erase(Vertices.begin() + selectedPoint);
    NumVertices --;
}



