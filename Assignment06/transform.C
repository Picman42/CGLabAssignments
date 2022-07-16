#include "transform.h"
#include "triangle.h"
#include <cmath>
#include <iostream>

Transform::Transform(Matrix &m, Object3D *o):
    M(m), Object(o)
{
    Mat = nullptr;
    if (o->getType() != ObjectType::TRIANGLE)
    {
        BoundingBox *bbox_o = o->getBoundingBox();
        Vec3f bbox_o_min = bbox_o->getMin();
        Vec3f bbox_o_max = bbox_o->getMax();
        float xmin = bbox_o_min.x(), ymin = bbox_o_min.y(),
            zmin = bbox_o_min.z(), xmax = bbox_o_max.x(), 
            ymax = bbox_o_max.y(), zmax = bbox_o_max.z();
        Vec3f v0(xmin, ymin, zmin), v1(xmax, ymin, zmin),
            v2(xmin, ymax, zmin), v3(xmin, ymin, zmax),
            v4(xmin, ymax, zmax), v5(xmax, ymin, zmax),
            v6(xmax, ymax, zmin), v7(xmax, ymax, zmax);
        M.Transform(v0);    M.Transform(v1);    M.Transform(v2);
        M.Transform(v3);    M.Transform(v4);    M.Transform(v5);
        M.Transform(v6);    M.Transform(v7);
        xmin = min(
            min(    min(v0.x(), v1.x()), min(v2.x(), v3.x())    ),
            min(    min(v4.x(), v5.x()), min(v6.x(), v7.x())    )
        );
        ymin = min(
            min(    min(v0.y(), v1.y()), min(v2.y(), v3.y())    ),
            min(    min(v4.y(), v5.y()), min(v6.y(), v7.y())    )
        );
        zmin = min(
            min(    min(v0.z(), v1.z()), min(v2.z(), v3.z())    ),
            min(    min(v4.z(), v5.z()), min(v6.z(), v7.z())    )
        );
        xmax = max(
            max(    max(v0.x(), v1.x()), max(v2.x(), v3.x())    ),
            max(    max(v4.x(), v5.x()), max(v6.x(), v7.x())    )
        );
        ymax = max(
            max(    max(v0.y(), v1.y()), max(v2.y(), v3.y())    ),
            max(    max(v4.y(), v5.y()), max(v6.y(), v7.y())    )
        );
        zmax = max(
            max(    max(v0.z(), v1.z()), max(v2.z(), v3.z())    ),
            max(    max(v4.z(), v5.z()), max(v6.z(), v7.z())    )
        );
        Vec3f bbox_min(xmin, ymin, zmin), bbox_max(xmax, ymax, zmax);
        Bbox = new BoundingBox(bbox_min, bbox_max);
    }
    else
    {
        Triangle *tri = dynamic_cast<Triangle*>(o);
        Vec3f a = tri->A, b = tri->B, c = tri->C;
        M.Transform(a);     
        M.Transform(b);     
        M.Transform(c); 
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
}

bool Transform::intersect(const Ray &r, Hit &h, float tmin)
{
    Hit h_original = h;
    Vec3f ro = r.getOrigin(), rd = r.getDirection();
    Vec4f ro_homo = Vec4f(ro.x(), ro.y(), ro.z(), 1.0f);
    Vec4f rd_homo = Vec4f(rd.x(), rd.y(), rd.z(), 0.0f);
    Matrix M_inv(M);
    M_inv.Inverse();
    M_inv.Transform(ro_homo);
    M_inv.Transform(rd_homo);
    Vec3f ro_tr = Vec3f(ro_homo.x(), ro_homo.y(), ro_homo.z());
    Vec3f rd_tr = Vec3f(rd_homo.x(), rd_homo.y(), rd_homo.z());
    float dir_scale = rd_tr.Length();
    rd_tr.Normalize();
    Ray ray_tr(ro_tr, rd_tr);
    h.set(h.getT() * dir_scale, h.getMaterial(), h.getNormal(), r);
    bool hit = Object->intersect(ray_tr, h, tmin);
    
    if (hit)
    {
        Vec3f n = h.getNormal();
        Vec4f n_homo = Vec4f(n.x(), n.y(), n.z(), 1.0f);
        Matrix M_inv_t(M_inv);
        M_inv_t.Transpose();
        M_inv_t.Transform(n_homo);
        Vec3f n_tr = Vec3f(n_homo.x(), n_homo.y(), n_homo.z());
        n_tr.Normalize();
        float t_tr = h.getT() / dir_scale;
        h.set(t_tr, h.getMaterial(), n_tr, r);      //FIXED
        return true;
    }
    h = h_original;
    return false;
}

void Transform::insertIntoGrid(Grid *g, Matrix *m)
{
    Matrix m2(M);
    if (m)
    {
        m2 = *m * M;
    }
    Object->insertIntoGrid(g, &m2);
    /*
    Vec3f bbox_min = Bbox->getMin(), bbox_max = Bbox->getMax();
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
    */
}

void Transform::paint()
{
    glPushMatrix();
    GLfloat *glMatrix = M.glGet();
    glMultMatrixf(glMatrix);
    delete[] glMatrix;
    Object->paint();
    glPopMatrix();
}