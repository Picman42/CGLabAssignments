#include "grid.h"
#include "rayTree.h"
#include "material.h"
#include <iostream>
#include "raytracing_stats.h"

Grid::Grid(BoundingBox *bb, int _nx, int _ny, int _nz, bool visualize):
    NumX(_nx), NumY(_ny), NumZ(_nz), PalleteSize(8), Visualize(visualize)
{
    Bbox = bb;
    Voxels = new int[_nx * _ny * _nz];
    VoxelObjects = new Object3DVector[_nx * _ny * _nz];
    Pallete = new PhongMaterial[PalleteSize];
    Pallete[0] = PhongMaterial(Vec3f(1.0f, 1.0f, 1.0f));
    Pallete[1] = PhongMaterial(Vec3f(1.0f, 0.0f, 1.0f));
    Pallete[2] = PhongMaterial(Vec3f(0.0f, 0.0f, 1.0f));
    Pallete[3] = PhongMaterial(Vec3f(0.0f, 1.0f, 1.0f));
    Pallete[4] = PhongMaterial(Vec3f(0.0f, 1.0f, 0.0f));
    Pallete[5] = PhongMaterial(Vec3f(1.0f, 1.0f, 0.0f));
    Pallete[6] = PhongMaterial(Vec3f(1.0f, 0.5f, 0.0f));
    Pallete[7] = PhongMaterial(Vec3f(1.0f, 0.0f, 0.0f));

    //std::cout << "Grid: " << _nx << " " << _ny << " " << _nz << std::endl;
}

bool Grid::GetOccupied(int x, int y, int z) const
{
    if (x >= 0 && x < NumX && y >= 0 && y < NumY && z >= 0 && z < NumZ)
    {
        return Voxels[x * NumY * NumZ + y * NumZ + z];
    }
    else
    {
        return false;
    }
}

int Grid::GetObjectNumber(int x, int y, int z) const
{
    if (x >= 0 && x < NumX && y >= 0 && y < NumY && z >= 0 && z < NumZ)
    {
        return VoxelObjects[x * NumY * NumZ + y * NumZ + z].getNumObjects();
    }
    else
    {
        return 0;
    }
}

void Grid::Set(int x, int y, int z, bool occupied)
{
    if (x >= 0 && x < NumX && y >= 0 && y < NumY && z >= 0 && z < NumZ)
    {
        Voxels[x * NumY * NumZ + y * NumZ + z] = occupied;
    }
}

void Grid::AddObject(int x, int y, int z, Object3D *obj)
{
    if (x >= 0 && x < NumX && y >= 0 && y < NumY && z >= 0 && z < NumZ)
    {
        VoxelObjects[x * NumY * NumZ + y * NumZ + z].addObject(obj);
    }
}

void Grid::AddNonVoxelObject(Object3D *obj)
{
    NonVoxelObjects.addObject(obj);
}

Vec3f Grid::GetBlockSize() const
{
    return (Bbox->getMax() - Bbox->getMin()) * 
        Vec3f(1.0f / NumX, 1.0f / NumY, 1.0f / NumZ);
}

Vec3f Grid::GetBlockCenter(int x, int y, int z) const
{
    Vec3f vsize = GetBlockSize();
    return Vec3f(x+0.5f, y+0.5f, z+0.5f) * vsize + Bbox->getMin();
}

PhongMaterial* Grid::GetMaterial(int obj_num) const
{
    if (obj_num <= 0)
    {
        return nullptr;
    }
    if (obj_num > PalleteSize)
    {
        obj_num = PalleteSize;
    }
    return &Pallete[obj_num-1];
}

Vec3f Grid::GetTnextFromInternalPoint(const Ray &r, Vec3f sign, Vec3f p) const
{
    Vec3f ro = r.getOrigin();
    Vec3f rd = r.getDirection();
    Vec3f vsize = GetBlockSize();
    Vec3f cmin = Bbox->getMin(), cmax = Bbox->getMax();
    float x, y, z;
    x = sign.x() > 0 ?
        ceil((p.x() - cmin.x()) / vsize.x() + 0.00001f) * vsize.x() + cmin.x() :
        floor((p.x() - cmin.x()) / vsize.x() - 0.00001f) * vsize.x() + cmin.x();
    y = sign.y() > 0 ?
        ceil((p.y() - cmin.y()) / vsize.y() + 0.00001f) * vsize.y() + cmin.y() :
        floor((p.y() - cmin.y()) / vsize.y() - 0.00001f) * vsize.y() + cmin.y();
    z = sign.z() > 0 ?
        ceil((p.z() - cmin.z()) / vsize.z() + 0.00001f) * vsize.z() + cmin.z() :
        floor((p.z() - cmin.z()) / vsize.z() - 0.00001f) * vsize.z() + cmin.z();
    float tx = rd.x() == 0 ? INFINITY : (x - ro.x()) / rd.x();
    float ty = rd.y() == 0 ? INFINITY : (y - ro.y()) / rd.y();
    float tz = rd.z() == 0 ? INFINITY : (z - ro.z()) / rd.z();
    return Vec3f(tx, ty, tz);
}

void Grid::GetIndexFromInternalPoint(Vec3f sign, Vec3f p, 
    int &i, int &j, int &k) const
{
    Vec3f vsize = GetBlockSize();
    Vec3f cmin = Bbox->getMin(), cmax = Bbox->getMax();
    i = sign.x() > 0 ?
        (int) floor((p.x() - cmin.x()) / vsize.x() + 0.00001f) :
        (int) floor((p.x() - cmin.x()) / vsize.x() - 0.00001f);
    j = sign.y() > 0 ?
        (int) floor((p.y() - cmin.y()) / vsize.y() + 0.00001f) :
        (int) floor((p.y() - cmin.y()) / vsize.y() - 0.00001f);
    k = sign.z() > 0 ?
        (int) floor((p.z() - cmin.z()) / vsize.z() + 0.00001f) :
        (int) floor((p.z() - cmin.z()) / vsize.z() - 0.00001f);
}

void Grid::initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const
{
    Vec3f ro = r.getOrigin();
    Vec3f rd = r.getDirection();
    Vec3f ro_tmin = r.pointAtParameter(tmin);
    mi.dt = GetBlockSize() * 
        Vec3f(1.0f / fabs(rd.x()), 1.0f / fabs(rd.y()), 1.0f / fabs(rd.z()));
    mi.Sign = Vec3f((rd.x() > 0 ? 1 : -1), 
        (rd.y() > 0 ? 1 : -1), (rd.z() > 0 ? 1 : -1));
    mi.Tmin = tmin;
    mi.Tnext = Vec3f(INFINITY, INFINITY, INFINITY);
    Vec3f cmin = Bbox->getMin(), cmax = Bbox->getMax();
    
    if (ro_tmin.x() < cmin.x() || ro_tmin.x() > cmax.x() ||
        ro_tmin.y() < cmin.y() || ro_tmin.y() > cmax.y() ||
        ro_tmin.z() < cmin.z() || ro_tmin.z() > cmax.z())
    {
        // Outside!
        float tmin_final = INFINITY, t;
        Vec3f int_p, hit_p, normal;
        if (rd.x() != 0.0f)
        {
            // Left face
            t = (cmin.x() - ro.x()) / rd.x();
            int_p = r.pointAtParameter(t);
            if (t > tmin && t < tmin_final && 
                int_p.y() > cmin.y() && int_p.y() < cmax.y() &&
                int_p.z() > cmin.z() && int_p.z() < cmax.z())
            {
                tmin_final = t;
                hit_p = int_p;
                normal = Vec3f(-1.0f, 0, 0);
                //std::cout << "HIT LEFT: " << int_p << std::endl;
            }
            // Right face
            t = (cmax.x() - ro.x()) / rd.x();
            int_p = r.pointAtParameter(t);
            if (t > tmin && t < tmin_final && 
                int_p.y() > cmin.y() && int_p.y() < cmax.y() &&
                int_p.z() > cmin.z() && int_p.z() < cmax.z())
            {
                tmin_final = t;
                hit_p = int_p;
                normal = Vec3f(1.0f, 0, 0);
                //std::cout << "HIT RIGHT: " << int_p << std::endl;
            }
        }
        if (rd.y() != 0.0f)
        {
            // Down face
            t = (cmin.y() - ro.y()) / rd.y();
            int_p = r.pointAtParameter(t);
            if (t > tmin && t < tmin_final && 
                int_p.x() > cmin.x() && int_p.x() < cmax.x() &&
                int_p.z() > cmin.z() && int_p.z() < cmax.z())
            {
                tmin_final = t;
                hit_p = int_p;
                normal = Vec3f(0, -1.0f, 0);
                //std::cout << "HIT DOWN: " << int_p << std::endl;
            }
            // Up face
            t = (cmax.y() - ro.y()) / rd.y();
            int_p = r.pointAtParameter(t);
            if (t > tmin && t < tmin_final && 
                int_p.x() > cmin.x() && int_p.x() < cmax.x() &&
                int_p.z() > cmin.z() && int_p.z() < cmax.z())
            {
                tmin_final = t;
                hit_p = int_p;
                normal = Vec3f(0, 1.0f, 0);
                //std::cout << "HIT UP: " << int_p << std::endl;
            }
        }
        if (rd.z() != 0.0f)
        {
            // Back face
            t = (cmin.z() - ro.z()) / rd.z();
            int_p = r.pointAtParameter(t);
            if (t > tmin && t < tmin_final && 
                int_p.x() > cmin.x() && int_p.x() < cmax.x() &&
                int_p.y() > cmin.y() && int_p.y() < cmax.y())
            {
                tmin_final = t;
                hit_p = int_p;
                normal = Vec3f(0, 0, -1.0f);
                //std::cout << "HIT BACK: " << int_p << std::endl;
            }
            // Front face
            t = (cmax.z() - ro.z()) / rd.z();
            int_p = r.pointAtParameter(t);
            if (t > tmin && t < tmin_final && 
                int_p.x() > cmin.x() && int_p.x() < cmax.x() &&
                int_p.y() > cmin.y() && int_p.y() < cmax.y())
            {
                tmin_final = t;
                hit_p = int_p;
                normal = Vec3f(0, 0, 1.0f);
                //std::cout << "HIT FRONT: " << int_p << std::endl;
            }
        }
        if (tmin_final < INFINITY)
        {
            //std::cout << "Outside first hit: " << hit_p << std::endl;
            mi.Tnext = GetTnextFromInternalPoint(r, mi.Sign, hit_p);
            GetIndexFromInternalPoint(mi.Sign, hit_p, mi.GI, mi.GJ, mi.GK);
            mi.Normal = normal;
            mi.Tmin = tmin_final;
        }
    }
    else
    {
        // Inside
        mi.Tnext = GetTnextFromInternalPoint(r, mi.Sign, ro_tmin);
        GetIndexFromInternalPoint(mi.Sign, ro_tmin, mi.GI, mi.GJ, mi.GK);
    }
}

void Grid::paint()
{
    Vec3f vsize = GetBlockSize();
    float u = vsize.x() * 0.5f, v = vsize.y() * 0.5f, w = vsize.z() * 0.5f;
    for (int x=0; x < NumX; x++)
    {
        for (int y=0; y < NumY; y++)
        {
            for (int z=0; z < NumY; z++)
            {
                int obj_num = GetObjectNumber(x, y, z);
                if (obj_num >= 1)
                {
                    /*      7   6
                     *    4   5
                     *
                     *      3   2
                     *    0   1
                     */
                    Vec3f c = GetBlockCenter(x, y, z);
                    Vec3f v0(c.x()-u, c.y()-v, c.z()+w),
                        v1(c.x()+u, c.y()-v, c.z()+w),
                        v2(c.x()+u, c.y()-v, c.z()-w),
                        v3(c.x()-u, c.y()-v, c.z()-w),
                        v4(c.x()-u, c.y()+v, c.z()+w),
                        v5(c.x()+u, c.y()+v, c.z()+w),
                        v6(c.x()+u, c.y()+v, c.z()-w),
                        v7(c.x()-u, c.y()+v, c.z()-w);

                    PhongMaterial *mat = GetMaterial(obj_num);
                    mat->glSetMaterial();
                    
                    Vec3f n;
                    glBegin(GL_QUADS);
                    Vec3f::Cross3(n, v3-v0, v1-v0);     n.Normalize();
                    glNormal3f(n.x(), n.y(), n.z());
                    glVertex3f(v0.x(), v0.y(), v0.z());
                    glVertex3f(v1.x(), v1.y(), v1.z());
                    glVertex3f(v2.x(), v2.y(), v2.z());
                    glVertex3f(v3.x(), v3.y(), v3.z());
                    Vec3f::Cross3(n, v2-v1, v5-v1);     n.Normalize();
                    glNormal3f(n.x(), n.y(), n.z());
                    glVertex3f(v1.x(), v1.y(), v1.z());
                    glVertex3f(v2.x(), v2.y(), v2.z());
                    glVertex3f(v6.x(), v6.y(), v6.z());
                    glVertex3f(v5.x(), v5.y(), v5.z());
                    Vec3f::Cross3(n, v5-v4, v7-v4);     n.Normalize();
                    glNormal3f(n.x(), n.y(), n.z());
                    glVertex3f(v4.x(), v4.y(), v4.z());
                    glVertex3f(v5.x(), v5.y(), v5.z());
                    glVertex3f(v6.x(), v6.y(), v6.z());
                    glVertex3f(v7.x(), v7.y(), v7.z());
                    Vec3f::Cross3(n, v4-v0, v3-v0);     n.Normalize();
                    glNormal3f(n.x(), n.y(), n.z());
                    glVertex3f(v0.x(), v0.y(), v0.z());
                    glVertex3f(v3.x(), v3.y(), v3.z());
                    glVertex3f(v7.x(), v7.y(), v7.z());
                    glVertex3f(v4.x(), v4.y(), v4.z());
                    Vec3f::Cross3(n, v1-v0, v4-v0);     n.Normalize();
                    glNormal3f(n.x(), n.y(), n.z());
                    glVertex3f(v0.x(), v0.y(), v0.z());
                    glVertex3f(v1.x(), v1.y(), v1.z());
                    glVertex3f(v5.x(), v5.y(), v5.z());
                    glVertex3f(v4.x(), v4.y(), v4.z());
                    Vec3f::Cross3(n, v7-v3, v2-v3);     n.Normalize();
                    glNormal3f(n.x(), n.y(), n.z());
                    glVertex3f(v3.x(), v3.y(), v3.z());
                    glVertex3f(v2.x(), v2.y(), v2.z());
                    glVertex3f(v6.x(), v6.y(), v6.z());
                    glVertex3f(v7.x(), v7.y(), v7.z());
                    glEnd();
                }
            }
        }
    }
}

bool Grid::intersect(const Ray &r, Hit &h, float tmin)
{
    //RayTree::AddShadowSegment(r, tmin, INFINITY);
    //std::cout << "///////////////////" << std::endl;
    MarchingInfo mi;
    initializeRayMarch(mi, r, tmin);
    /*
    if (!((mi.GI >= 0 && mi.GI < NumX && mi.GJ >= 0 && mi.GJ < NumY && 
        mi.GJ >= 0 && mi.GJ < NumY)))
    {
        return false;
    }
    */
    Obj2Hit.clear();

    Vec3f vsize = GetBlockSize() + Vec3f(.0001f, .0001f, .0001f);
    float u = vsize.x() * 0.5f, v = vsize.y() * 0.5f, w = vsize.z() * 0.5f;

    Material *mat = new PhongMaterial(Vec3f(1.0f, 1.0f, 1.0f));

    bool is_hit = false;

    while (mi.GI >= 0 && mi.GI < NumX && mi.GJ >= 0 && mi.GJ < NumY && 
        mi.GK >= 0 && mi.GK < NumZ)
    {
        RayTracingStats::IncrementNumGridCellsTraversed();
        
        Vec3f c = GetBlockCenter(mi.GI, mi.GJ, mi.GK);
        Vec3f v0(c.x()-u, c.y()-v, c.z()+w),
            v1(c.x()+u, c.y()-v, c.z()+w),
            v2(c.x()+u, c.y()-v, c.z()-w),
            v3(c.x()-u, c.y()-v, c.z()-w),
            v4(c.x()-u, c.y()+v, c.z()+w),
            v5(c.x()+u, c.y()+v, c.z()+w),
            v6(c.x()+u, c.y()+v, c.z()-w),
            v7(c.x()-u, c.y()+v, c.z()-w);
            /*      7   6
             *    4   5
             *
             *      3   2
             *    0   1
             */
        RayTree::AddHitCellFace(v0, v1, v2, v3, Vec3f(0, -1.0f, 0), mat);
        RayTree::AddHitCellFace(v4, v5, v6, v7, Vec3f(0, 1.0f, 0), mat);
        RayTree::AddHitCellFace(v0, v3, v7, v4, Vec3f(-1.0f, 0, 0), mat);
        RayTree::AddHitCellFace(v1, v2, v6, v5, Vec3f(1.0f, 0, 0), mat);
        RayTree::AddHitCellFace(v0, v1, v5, v4, Vec3f(0, 0, 1.0f), mat);
        RayTree::AddHitCellFace(v3, v2, v6, v7, Vec3f(0, 0, -1.0f), mat);
        
        if (mi.Normal.x() == 1.0f)
            RayTree::AddEnteredFace(v1, v2, v6, v5, Vec3f(1.0f, 0, 0), mat);
        else if (mi.Normal.x() == -1.0f)
            RayTree::AddEnteredFace(v0, v3, v7, v4, Vec3f(-1.0f, 0, 0), mat);
        else if (mi.Normal.y() == 1.0f)
            RayTree::AddEnteredFace(v4, v5, v6, v7, Vec3f(0, 1.0f, 0), mat);
        else if (mi.Normal.y() == -1.0f)
            RayTree::AddEnteredFace(v0, v1, v2, v3, Vec3f(0, -1.0f, 0), mat);
        else if (mi.Normal.z() == 1.0f)
            RayTree::AddEnteredFace(v0, v1, v5, v4, Vec3f(0, 0, 1.0f), mat);
        else if (mi.Normal.z() == -1.0f)
            RayTree::AddEnteredFace(v3, v2, v6, v7, Vec3f(0, 0, -1.0f), mat);

        int obj_num = GetObjectNumber(mi.GI, mi.GJ, mi.GK);
        //std::cout << Vec3f(mi.GI, mi.GJ, mi.GK) << " " << obj_num << std::endl;
        if (obj_num >= 1)
        {
            if (Visualize)
            {
                h.set(mi.Tmin, GetMaterial(obj_num), mi.Normal, r);
                RayTracingStats::IncrementNumIntersections();
                return true;
            }

            Object3DVector& objs = 
                VoxelObjects[mi.GI * NumY * NumZ + mi.GJ * NumZ + mi.GK];

            for (int i=0; i<obj_num; i++)
            {
                Object3D *obj = objs.getObject(i);
                Hit hit(INFINITY, nullptr, Vec3f(0,0,0));
                if (Obj2Hit.count(obj) >= 1)
                {
                    hit = Obj2Hit[obj];
                }
                else 
                {
                    obj->intersect(r, hit, tmin);
                    Obj2Hit[obj] = hit;
                }
                
                if (hit.getT() >= INFINITY)
                {
                    continue;
                }

                Vec3f p_hit = r.pointAtParameter(hit.getT());
                //std::cout << "Hit Point: " << p_hit << std::endl;
                //std::cout << "Border: " << c << " " << Vec3f(u,v,w) << endl;
                if (p_hit.x() >= c.x()-u && p_hit.x() <= c.x()+u &&
                    p_hit.y() >= c.y()-v && p_hit.y() <= c.y()+v &&
                    p_hit.z() >= c.z()-w && p_hit.z() <= c.z()+w)
                {
                    if (hit.getT() < h.getT())
                    {
                        is_hit = true;
                        h = hit;
                    }
                }
            }
            if (is_hit)
            {  
                //std::cout << h.getNormal() << std::endl;
                break;
            }
        }
        //std::cout << mi.GI << " " << mi.GJ << " " << mi.GK << std::endl;
        mi.NextCell();
    }
    
    if (!Visualize)
    {
        for (int i=0; i<NonVoxelObjects.getNumObjects(); i++)
        {
            Object3D *obj = NonVoxelObjects.getObject(i);
            Hit hit(INFINITY, nullptr, Vec3f(0,0,0));
            bool current_hit = obj->intersect(r, hit, tmin);
            if (current_hit)
            {
                if (hit.getT() < h.getT())
                {
                    is_hit = true;
                    h = hit;
                }
            }
        }
    }
    
    return is_hit;
}