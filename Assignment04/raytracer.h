#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

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
/*
extern char *input_file;
extern int width;
extern int height;
extern char *output_file;
extern bool save_depth;
extern float depth_min;
extern float depth_max;
extern char *depth_file;
extern bool save_normal;
extern char *normal_file;
extern bool shade_back;
extern bool gui;
extern int tessel_theta_steps;
extern int tessel_phi_steps;
extern bool gouraud;
*/
class Raytracer
{
public:
    SceneParser *parser;
    int MaxBounces;
    float CutoffWeight;
    bool CastShadow;
    bool ShadeBack;

    Camera *camera;
    Group *group;
    Vec3f light_ambient;
    int light_num;
    
    Raytracer() {}
    Raytracer(SceneParser *s, 
        int max_bounces, float cutoff_weight, bool shadows,
        bool shade_back);

    Vec3f traceRay(Ray &ray, float tmin, int bounces, float weight, 
        float indexOfRefraction, Hit &hit) const;
};

#endif