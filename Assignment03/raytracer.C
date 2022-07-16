#include "raytracer.h"
#include "string.h"
#include <iostream>

void Raytracer::Set(SceneParser *_parser,
    char *_input_file, int _width, int _height, char *_output_file,
    bool _save_depth, float _depth_min, float _depth_max, char *_depth_file,
    bool _save_normal, char *_normal_file, bool _shade_back)
{
    parser = _parser;
    width = _width;
    height = _height;
    save_depth = _save_depth;
    depth_min = _depth_min;
    depth_max = _depth_max;
    save_normal = _save_normal;
    shade_back = _shade_back;
    if (_input_file) strcpy(input_file, _input_file);
    if (_output_file) strcpy(output_file, _output_file);
    if (_depth_file) strcpy(depth_file, _depth_file);
    if (_normal_file) strcpy(normal_file, _normal_file);
}

void Raytracer::Render()
{
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
                    Material *mat = hit.getMaterial();
                    Vec3f normal_corrected = is_wrong_side ? 
                        Vec3f(0,0,0) - normal : normal;
                    hit.set(hit.getT(), mat, normal_corrected, ray);
                    Vec3f col_object = hit.getMaterial()->getDiffuseColor();
                    Vec3f col_ambient = col_object * light_ambient;
                    Vec3f col = col_ambient;
                    for (int i=0; i<light_num; i++)
                    {
                        Light* light = parser->getLight(i);
                        Vec3f dir, light_col;
                        float dist;
                        light->getIllumination(point3d, dir, light_col, dist);
                        col += mat->Shade(ray, hit, dir, light_col);
                        // float dot = dir.Dot3(normal_corrected);
                        // dot = dot > 0 ? dot : 0;
                        // col_diffuse += dot * light_col * col_object;
                    }
                    img.SetPixel(x, y, col);
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
    
    if (save_depth)
    {
        printf("Saving Depth...\n");
        img_depth.SaveTGA(depth_file);
    }
    if (save_normal)
    {
        printf("Saving Normal...\n");
        img_normal.SaveTGA(normal_file);
    }
}