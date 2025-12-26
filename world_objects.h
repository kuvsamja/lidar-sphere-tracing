#include "vec3.h"
#include <bits/stdc++.h>

double dist(vec3 a, vec3 b){
    return std::sqrt((a.x() - b.x())*(a.x() - b.x()) + (a.y() - b.y())*(a.y() - b.y()) + (a.z() - b.z())*(a.z() - b.z()));
}



class Sphere{
  private:
    
    vec3 c;
    double r;

  public:
    Sphere(){
        c = vec3(0, 0, 0);
        r = 0;
    }

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


class World{
  private:
    std::vector<Sphere*> world_spheres;

  public:
    void addSphere(Sphere* sphere){
        world_spheres.push_back(sphere);
    }

    double minDist(vec3 point){
        double min_dist = 999;

        for(Sphere* sphere:world_spheres){
            double dist_to_sphere = (*sphere).pointSphereDist(point);
            if(dist_to_sphere < min_dist)
                min_dist = dist_to_sphere;
        }

        return min_dist;
    }

};

