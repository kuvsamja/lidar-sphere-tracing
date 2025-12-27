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

class MandelBulb{
  private:
    vec3 c;
    double power;
    double bailout;
    int max_iterations;
  public:
    MandelBulb(){
        c = vec3(0, 0, 0);
        power = 8;
        bailout = 7;
        max_iterations = 3000;
    }

    double pointMandelbulbDist(vec3 point){
        vec3 z = point - c;
        double dr = 1.0;
        double r = 0.0;
        
        for(int i = 0; i < max_iterations; i++){
            r = z.length();
            if(r > bailout) break;
            
            // Convert to polar coordinates
            double theta = acos(z.z() / r);
            double phi = atan2(z.y(), z.x());
            dr = pow(r, power - 1.0) * power * dr + 1.0;
            
            // Scale and rotate the point
            double zr = pow(r, power);
            theta = theta * power;
            phi = phi * power;
            
            // Convert back to cartesian coordinates
            z = zr * vec3(sin(theta) * cos(phi), 
                         sin(theta) * sin(phi), 
                         cos(theta));
            z = z + (point - c);
        }
        
        return 0.5 * log(r) * r / dr;
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
    std::vector<MandelBulb*> world_mandelbulbs;
  public:
    void addSphere(Sphere* sphere){
        world_spheres.push_back(sphere);
    }
    void addBox(Box* box){
        world_boxes.push_back(box);
    }
    void addMandelbulb(MandelBulb* mandelbulb){
        world_mandelbulbs.push_back(mandelbulb);
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

        for(MandelBulb* mandelbulb:world_mandelbulbs){
            double dist_to_bulb = mandelbulb->pointMandelbulbDist(point);
            if(dist_to_bulb < min_dist)
                min_dist = dist_to_bulb;
        }
        return min_dist;
    }

};

