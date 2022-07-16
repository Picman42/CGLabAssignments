#include "raytracer.h"
#include "string.h"
#include <iostream>
#include "rayTree.h"
#include "boundingbox.h"
#include "raytracing_stats.h"

Raytracer::Raytracer(SceneParser *s, 
    int max_bounces, float cutoff_weight, bool shadows, bool shade_back, 
    bool use_grid, bool visualize_grid, int grid_nx, int grid_ny, int grid_nz):
    parser(s), MaxBounces(max_bounces), 
    CutoffWeight(cutoff_weight), CastShadow(shadows), ShadeBack(shade_back),
    UseGrid(use_grid), VisualizeGrid(visualize_grid)
{
    camera = parser->getCamera();
    group = parser->getGroup();
    light_ambient = parser->getAmbientLight();
    light_num = parser->getNumLights();
    
    
    //std::cout << bbox->getMin() << " " << bbox->getMax() << std::endl;
    if (use_grid)
    {
        //std::cout << "Inserting into grid..." << std::endl;
        BoundingBox *bbox = group->getBoundingBox();
        Vec3f bbox_min = bbox->getMin(), bbox_max = bbox->getMax();
        if (bbox_max.x() < bbox_min.x() + 0.01f)
        {
            bbox_max.Set(bbox_min.x() + 0.01f, bbox_max.y(), bbox_max.z());
        }
        if (bbox_max.y() < bbox_min.y() + 0.01f)
        {
            bbox_max.Set(bbox_max.x(), bbox_min.y() + 0.01f, bbox_max.z());
        }
        if (bbox_max.z() < bbox_min.z() + 0.01f)
        {
            bbox_max.Set(bbox_max.z(), bbox_max.y(), bbox_min.z() + 0.01f);
        }
        bbox = new BoundingBox(bbox_min, bbox_max);
        SceneGrid = new Grid(bbox, grid_nx, grid_ny, grid_nz, visualize_grid);
        group->insertIntoGrid(SceneGrid, nullptr);
    }
    else
    {
        SceneGrid = nullptr;
    }
}

Vec3f Raytracer::traceRay(Ray &ray, float tmin, int bounces, float weight, 
    float indexOfRefraction, Hit &hit) const
{   
    Vec3f color = parser->getBackgroundColor();

    RayTracingStats::IncrementNumNonShadowRays();
    
    bool is_hit;
    is_hit = UseGrid ? SceneGrid->intersect(ray, hit, tmin) :
        group->intersect(ray, hit, tmin);
    //std::cout << "Start Hit: " << hit << " // " << hit.getIntersectionPoint() << std::endl;
    if (is_hit)
    {
        float epsilon = 0.00002f;
        
        float t = hit.getT();
        Vec3f point3d = ray.pointAtParameter(t);
        Vec3f view = ray.getDirection();
        view.Normalize();
        Vec3f normal = hit.getNormal();
        Vec3f p_intersect = hit.getIntersectionPoint();
        bool is_wrong_side = ray.getDirection().Dot3(normal) > 0;
        bool can_shade = !is_wrong_side || ShadeBack;
        if (can_shade)
        {
            //RayTree::SetMainSegment(ray, 0, t);

            Material *mat = hit.getMaterial();
            Vec3f normal_corrected = is_wrong_side ? 
                Vec3f(0,0,0) - normal : normal;
            hit.set(hit.getT(), mat, normal_corrected, ray);
            Vec3f col_object = hit.getMaterial()->getDiffuseColor(p_intersect);
            Vec3f col_ambient = col_object * light_ambient;
            Vec3f col = col_ambient;
            for (int i=0; i<light_num; i++)
            {
                Light* light = parser->getLight(i);
                Vec3f dir, light_col;
                float dist;
                light->getIllumination(point3d, dir, light_col, dist);
                
                // Shadow
                if (CastShadow)
                {
                    RayTracingStats::IncrementNumShadowRays();
                    Ray ray_shadow(p_intersect, dir);
                    Hit hit_shadow(dist, NULL, Vec3f(0, 0, 0));
                    
                    bool is_hit_shadow = UseGrid ? 
                        SceneGrid->intersect(ray_shadow, hit_shadow, epsilon) :
                        group->intersect(ray_shadow, hit_shadow, epsilon);
                    if (hit_shadow.getT() == dist)
                    {
                        col += mat->Shade(ray, hit, dir, light_col);
                    }
                    RayTree::AddShadowSegment(ray_shadow, epsilon, hit_shadow.getT());
                    //std::cout << ray_shadow << std::endl;
                }
                else
                {
                    col += mat->Shade(ray, hit, dir, light_col);
                }
            }

            // Reflection
            Vec3f col_refl = mat->getReflectionColor(p_intersect);
            //std::cout << bounces << " " << weight << " " << col_refl.Length() << endl;
            if (bounces < MaxBounces && weight > CutoffWeight && 
                col_refl.Length() > 0.0002f)
            {
                Vec3f reflect = view - 2.0f * view.Dot3(normal) * normal;
                reflect.Normalize();
                Ray ray_refl(p_intersect, reflect);
                Hit hit_refl(99999999.0f, nullptr, Vec3f(0,0,0));
                col += col_refl * traceRay(ray_refl, epsilon, bounces+1, 
                    weight * col_refl.Length(), indexOfRefraction, hit_refl);
                
                //std::cout << ray_refl << " " << hit_refl << " " << col_refl.Length() << endl;
                //std::cout << "Reflection: " << ray_refl << " " << hit_refl.getIntersectionPoint() << std::endl;
                RayTree::AddReflectedSegment(ray_refl, epsilon, hit_refl.getT());
            }

            // Refraction
            Vec3f col_refrac = mat->getTransparentColor(p_intersect);
            float refrac = mat->getRefractionIndex(p_intersect);
            if (bounces < MaxBounces && col_refrac.Length() > 0.0002f)
            {
                Vec3f I = -1.0f * view;
                float IdotN = I.Dot3(normal);
                bool into = IdotN > 0;
                IdotN = into ? IdotN : -IdotN;
                Vec3f N = into ? normal : -1.0f * normal;
                float ni = indexOfRefraction;
                float nt = into ? refrac : 1.0f;
                float nr = ni / nt;
                float in_sqrt = 1.0f - nr * nr * (1.0f - IdotN * IdotN);
                if (in_sqrt > 0.0f) // Not total reflection
                {
                    Vec3f T = (nr * IdotN - sqrt(in_sqrt)) * N - nr * I;
                    T.Normalize();
                    Ray ray_refrac(p_intersect, T);
                    Hit hit_refrac(99999999.0f, nullptr, Vec3f(0,0,0));
                    col += col_refrac * traceRay(ray_refrac, epsilon, bounces+1, 
                        weight * col_refrac.Length(), nt, hit_refrac);
                    
                    //std::cout << N << " " << nr << std::endl;

                    RayTree::AddTransmittedSegment(ray_refrac, epsilon, hit_refrac.getT());
                }
                //Vec3f T = (nt * VdotN )
            }
            

            color = col;
        }
    }
    //std::cout << "Final Hit: " << hit << " // " << hit.getIntersectionPoint() << std::endl;
    return color;
}