#include <GL/gl.h>
#include <GL/glut.h>

#include "material.h"
#include "perlin_noise.h"
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

void CheckerBoard::glSetMaterial(void) const
{
    Mat1->glSetMaterial();
}

int CheckerBoard::GetMaterialIndex(Vec3f p_hit) const
{
    //Vec3f p_hit = hit.getIntersectionPoint();
    M->Transform(p_hit);
    int gx = (int)floor(p_hit.x());
    int gy = (int)floor(p_hit.y());
    int gz = (int)floor(p_hit.z());
    return (gx + gy + gz) % 2;
}

Vec3f CheckerBoard::Shade(
    const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const
{
    if (GetMaterialIndex(hit.getIntersectionPoint()) == 0)
    {
        return Mat1->Shade(ray, hit, dirToLight, lightColor);
    }
    else
    {
        return Mat2->Shade(ray, hit, dirToLight, lightColor);
    }
}

Vec3f CheckerBoard::getDiffuseColor(Vec3f coord) const
{
    return GetMaterialIndex(coord) == 0 ? 
        Mat1->getDiffuseColor(coord) : Mat2->getDiffuseColor(coord);
}

Vec3f CheckerBoard::getReflectionColor(Vec3f coord) const
{
    return GetMaterialIndex(coord) == 0 ? 
        Mat1->getReflectionColor(coord) : Mat2->getReflectionColor(coord);
}

Vec3f CheckerBoard::getTransparentColor(Vec3f coord) const
{
    return GetMaterialIndex(coord) == 0 ? 
        Mat1->getTransparentColor(coord) : Mat2->getTransparentColor(coord);
}

float CheckerBoard::getRefractionIndex(Vec3f coord) const
{
    return GetMaterialIndex(coord) == 0 ? 
        Mat1->getRefractionIndex(coord) : Mat2->getRefractionIndex(coord);
}

void Noise::glSetMaterial(void) const
{
    Mat1->glSetMaterial();
}

float Noise::GetMaterialBlend(Vec3f coord, int octaves)
{
    float mul = 1.0f;
    float N = 0;
    for (int i=0; i<octaves; i++)
    {
        N += PerlinNoise::noise(coord.x() * mul, coord.y() * mul, coord.z() * mul) / mul;
        mul *= 2.0f;
    }
    if (N < -1.0f)
    {
        N = -1.0f;
    }
    if (N > 1.0f)
    {
        N = 1.0f;
    }
    //N = N * 0.5f + 0.5f;
    return N;
}

Vec3f Noise::Shade(
    const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const
{
    Vec3f coord = hit.getIntersectionPoint();
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f shade1 = Mat1->Shade(ray, hit, dirToLight, lightColor);
    Vec3f shade2 = Mat2->Shade(ray, hit, dirToLight, lightColor);
    return blend * shade1 + (1.0f - blend) * shade2;
}

Vec3f Noise::getDiffuseColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getDiffuseColor(coord);
    Vec3f col2 = Mat2->getDiffuseColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

Vec3f Noise::getReflectionColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getReflectionColor(coord);
    Vec3f col2 = Mat2->getReflectionColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

Vec3f Noise::getTransparentColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getTransparentColor(coord);
    Vec3f col2 = Mat2->getTransparentColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

float Noise::getRefractionIndex(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    float col1 = Mat1->getRefractionIndex(coord);
    float col2 = Mat2->getRefractionIndex(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

void Marble::glSetMaterial(void) const
{
    Mat1->glSetMaterial();
}

float Marble::GetMaterialBlend(Vec3f coord, int octaves) const
{
    float N = Noise::GetMaterialBlend(coord, octaves);
    float M = sin(Frequency * coord.x() + Amplitude * N);
    if (M < -1.0f)
    {
        M = -1.0f;
    }
    if (M > 1.0f)
    {
        M = 1.0f;
    }
    //N = N * 0.5f + 0.5f;
    return M;
}

Vec3f Marble::Shade(
    const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const
{
    Vec3f coord = hit.getIntersectionPoint();
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f shade1 = Mat1->Shade(ray, hit, dirToLight, lightColor);
    Vec3f shade2 = Mat2->Shade(ray, hit, dirToLight, lightColor);
    return blend * shade1 + (1.0f - blend) * shade2;
}

Vec3f Marble::getDiffuseColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getDiffuseColor(coord);
    Vec3f col2 = Mat2->getDiffuseColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

Vec3f Marble::getReflectionColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getReflectionColor(coord);
    Vec3f col2 = Mat2->getReflectionColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

Vec3f Marble::getTransparentColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getTransparentColor(coord);
    Vec3f col2 = Mat2->getTransparentColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

float Marble::getRefractionIndex(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    float col1 = Mat1->getRefractionIndex(coord);
    float col2 = Mat2->getRefractionIndex(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

void Wood::glSetMaterial(void) const
{
    Mat1->glSetMaterial();
}

float Wood::GetMaterialBlend(Vec3f coord, int octaves) const
{
    float r = sqrt(coord.x() * coord.x() + coord.z() * coord.z());
    float theta = atan2(coord.z(), coord.x());
    coord = Vec3f(r, theta, coord.z());
    float N = Noise::GetMaterialBlend(coord, octaves);
    float W = sin(Frequency * coord.x() + Amplitude * N);
    if (W < -1.0f)
    {
        W = -1.0f;
    }
    if (W > 1.0f)
    {
        W = 1.0f;
    }
    //N = N * 0.5f + 0.5f;
    return W;
}

Vec3f Wood::Shade(
    const Ray &ray, const Hit &hit, const Vec3f &dirToLight, 
    const Vec3f &lightColor) const
{
    Vec3f coord = hit.getIntersectionPoint();
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f shade1 = Mat1->Shade(ray, hit, dirToLight, lightColor);
    Vec3f shade2 = Mat2->Shade(ray, hit, dirToLight, lightColor);
    return blend * shade1 + (1.0f - blend) * shade2;
}

Vec3f Wood::getDiffuseColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getDiffuseColor(coord);
    Vec3f col2 = Mat2->getDiffuseColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

Vec3f Wood::getReflectionColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getReflectionColor(coord);
    Vec3f col2 = Mat2->getReflectionColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

Vec3f Wood::getTransparentColor(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    Vec3f col1 = Mat1->getTransparentColor(coord);
    Vec3f col2 = Mat2->getTransparentColor(coord);
    return blend * col1 + (1.0f - blend) * col2;
}

float Wood::getRefractionIndex(Vec3f coord) const
{
    M->Transform(coord);
    float blend = GetMaterialBlend(coord, Octaves);
    blend = blend * 0.5f + 0.5f;
    float col1 = Mat1->getRefractionIndex(coord);
    float col2 = Mat2->getRefractionIndex(coord);
    return blend * col1 + (1.0f - blend) * col2;
}