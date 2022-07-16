#include "image.h"
#include "matrix.h"
#include "ifs.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // sample command line:
    // .\ifs -input fern.txt -points 10000 -iters 10 -size 100 -output fern.tga

    srand((unsigned)time(NULL));

    char *input_file = NULL;
    int num_points = 10000;
    int num_iters = 10;
    int size = 100;
    char *output_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc); 
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-points")) {
            i++; assert (i < argc); 
            num_points = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-iters")) {
            i++; assert (i < argc); 
            num_iters = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-size")) {
            i++; assert (i < argc); 
            size = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc); 
            output_file = argv[i];
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    Image img(size, size);
    //img.SetAllPixels(Vec3f(1.0f, 0.0f, 0.0f));
    //img.SaveTGA(output_file);

    // ========================================================
    // ========================================================
    // Some sample code you might like to use for
    // parsing the IFS transformation files

    // open the file
    FILE *input = fopen(input_file,"r");
    assert(input != NULL);

    // read the number of transforms
    int num_transforms; 
    fscanf(input,"%d",&num_transforms);

    // < DO SOMETHING WITH num_transforms >
    IFS ifs(num_transforms);

    // read in the transforms
    for (int i = 0; i < num_transforms; i++) {
        float probability; 
        fscanf (input,"%f",&probability);
        Matrix m; 
        m.Read3x3(input);
        // < DO SOMETHING WITH probability and m >
        //printf("%f %f %f\n", m.Get(0,0), m.Get(1,0), m.Get(2,0));
        //printf("%f %f %f\n", m.Get(0,1), m.Get(0,2)), m.Get(0,3);
        ifs.SetTransformation(i, probability, m);
    }

    // close the file
    fclose(input);

    img.SetAllPixels(Vec3f(1.0f, 1.0f, 1.0f));
    ifs.Render(img, num_points, num_iters);
    img.SaveTGA(output_file);

    return 0;
}