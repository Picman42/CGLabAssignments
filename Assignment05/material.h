#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "ray.h"
#include "hit.h"
#include <iostream>

#include "glCanvas.h"  

#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif

class Material {

public:

    // CONSTRUCTORS & DESTRUCTOR
    Material() {}
    Material(const Vec3f &dif_col, const Vec3f &refl_col,
        const Vec3f &transp_col, float refrac):
        DiffuseColor(dif_col), ReflectionColor(refl_col),
        TransparentColor(transp_col), RefractionIndex(refrac) {}

    virtual ~Material() {}

    // ACCESSORS
    virtual Vec3f getDiffuseColor() const { return DiffuseColor; }
    virtual Vec3f getReflectionColor() const { return ReflectionColor; }
    virtual Vec3f getTransparentColor() const { return TransparentColor; }
    virtual float getRefractionIndex() const { return RefractionIndex; }

    virtual Vec3f Shade(
        const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
        const Vec3f &lightColor) const = 0;

    virtual void glSetMaterial(void) const = 0;

protected:

    // REPRESENTATION
    Vec3f DiffuseColor;
    Vec3f ReflectionColor;
    Vec3f TransparentColor;
    float RefractionIndex;
  
};

class PhongMaterial: public Material
{
public:
    PhongMaterial() {}
    PhongMaterial(const Vec3f &dif_col):
        Material(dif_col, Vec3f(0,0,0), Vec3f(0,0,0), 0), 
        SpecularColor(Vec3f(0,0,0)), Exponent(0) {}
    PhongMaterial(const Vec3f &dif_col, const Vec3f &spec_col, float exp,
        const Vec3f &refl_col, const Vec3f &transp_col, float refrac):
        Material(dif_col, refl_col, transp_col, refrac), 
        SpecularColor(spec_col), Exponent(exp) { }

    Vec3f getSpecularColor() const { return SpecularColor; }

    virtual Vec3f Shade(
        const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
        const Vec3f &lightColor) const;

    virtual void glSetMaterial(void) const;

    Vec3f SpecularColor;    //ks
    float Exponent;         //q

};

#endif
