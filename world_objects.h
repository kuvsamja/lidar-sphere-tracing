#include "vec3.h"
#include <bits/stdc++.h>

inline double dist(vec3 a, vec3 b){
    return (a - b).length();
}



class Sphere{
  private:
    
    vec3 c;
    double r;

  public:
    Sphere(vec3 center, double radius){
        c = center;
        r = radius;
    }

    double& getRadius(){
        return r;
    }

    vec3& getCenter(){
        return c;
    }

    double pointSphereDist(vec3 point){
        return dist(c, point) - r;
    }
};


class Box{ // Axis Aligned Box
  private:
    vec3 c;
    vec3 half_size;
  public:
    
    Box(vec3 center, vec3 half_size){
        c = center;
        this->half_size = half_size;
    }
        
    double pointBoxDist(vec3 point){
        vec3 dist = abs(point - c) - half_size;

        double outside = max(dist, vec3(0,0,0)).length();
        double inside = std::min(std::max({dist.x(), dist.y(), dist.z()}), 0.0);

        return outside + inside;
    }


};


class World{
  private:
    std::vector<Sphere*> world_spheres;
    std::vector<Box*> world_boxes;
  public:
    void addSphere(Sphere* sphere){
        world_spheres.push_back(sphere);
    }
    void addBox(Box* box){
        world_boxes.push_back(box);
    }

    double minDist(vec3 point){
        double min_dist = 999;

        for(Sphere* sphere:world_spheres){
            double dist_to_sphere = sphere->pointSphereDist(point);
            if(dist_to_sphere < min_dist)
                min_dist = dist_to_sphere;
        }

        for(Box* box:world_boxes){
            double dist_to_box = box->pointBoxDist(point);
            if(dist_to_box < min_dist)
                min_dist = dist_to_box;
        }

        return min_dist;
    }

};

