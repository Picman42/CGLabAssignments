#include <GL/gl.h>
#include <GL/glut.h>

#include "material.h"
#include <cmath>

Vec3f PhongMaterial::Shade(
    const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const
{
    Vec3f n = hit.getNormal();

    //Diffuse
    Vec3f col_diff = DiffuseColor * lightColor * max(n.Dot3(dirToLight), 0.0f);
    
    //Specular
    Vec3f v = Vec3f(0,0,0) - ray.getDirection();
    v.Normalize();
    Vec3f halfway = dirToLight + v;
    halfway.Normalize();
    Vec3f col_spec = SpecularColor * 
        pow(max(halfway.Dot3(n), 0.0f), Exponent) * lightColor;
    
    Vec3f col = col_diff + col_spec;
    return col;
}

void PhongMaterial::glSetMaterial(void) const {

    GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat specular[4] = {
        getSpecularColor().r(),
        getSpecularColor().g(),
        getSpecularColor().b(),
        1.0};
    GLfloat diffuse[4] = {
        getDiffuseColor().r(),
        getDiffuseColor().g(),
        getDiffuseColor().b(),
        1.0};
    
    // NOTE: GL uses the Blinn Torrance version of Phong...      
    float glexponent = Exponent;
    if (glexponent < 0) glexponent = 0;
    if (glexponent > 128) glexponent = 128;
  
#if !SPECULAR_FIX 
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
  
#else

  // OPTIONAL: 3 pass rendering to fix the specular highlight 
  // artifact for small specular exponents (wide specular lobe)

    if (SPECULAR_FIX_WHICH_PASS == 0) {
        // First pass, draw only the specular highlights
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);
      
    } else if (SPECULAR_FIX_WHICH_PASS == 1) {
        // Second pass, compute normal dot light 
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    } else {
        // Third pass, add ambient & diffuse terms
        assert (SPECULAR_FIX_WHICH_PASS == 2);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
    }

#endif
}