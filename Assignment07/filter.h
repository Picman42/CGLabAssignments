#ifndef _FILTER_H_
#define _FILTER_H_

#include "vectors.h"
#include "film.h"

class Filter
{
public:
    Filter() {}
    virtual Vec3f getColor(int i, int j, Film *film);
    virtual float getWeight(float x, float y) = 0;
    virtual int getSupportRadius() = 0;

};

class BoxFilter: public Filter
{
public:
    BoxFilter(float r): Radius(r) {}
    virtual float getWeight(float x, float y);
    virtual int getSupportRadius();

    float Radius;
};

class TentFilter: public Filter
{
public:
    TentFilter(float r): Radius(r) {}
    virtual float getWeight(float x, float y);
    virtual int getSupportRadius();

    float Radius;
};

class GaussianFilter: public Filter
{
public:
    GaussianFilter(float sigma): Sigma(sigma) {}
    virtual float getWeight(float x, float y);
    virtual int getSupportRadius();

    float Sigma;
};


#endif