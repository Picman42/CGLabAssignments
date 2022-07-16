#ifndef _CURVE_H_
#define _CURVE_H_

#include "spline.h"
#include "matrix.h"
#include <vector>

class Curve: public Spline
{
public:
    Curve(int n_vertices=0);
    // FOR VISUALIZATION
    virtual void Paint(ArgParser *args);

    // FOR CONVERTING BETWEEN SPLINE TYPES
    virtual void OutputBezier(FILE *file);
    virtual void OutputBSpline(FILE *file);

    // FOR CONTROL POINT PICKING
    virtual int getNumVertices() { return NumVertices; }
    virtual Vec3f getVertex(int i) { return Vertices[i]; }

    // FOR EDITING OPERATIONS
    virtual void moveControlPoint(int selectedPoint, float x, float y);
    virtual void addControlPoint(int selectedPoint, float x, float y) {}
    virtual void deleteControlPoint(int selectedPoint) {}

    // FOR GENERATING TRIANGLES
    virtual TriangleMesh* OutputTriangles(ArgParser *args);

    virtual void set(int index, Vec3f v);

    virtual Vec3f Calculate(float t) const = 0;

    int NumVertices;
    vector<Vec3f> Vertices;
    //Vec3f *Vertices;
    Matrix B;
};

class BezierCurve: public Curve
{
public:
    BezierCurve(int n_vertices=0);

    virtual Vec3f Calculate(float t) const;
};

class BSplineCurve: public Curve
{
public:
    BSplineCurve(int n_vertices=0);

    virtual void addControlPoint(int selectedPoint, float x, float y);
    virtual void deleteControlPoint(int selectedPoint);

    virtual Vec3f Calculate(float t) const;
};

#endif