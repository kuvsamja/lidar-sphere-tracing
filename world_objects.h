#include "vec3.h"
#include <iostream>

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
    double scale;
  public:
    MandelBulb(vec3 center, double scale, double power, double bailout, double max_iterations){

        this->c = center;
        this->scale = scale;
        this->power = power;
        this->bailout = bailout;
        this->max_iterations = max_iterations;
    }

    double pointMandelbulbDist(vec3 point){
        vec3 z = (point - c) / scale;
        double dr = 1.0;
        double r = 0.0;
        
        for(int i = 0; i < max_iterations; i++){
            r = z.length();
            if(r > bailout) break;
            
            double theta = acos(z.z() / r);
            double phi = atan2(z.y(), z.x());
            dr = pow(r, power - 1.0) * power * dr + 1.0;
            
            double zr = pow(r, power);
            theta = theta * power;
            phi = phi * power;
            
            z = zr * vec3(sin(theta) * cos(phi), 
                         sin(theta) * sin(phi), 
                         cos(theta));
            z = z + (point - c) / scale;
        }
        
        return 0.5 * log(r) * r / dr * scale;
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

    int containsPoint(const vec3& point) {
        vec3 diff = abs(point - c);
        return diff.x() <= half_size.x()
            && diff.y() <= half_size.y()
            && diff.z() <= half_size.z();
    }


};

class Torus {
  private:
    vec3 center;
    double big_r;   // radius of the ring
    double small_r; // thickness of the tube
  public:
    Torus(vec3 center, double big_r, double small_r){
        this->center = center;
        this->big_r = big_r;
        this->small_r = small_r;
    }
    double pointTorusDist(vec3 point){
        vec3 p = point - center;
        double q_x = sqrt(p.x()*p.x() + p.z()*p.z()) - big_r;
        double q_y = p.y();
        return sqrt(q_x*q_x + q_y*q_y) - small_r;
    }
};

class World{
  private:

  public:
    std::vector<Sphere*> world_spheres;
    std::vector<Box*> world_boxes;
    std::vector<MandelBulb*> world_mandelbulbs;
    std::vector<Torus*> world_toruses;

    void addSphere(Sphere* sphere){
        world_spheres.push_back(sphere);
    }
    void addBox(Box* box){
        world_boxes.push_back(box);
    }
    void addMandelbulb(MandelBulb* mandelbulb){
        world_mandelbulbs.push_back(mandelbulb);
    }
    void addTorus(Torus* torus){
        world_toruses.push_back(torus);
    }
    World* operator+(World& world1){
        World* world_out = new World;
        *world_out = *this;
        for(auto s : world1.world_spheres)    world_out->addSphere(s);
        for(auto b : world1.world_boxes)      world_out->addBox(b);
        for(auto m : world1.world_mandelbulbs) world_out->addMandelbulb(m);
        for(auto t : world1.world_toruses)    world_out->addTorus(t);
        return world_out;

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
        for(Torus* torus : world_toruses){
            double d = torus->pointTorusDist(point);
            if(d < min_dist) min_dist = d;
        }

        return min_dist;
    }

};



class WorldSet{
  private:
    std::vector<World*> worlds;
    std::vector<Box*> bounding_boxes;
  public:
    WorldSet(std::vector<World*> worlds, std::vector<Box*> bounding_boxes) {
        this->worlds = worlds;
        this->bounding_boxes = bounding_boxes;
    }
    WorldSet() {
        this->worlds = {};
        this->bounding_boxes = {};
    }

    void addWorld(World* world, Box* bounding_box) {
        worlds.push_back(world);
        bounding_boxes.push_back(bounding_box);
    }

    World* currentWorld(vec3 position) {
        World* world_out = new World;
        for(uint64_t i = 0ull; i < bounding_boxes.size(); i++) {
            if (bounding_boxes[i]->containsPoint(position)) {
                world_out = *world_out + *(worlds[i]);
            }
        }

        return world_out;
    }
};