#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "scene_parser.h"
#include "camera.h"
#include "hit.h"
#include "group.h"
#include "image.h"

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    int width = 100;
    int height = 100;
    char *output_file = NULL;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = NULL;

    // sample command line:
    // raytracer -input scene1_01.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i],"-input")) {
            i++; assert (i < argc); 
            input_file = argv[i];
        } else if (!strcmp(argv[i],"-size")) {
            i++; assert (i < argc); 
            width = atoi(argv[i]);
            i++; assert (i < argc); 
            height = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-output")) {
            i++; assert (i < argc); 
            output_file = argv[i];
        } else if (!strcmp(argv[i],"-depth")) {
            i++; assert (i < argc); 
            depth_min = atof(argv[i]);
            i++; assert (i < argc); 
            depth_max = atof(argv[i]);
            i++; assert (i < argc); 
            depth_file = argv[i];
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }

    SceneParser parser(input_file);
    OrthographicCamera *camera = dynamic_cast<OrthographicCamera *>(parser.getCamera());
    Group *group = parser.getGroup();

    Image img_albedo(width, height), img_depth(width, height);
    img_albedo.SetAllPixels(parser.getBackgroundColor());
    img_depth.SetAllPixels(Vec3f(0, 0, 0));
    
    for (int x=0; x<width; x++)
    {
        for (int y=0; y<height; y++)
        {
            Ray ray = camera->generateRay(Vec2f((float)x / width, (float)y / height));
            Hit hit(99999999.0f, nullptr);
            bool is_hit = group->intersect(ray, hit, camera->getTMin());
            if (is_hit)
            {
                img_albedo.SetPixel(x, y, hit.getMaterial()->getDiffuseColor());
                float t = hit.getT();
                float gray = t < depth_min ? 1.0f :
                             t > depth_max ? 0.0f :
                             1.0f - (t - depth_min) / (depth_max - depth_min);
                img_depth.SetPixel(x, y, Vec3f(gray, gray, gray));
            }
        }
    }
    
    img_albedo.SaveTGA(output_file);
    img_depth.SaveTGA(depth_file);

    return 0;
}