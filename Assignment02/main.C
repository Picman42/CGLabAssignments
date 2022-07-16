#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "scene_parser.h"
#include "camera.h"
#include "hit.h"
#include "group.h"
#include "image.h"
#include "material.h"
#include "light.h"

void ParseArguments(
    int argc, char *argv[],
    char *&input_file, int &width, int &height, char *&output_file,
    float &depth_min, float &depth_max, char *&depth_file,
    char *&normal_file, bool &shade_back)
{
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
        } else if (!strcmp(argv[i],"-normals")) {
            i++; assert (i < argc); 
            normal_file = argv[i];
        } else if (!strcmp(argv[i],"-shade_back")) {
            shade_back = true;
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }
}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    int width = 100;
    int height = 100;
    char *output_file = NULL;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = NULL;
    char *normal_file = NULL;
    bool shade_back = false;

    ParseArguments(argc, argv, 
        input_file, width, height, output_file,
        depth_min, depth_max, depth_file,
        normal_file, shade_back);

    SceneParser parser(input_file);
    Camera *camera = parser.getCamera();
    Group *group = parser.getGroup();
    Vec3f light_ambient = parser.getAmbientLight();
    int light_num = parser.getNumLights();

    Image img(width, height), img_depth(width, height), img_normal(width, height);
    img.SetAllPixels(parser.getBackgroundColor());

    if (depth_file)
    {
        img_depth.SetAllPixels(Vec3f(0, 0, 0));
    }
    if (normal_file)
    {
        img_normal.SetAllPixels(Vec3f(0, 0, 0));
    }

    for (int x=0; x<width; x++)
    {
        for (int y=0; y<height; y++)
        {
            //printf("(%d, %d)\n", x, y);
            Ray ray = camera->generateRay(Vec2f((float)x / width, (float)y / height));
            Hit hit(99999999.0f, nullptr, Vec3f(0,0,0));
            bool is_hit = group->intersect(ray, hit, camera->getTMin());
            //if (x==0 && y==0) std::cout << ray.getDirection() << std::endl;
            if (is_hit)
            {
                //Diffuse Render
                float t = hit.getT();
                Vec3f point3d = ray.pointAtParameter(t);
                Vec3f normal = hit.getNormal();
                bool is_wrong_side = ray.getDirection().Dot3(normal) > 0;
                bool can_shade = !is_wrong_side || shade_back;
                if (can_shade)
                {
                    Vec3f normal_corrected = is_wrong_side ? 
                        Vec3f(0,0,0) - normal : normal;
                    Vec3f col_object = hit.getMaterial()->getDiffuseColor();
                    Vec3f col_ambient = col_object * light_ambient;
                    Vec3f col_diffuse = col_ambient;
                    for (int i=0; i<light_num; i++)
                    {
                        Light* light = parser.getLight(i);
                        Vec3f dir, light_col;
                        light->getIllumination(point3d, dir, light_col);
                        float dot = dir.Dot3(normal_corrected);
                        dot = dot > 0 ? dot : 0;
                        col_diffuse += dot * light_col * col_object;
                    }
                    img.SetPixel(x, y, col_diffuse);
                }
                else
                {
                    img.SetPixel(x, y, Vec3f(0, 0, 0));
                }
                //Depth Render
                if (depth_file)
                {
                    float gray = t < depth_min ? 1.0f :
                                t > depth_max ? 0.0f :
                                1.0f - (t - depth_min) / (depth_max - depth_min);
                    img_depth.SetPixel(x, y, Vec3f(gray, gray, gray));
                }
                //Normal Render
                if (normal_file)
                {
                    Vec3f c = Vec3f(abs(normal.x()), abs(normal.y()), abs(normal.z()));
                    img_normal.SetPixel(x, y, c);
                }
            }
        }
    }
    
    img.SaveTGA(output_file);
    if (depth_file)
    {
        img_depth.SaveTGA(depth_file);
    }
    if (normal_file)
    {
        img_normal.SaveTGA(normal_file);
    }
    
}