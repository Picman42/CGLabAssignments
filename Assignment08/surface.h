#ifndef _SURFACE_H_
#define _SURFACE_H_

#include "spline.h"
#include "curve.h"

class Surface: public Spline
{
public:
    Surface(): Mesh(nullptr) {}
    // FOR VISUALIZATION
    virtual void Paint(ArgParser *args) { C->Paint(args); }

    // FOR CONVERTING BETWEEN SPLINE TYPES
    virtual void OutputBezier(FILE *file) { C->OutputBezier(file); }
    virtual void OutputBSpline(FILE *file) { C->OutputBSpline(file); }

    // FOR CONTROL POINT PICKING
    virtual int getNumVertices() { return C->getNumVertices(); }
    virtual Vec3f getVertex(int i) { return C->getVertex(i); }

    // FOR EDITING OPERATIONS
    virtual void moveControlPoint(int selectedPoint, float x, float y)
        { C->moveControlPoint(selectedPoint, x, y); }
    virtual void addControlPoint(int selectedPoint, float x, float y)
        { C->addControlPoint(selectedPoint, x, y); }
    virtual void deleteControlPoint(int selectedPoint)
        { C->deleteControlPoint(selectedPoint); }

    // FOR GENERATING TRIANGLES
    virtual TriangleMesh* OutputTriangles(ArgParser *args) = 0;

    virtual void set(int index, Vec3f v) { C->set(index, v); }

    Curve* C;
    TriangleMesh* Mesh;
};

class SurfaceOfRevolution: public Surface
{
public:
    SurfaceOfRevolution(Curve *c) { C = c; }

    virtual TriangleMesh* OutputTriangles(ArgParser *args);

};

class BezierPatch: public Surface
{
public:
    BezierPatch();

    virtual void Paint(ArgParser *args);

    Vec3f CalculateBezier(const Vec3f &P0, const Vec3f &P1, 
        const Vec3f &P2, const Vec3f &P3, float t) const;

    virtual TriangleMesh* OutputTriangles(ArgParser *args);

    Matrix B;
};

#endif