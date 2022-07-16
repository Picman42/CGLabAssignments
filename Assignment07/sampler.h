#ifndef _SAMPLER_H_
#define _SAMPLER_H

#include "vectors.h"

class Sampler
{
public:
    Sampler(int n): NumSamples(n) {}

    virtual Vec2f getSamplePosition(int n) = 0;

    int NumSamples;
};

class RandomSampler: public Sampler
{
public:
    RandomSampler(int n): Sampler(n) {}

    virtual Vec2f getSamplePosition(int n);

};

class UniformSampler: public Sampler
{
public:
    UniformSampler(int n): Sampler(n) {}

    virtual Vec2f getSamplePosition(int n);

};

class JitteredSampler: public Sampler
{
public:
    JitteredSampler(int n): Sampler(n) {}

    virtual Vec2f getSamplePosition(int n);

};


#endif