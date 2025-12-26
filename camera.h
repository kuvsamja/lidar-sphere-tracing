#include <SDL2/SDL.h>
#include "vec3.h"
#include "ray.h"
#include "world_objects.h"

#define PI 3.14159

class Camera{
  public:
    double fov = PI/2;
    vec3 camera_center = vec3(0, 0, 0);
    int image_width = 400;
    double aspect_ratio = 4. / 3;
    double focal_length = 1;
    double angle_x = 0;
    double angle_y = 0;

    double image_height;
    double viewport_width;
    double viewport_height;
    double pixel_width;
    double pixel_height;
    
    double max_spheres = 10;
    World* world;
    
    double speed = 1;
    double sensitivity = 0.1;
    Camera(World* world){
        this->world = world;
    }
    // initialize the viewport
    void initialize(){
        image_height = (int)(image_width / aspect_ratio);
        viewport_width = focal_length*std::tan(fov/2);
        viewport_height = viewport_width / aspect_ratio;
        pixel_width = viewport_width / image_width;
        pixel_height = viewport_height / image_height;


    }

    // camera movement
    void move(const Uint8* keys){
        if(keys[SDL_SCANCODE_LEFT])
            angle_x -= sensitivity;
        if(keys[SDL_SCANCODE_RIGHT])
            angle_x += sensitivity;
        if(keys[SDL_SCANCODE_DOWN])
            angle_y -= sensitivity;
        if(keys[SDL_SCANCODE_UP])
            angle_y += sensitivity;

        if(keys[SDL_SCANCODE_W]){
            camera_center[2] += speed*cos(angle_x);
            camera_center[0] += speed*sin(angle_x);
        }
        if(keys[SDL_SCANCODE_S]){
            camera_center[2] -= speed*cos(angle_x);
            camera_center[0] -= speed*sin(angle_x);
        }
        if(keys[SDL_SCANCODE_A]){
            camera_center[2] += speed*sin(angle_x);
            camera_center[0] -= speed*cos(angle_x);
        }
        if(keys[SDL_SCANCODE_D]){
            camera_center[2] -= speed*sin(angle_x);
            camera_center[0] += speed*cos(angle_x);
        }   
        angle_y = fmod(angle_y, 2*PI);

        angle_x = fmod(angle_x, 2*PI);

    }

    // rotation around a point
    vec3 rotateY(vec3 v, double angle) {
        double c = cos(angle);
        double s = sin(angle);
        return vec3(
            c * v.x() + s * v.z(),
            v.y(),
            -s * v.x() + c * v.z()
        );
    }

    vec3 rotateX(vec3 v, double angle) {
        double c = cos(angle);
        double s = sin(angle);
        return vec3(
            v.x(),
            c * v.y() - s * v.z(),
            s * v.y() + c * v.z()
        );
    }


    void render(SDL_Renderer* renderer){
        vec3 pixel_00_not_rotated = camera_center + vec3(-viewport_width / 2, -viewport_height / 2, focal_length);

        for(int i = 0; i < image_width; i++){
            for(int j = 0; j < image_height; j++){
                // calculate pixel locations in 3d space
                vec3 pixel_loc = pixel_00_not_rotated + vec3(i*pixel_width, 0, 0) + vec3(0, j*pixel_height, 0);
                pixel_loc += 0.5*vec3(pixel_width, pixel_height, 0);

                vec3 translated_pixel = pixel_loc - camera_center;

                translated_pixel = rotateX(translated_pixel, angle_y);
                translated_pixel = rotateY(translated_pixel, angle_x);
                
                pixel_loc = camera_center + translated_pixel;

                auto ray_direction = pixel_loc - camera_center;
                ray r(camera_center, ray_direction);

                vec3 pixel_color = sphereCast(r, *world);
            
                SDL_SetRenderDrawColor(renderer, pixel_color[0], pixel_color[1], pixel_color[2], 0x1);
                SDL_RenderDrawPoint(renderer, i, j);
            }
        }
    }

    vec3 sphereCast(ray r, World world){
        int length = 0;
        
        length = world.minDist(r.origin());
        r.setRayLength(length);

        for(int sphere_count = 0; sphere_count < max_spheres; sphere_count++){
            sphere_count++;
            double dist_to_obj = world.minDist(r.direction() + r.origin());
            
            if(dist_to_obj < 1){
                return vec3(255, 0, 0);
            }
            length += dist_to_obj;
            r.setRayLength(length);
        }

        return vec3(180, 200, 255);
    }
};
