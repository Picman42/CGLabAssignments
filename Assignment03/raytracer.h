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

class Raytracer
{
public:
    SceneParser *parser;
    char input_file[256];
    int width;
    int height;
    char output_file[256];
    bool save_depth;
    float depth_min;
    float depth_max;
    char depth_file[256];
    bool save_normal;
    char normal_file[256];
    bool shade_back;
    
    Raytracer() {}
    void Set(SceneParser *_parser,
        char *_input_file, int _width, int _height, char *_output_file,
        bool _save_depth, float _depth_min, float _depth_max, char *_depth_file,
        bool _save_normal, char *_normal_file, bool _shade_back);
    
    void Render();

    void DebugPrint() { printf("input=%s, width=%d, shade_back=%d\n", input_file, width, height); }
};

#endif