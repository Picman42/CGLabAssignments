#ifndef _GRID_H_
#define _GRID_H_

#include "object3d.h"
#include "object3dvector.h"
#include "vectors.h"
#include "marchinginfo.h"
#include "material.h"

class Grid: public Object3D
{
public:
    Grid() {}
    Grid(BoundingBox *bb, int _nx, int _ny, int _nz);
    ~Grid() { delete[] Voxels; }

    virtual bool intersect(const Ray &r, Hit &h, float tmin);
    virtual void paint();
    bool GetOccupied(int x, int y, int z) const;
    int GetObjectNumber(int x, int y, int z) const;
    void Set(int x, int y, int z, bool occupied = true);
    void AddObject(int x, int y, int z, Object3D *obj);
    Vec3f GetBlockSize() const;
    Vec3f GetBlockCenter(int x, int y, int z) const;
    PhongMaterial *GetMaterial(int obj_num) const;

    Vec3f GetTnextFromInternalPoint(const Ray &r, Vec3f sign, Vec3f p) const;
    void GetIndexFromInternalPoint(Vec3f sign, Vec3f p, 
        int &i, int &j, int &k) const;
    void initializeRayMarch(MarchingInfo &mi, const Ray &r, float tmin) const;

    int NumX, NumY, NumZ;
    int *Voxels;
    Object3DVector *VoxelObjects;
    PhongMaterial *Pallete;
    int PalleteSize;
};

#endif