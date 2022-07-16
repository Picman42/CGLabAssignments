#include "matrix.h"
#include "image.h"

class IFS
{
public:
    IFS() {}
    IFS(int _n);
    ~IFS();
    void SetTransformation(int k, float prob, Matrix m);
    void Render(Image& img, int pnum, int iters) const;

    int N;
    Matrix* Matrices;
    float* Probs;       //Assuming total probablity is 1.
};