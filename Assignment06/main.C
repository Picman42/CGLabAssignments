#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <GL/gl.h>
#include <GL/glut.h>

#include "scene_parser.h"
#include "raytracer.h"
#include "camera.h"
#include "hit.h"
#include "group.h"
#include "image.h"
#include "material.h"
#include "light.h"
#include "glCanvas.h"
#include "raytracing_stats.h"

char *input_file = NULL;
int width = 100;
int height = 100;
char *output_file = NULL;
float depth_min = 0.0f;
float depth_max = 1.0f;
char *depth_file = NULL;
char *normal_file = NULL;
bool shade_back = false;
bool gui = false;
int tessel_theta_steps = 0;
int tessel_phi_steps = 0;
bool gouraud = false;
bool cast_shadow = false;
int rt_bounces = 0;
float rt_weight = 0;
bool use_grid = false;
int grid_nx = 0;
int grid_ny = 0;
int grid_nz = 0;
bool visualize_grid = false;
bool print_stats = false;

SceneParser *parser = NULL;

Raytracer raytracer;

void ParseArguments(int argc, char *argv[])
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
        } else if (!strcmp(argv[i],"-gui")) {
            gui = true;
        } else if (!strcmp(argv[i],"-tessellation")) {
            i++; assert (i < argc); 
            tessel_theta_steps = atoi(argv[i]);
            i++; assert (i < argc); 
            tessel_phi_steps = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-gouraud")) {
            gouraud = true;
        } else if (!strcmp(argv[i],"-shadows")) {
            cast_shadow = true;
        } else if (!strcmp(argv[i],"-bounces")) {
            i++; assert (i < argc); 
            rt_bounces = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-weight")) {
            i++; assert (i < argc); 
            rt_weight = atof(argv[i]);
        } else if (!strcmp(argv[i],"-grid")) {
            use_grid = true;
            i++; assert (i < argc); 
            grid_nx = atoi(argv[i]);
            i++; assert (i < argc); 
            grid_ny = atoi(argv[i]);
            i++; assert (i < argc); 
            grid_nz = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-visualize_grid")) {
            visualize_grid = true;
        } else if (!strcmp(argv[i],"-stats")) {
            print_stats = true;
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }
}

void render()
{
    //_MAIN_raytracer.DebugPrint();
    //raytracer.Render();

    Camera *camera = parser->getCamera();
    Group *group = parser->getGroup();
    Vec3f light_ambient = parser->getAmbientLight();
    int light_num = parser->getNumLights();
    
    Image img(width, height), img_depth(width, height), img_normal(width, height);
    img.SetAllPixels(parser->getBackgroundColor());

    if (depth_file)
    {
        img_depth.SetAllPixels(Vec3f(0, 0, 0));
    }
    if (normal_file)
    {
        img_normal.SetAllPixels(Vec3f(0, 0, 0));
    }

    float longside = max(width, height);

    for (int x=0; x<width; x++)
    {
        for (int y=0; y<height; y++)
        {
            float sx = 0.5f + ((float)x - width * 0.5f) / longside;     // FIXED
            float sy = 0.5f + ((float)y - height * 0.5f) / longside;
            Ray ray = camera->generateRay(Vec2f(sx, sy));
            Hit hit(INFINITY, nullptr, Vec3f(0,0,0));

            Vec3f col = raytracer.traceRay(ray, camera->getTMin(), 
                0.0f, 1.0f, 1.0f, hit);

            img.SetPixel(x, y, col);
        }
    }
    img.SaveTGA(output_file);
    
    if (depth_file)
    {
        printf("Saving Depth...\n");
        img_depth.SaveTGA(depth_file);
    }
    if (normal_file)
    {
        printf("Saving Normal...\n");
        img_normal.SaveTGA(normal_file);
    }

    if (print_stats)
    {
        RayTracingStats::PrintStatistics();
    }
}

void traceRay(float x, float y)
{
    printf("Visualizing ray at (%f, %f)...\n", x, y);
    Camera *camera = parser->getCamera();
    Ray ray = camera->generateRay(Vec2f(x, y));
    Hit hit(99999999.0f, nullptr, Vec3f(0,0,0));

    Vec3f col = raytracer.traceRay(ray, camera->getTMin(), 
        0.0f, 1.0f, 1.0f, hit);
}

int main(int argc, char *argv[])
{

    ParseArguments(argc, argv);

    parser = new SceneParser(input_file);
    
    raytracer = Raytracer(parser, 
        rt_bounces, rt_weight, cast_shadow, shade_back,
        use_grid, visualize_grid, grid_nx, grid_ny, grid_nz);

    RayTracingStats::Initialize(width, height, parser->getGroup()->getBoundingBox(),
        grid_nx, grid_ny, grid_nz);

    glutInit(&argc, argv);

    if (gui)
    {
        GLCanvas canvas;
        canvas.initialize(parser, render, traceRay, 
            raytracer.GetSceneGrid(), visualize_grid);
    }
    else
    {
        render();
    }
    
}