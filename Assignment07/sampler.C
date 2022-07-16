#include "sampler.h"
#include "stdlib.h"
#include <cmath>

Vec2f RandomSampler::getSamplePosition(int n)
{
    float x = (float) rand() / RAND_MAX;
    float y = (float) rand() / RAND_MAX;
    return Vec2f(x, y);
}

Vec2f UniformSampler::getSamplePosition(int n)
{
    // n starts from 0
    float lenf = sqrt(NumSamples);
    int leni = int(lenf + .00001f);
    float x = ((float)(n % leni) + 0.5f) / lenf;
    float y = ((float)(n / leni) + 0.5f) / lenf;
    return Vec2f(x, y);
}

Vec2f JitteredSampler::getSamplePosition(int n)
{
    // n starts from 0
    float lenf = sqrt(NumSamples);
    int leni = int(lenf + .00001f);
    float xmin = (float)(n % leni) / lenf;
    float xmax = (float)(n % leni + 1) / lenf;
    float ymin = (float)(n / leni) / lenf;
    float ymax = (float)(n / leni + 1) / lenf;
    float x = (float) rand() / RAND_MAX * (xmax - xmin) + xmin;
    float y = (float) rand() / RAND_MAX * (ymax - ymin) + ymin;
    return Vec2f(x, y);
}