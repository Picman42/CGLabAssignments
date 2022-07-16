#pragma comment(lib, "D:/Codes/CPP/CGLabAssignments/freeglut/lib/freeglutd.lib")

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

Raytracer _MAIN_raytracer;

int tessel_theta_steps = 0;
int tessel_phi_steps = 0;
bool gouraud = false;

void ParseArguments(
    int argc, char *argv[],
    char *&input_file, int &width, int &height, char *&output_file,
    bool &save_depth, float &depth_min, float &depth_max, char *&depth_file,
    bool &save_normal, char *&normal_file, bool &shade_back, bool &gui, 
    int &tessel_theta_steps, int &tessel_phi_steps, bool &gouraud)
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
            save_depth = true;
            i++; assert (i < argc); 
            depth_min = atof(argv[i]);
            i++; assert (i < argc); 
            depth_max = atof(argv[i]);
            i++; assert (i < argc); 
            depth_file = argv[i];
        } else if (!strcmp(argv[i],"-normals")) {
            save_normal = true;
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
        } else {
            printf ("whoops error with command line argument %d: '%s'\n",i,argv[i]);
            assert(0);
        }
    }
}

void render()
{
    //_MAIN_raytracer.DebugPrint();
    _MAIN_raytracer.Render();
}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    int width = 100;
    int height = 100;
    char *output_file = NULL;
    bool save_depth = false;
    float depth_min = 0;
    float depth_max = 1;
    char *depth_file = NULL;
    bool save_normal = false;
    char *normal_file = NULL;
    bool shade_back = false;
    bool gui = false;

    ParseArguments(argc, argv, 
        input_file, width, height, output_file,
        save_depth, depth_min, depth_max, depth_file,
        save_normal, normal_file, shade_back, gui, 
        tessel_theta_steps, tessel_phi_steps, gouraud);

    SceneParser parser(input_file);
    
    _MAIN_raytracer.Set(&parser, 
        input_file, width, height, output_file,
        save_depth, depth_min, depth_max, depth_file,
        save_normal, normal_file, shade_back);

    glutInit(&argc, argv);

    if (gui)
    {
        GLCanvas canvas;
        canvas.initialize(&parser, render);
    }
    else
    {
        render();
    }
    
}