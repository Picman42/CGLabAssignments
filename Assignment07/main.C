#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "time.h"
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
#include "sample.h"
#include "film.h"
#include "sampler.h"
#include "filter.h"

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
char *sample_file = NULL;
int sample_zoom = 0;
bool random_samples = false;
bool uniform_samples = false;
bool jittered_samples = false;
int num_samples = 1;
char *filter_file = NULL;
int filter_zoom = 0;
bool box_filter = false;
float box_filter_radius = 0.5f;
bool tent_filter = false;
float tent_filter_radius = 0.5f;
bool gaussian_filter = false;
float gaussian_filter_sigma = 0;

SceneParser *parser = NULL;

Film *film;

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
        } else if (!strcmp(argv[i],"-render_samples")) {
            i++; assert (i < argc); 
            sample_file = argv[i];
            i++; assert (i < argc); 
            sample_zoom = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-random_samples")) {
            random_samples = true;
            i++; assert (i < argc); 
            num_samples = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-uniform_samples")) {
            uniform_samples = true;
            i++; assert (i < argc); 
            num_samples = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-jittered_samples")) {
            jittered_samples = true;
            i++; assert (i < argc); 
            num_samples = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-render_filter")) {
            i++; assert (i < argc); 
            filter_file = argv[i];
            i++; assert (i < argc); 
            filter_zoom = atoi(argv[i]);
        } else if (!strcmp(argv[i],"-box_filter")) {
            box_filter = true;
            i++; assert (i < argc); 
            box_filter_radius = atof(argv[i]);
        } else if (!strcmp(argv[i],"-tent_filter")) {
            tent_filter = true;
            i++; assert (i < argc); 
            tent_filter_radius = atof(argv[i]);
        } else if (!strcmp(argv[i],"-gaussian_filter")) {
            gaussian_filter = true;
            i++; assert (i < argc); 
            gaussian_filter_sigma = atof(argv[i]);
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

    Sampler *sampler;
    if (random_samples)
    {
        sampler = new RandomSampler(num_samples);
    }
    else if (jittered_samples)
    {
        sampler = new JitteredSampler(num_samples);
    }
    else
    {
        sampler = new UniformSampler(num_samples);
    }

    Filter *filter;
    if (tent_filter)
    {
        filter = new TentFilter(tent_filter_radius);
    }
    else if (gaussian_filter)
    {
        filter = new GaussianFilter(gaussian_filter_sigma);
    }
    else
    {
        filter = new BoxFilter(box_filter_radius);
    }

    Camera *camera = parser->getCamera();
    Group *group = parser->getGroup();
    Vec3f light_ambient = parser->getAmbientLight();
    int light_num = parser->getNumLights();
    
    Image img(width, height), img_depth(width, height), img_normal(width, height);
    
    if (output_file)
    {
        img.SetAllPixels(parser->getBackgroundColor());
    }
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
            for (int i=0; i<num_samples; i++)
            {
                std::cout << "\rRendering (x, y, sample): " << Vec3f(x,y,i) << "     ";
                Vec2f sample_offset = sampler->getSamplePosition(i);
                
                float sx = 0.5f + ((float)x - width * 0.5f) / longside;     // FIXED
                float sy = 0.5f + ((float)y - height * 0.5f) / longside;
                sx += sample_offset.x() * (1.0f / width);
                sy += sample_offset.y() * (1.0f / height);
                //std::cout << sx << " " << sy << std::endl;
                Ray ray = camera->generateRay(Vec2f(sx, sy));
                Hit hit(INFINITY, nullptr, Vec3f(0,0,0));

                Vec3f col = raytracer.traceRay(ray, camera->getTMin(), 
                    0.0f, 1.0f, 1.0f, hit);
                /*
                if (output_file)
                {
                    img.SetPixel(x, y, col);
                }
                */
                if (hit.getT() >= INFINITY)
                {
                    col = parser->getBackgroundColor();
                }

                film->setSample(x, y, i, sample_offset, col);
            }
        }
    }
    std::cout << "\rSampling...                                              " << std::endl; 

    if (output_file)
    {
        
        for (int x=0; x<width; x++)
        {
            for (int y=0; y<height; y++)
            {
                //std::cout << x << " " << y << std::endl;
                Vec3f col = filter->getColor(x, y, film);
                img.SetPixel(x, y, col);
            }
        }
        img.SaveTGA(output_file);
    }
    if (depth_file)
    {
        img_depth.SaveTGA(depth_file);
    }
    if (normal_file)
    {
        img_normal.SaveTGA(normal_file);
    }
    if (sample_file)
    {
        film->renderSamples(sample_file, sample_zoom);
    }
    if (filter_file)
    {
        film->renderFilter(filter_file, filter_zoom, filter);
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

    srand(time(NULL));

    parser = new SceneParser(input_file);

    film = new Film(width, height, num_samples);
    
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