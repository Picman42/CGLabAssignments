#include "object3d.h"
#include "grid.h"

void Object3D::insertIntoGrid(Grid *g, Matrix *m)
{
    Vec3f bbox_min = Bbox->getMin(), bbox_max = Bbox->getMax();
    if (m)
    {
        Vec3f bbox_o_min = Bbox->getMin();
        Vec3f bbox_o_max = Bbox->getMax();
        float xmin = bbox_o_min.x(), ymin = bbox_o_min.y(),
            zmin = bbox_o_min.z(), xmax = bbox_o_max.x(), 
            ymax = bbox_o_max.y(), zmax = bbox_o_max.z();
        Vec3f v0(xmin, ymin, zmin), v1(xmax, ymin, zmin),
            v2(xmin, ymax, zmin), v3(xmin, ymin, zmax),
            v4(xmin, ymax, zmax), v5(xmax, ymin, zmax),
            v6(xmax, ymax, zmin), v7(xmax, ymax, zmax);
        m->Transform(v0);    m->Transform(v1);    m->Transform(v2);
        m->Transform(v3);    m->Transform(v4);    m->Transform(v5);
        m->Transform(v6);    m->Transform(v7);
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
        bbox_min = Vec3f(xmin, ymin, zmin);
        bbox_max = Vec3f(xmax, ymax, zmax);
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