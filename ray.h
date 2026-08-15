#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
  public:
    ray() {}

    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

    point3& origin() { return orig; }
    vec3& direction() { return dir; }

    point3 at(double t) const {
        return orig + t*dir;
    }

    void setRayLength(double length){
        dir = dir / dir.length() * length;
    }
    
  private:
    point3 orig;
    vec3 dir;
};

#endif