#include "ifs.h"
#include <stdio.h>
#include <stdlib.h>

IFS::IFS(int _n): N(_n)
{
    Matrices = new Matrix[_n];
    Probs = new float[_n];
}

IFS::~IFS()
{
    delete[] Matrices;
    delete[] Probs;
}

void IFS::SetTransformation(int k, float prob, Matrix m)
{
    Probs[k] = prob;
    Matrices[k] = m;
    //printf("%f %f %f\n", Matrices[k].Get(0,0), Matrices[k].Get(1,0), Matrices[k].Get(2,0));
    //printf("%f %f %f\n", Matrices[k].Get(0,1), Matrices[k].Get(0,2)), Matrices[k].Get(0,3);
}

void IFS::Render(Image& img, int pnum, int iters) const
{
    for (int i=0; i<pnum; i++)
    {
        float x0, y0;
        x0 = rand() / (RAND_MAX + 1.0f);
        y0 = rand() / (RAND_MAX + 1.0f);
        
        Vec2f p(x0, y0);
        for (int k=0; k<iters; k++)
        {
            float barrier = rand() / (RAND_MAX + 1.0f);
            float cur_total_prob = 0;
            int j;
            for (j=0; j<N; j++)
            {
                cur_total_prob += Probs[j];
                if (cur_total_prob >= barrier)
                {
                    break;
                }
            }
            Matrices[j].Transform(p);
            //printf("%d %f %f %f %f\n", j, x0, y0, p.x(), p.y());
        }
        int x = (int)(p.x() * img.Width()), y = (int)(p.y() * img.Height());
        //printf("%d, %d\n", x, y);
        if (x >= 0 && x < img.Width() && y >= 0 && y < img.Height())
        {
            img.SetPixel(x, y, Vec3f(0, 0, 0));
        }
    }
}