#include "transform.h"
#include <cmath>

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
    //if (r.getOrigin().x() <= 0.002f && r.getOrigin().x() >= -0.002f && r.getOrigin().y() <= 0.002f && r.getOrigin().y() >= -0.002f) std::cout << " " << h << std::endl;
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
        h.set(t_tr, h.getMaterial(), n_tr, ray_tr);
        //if (r.getOrigin().x() <= 0.002f && r.getOrigin().x() >= -0.002f && r.getOrigin().y() <= 0.002f && r.getOrigin().y() >= -0.002f) std::cout << "hit " << h << std::endl;
        return true;
    }
    h = h_original;
    return false;
}