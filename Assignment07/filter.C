#include "filter.h"
#include <cmath>

Vec3f Filter::getColor(int i, int j, Film *film)
{
    Vec3f col_tot(0,0,0);
    float w_tot = 0;

    int range = getSupportRadius();
    int bl = std::max(i-range, 0);
    int br = std::min(i+range, film->getWidth() - 1);
    int bd = std::max(j-range, 0);
    int bu = std::min(j+range, film->getHeight() - 1);
    int num_samples = film->getNumSamples();
    for (int x = bl; x <= br; x++)
    {
        for (int y = bd; y <= bu; y++)
        {
            for (int k = 0; k < num_samples; k++)
            {
                Sample sample = film->getSample(x, y, k);
                Vec2f offset = sample.getPosition();
                float weight = getWeight(x + offset.x() - i - 0.5f, 
                    y + offset.y() - j - 0.5f);
                w_tot += weight;
                col_tot += weight * sample.getColor();
                //if (i==3 && j==6)
                //    std::cout << Vec3f(x,y,k) << " " << w_tot << " " << col_tot << std::endl;
            }
        }
    }
    Vec3f col = w_tot > 0 ? col_tot * (1.0f / w_tot) : Vec3f(0, 0, 0);
    return col;
}

float BoxFilter::getWeight(float x, float y)
{
    return float(max(fabs(x), fabs(y)) <= Radius);
}

int BoxFilter::getSupportRadius()
{
    return int(Radius + 0.5f - 0.00001f);
}

float TentFilter::getWeight(float x, float y)
{
    float dist = sqrt(x * x + y * y);
    return dist <= Radius ? 1.0f - dist / Radius : 0;
}

int TentFilter::getSupportRadius()
{
    return int(Radius + 0.5f - 0.00001f);
}

float GaussianFilter::getWeight(float x, float y)
{
    float dist = sqrt(x * x + y * y);
    return exp(-dist * dist * 0.5f / Sigma / Sigma);
}

int GaussianFilter::getSupportRadius()
{
    return int(Sigma * 2.0f + 0.5f - 0.00001f);
}