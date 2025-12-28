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

    int image_height;
    double viewport_width;
    double viewport_height;
    double pixel_width;
    double pixel_height;
    
    double max_spheres = 100;
    double sphere_detect_size = 0.001;

    World* world;
    
    double speed = 0.05;
    double sensitivity = 0.1;

    std::deque<vec3> lidar_points;
    uint64_t max_lidar_points = 500000;
    int lidar_density = 1; // bigger number, less density

    Camera(World* world){
        this->world = world;
    }
    // initialize the viewport
    void initialize(){
        image_height = (int)(image_width / aspect_ratio);
        viewport_width = 2*focal_length*std::tan(fov/2);
        viewport_height = viewport_width / aspect_ratio;
        pixel_width = viewport_width / image_width;
        pixel_height = viewport_height / image_height;


    }

    // camera movement
    void input(const Uint8* keys){
        if(keys[SDL_SCANCODE_LEFT])
            angle_x -= sensitivity;
        if(keys[SDL_SCANCODE_RIGHT])
            angle_x += sensitivity;
        if(keys[SDL_SCANCODE_DOWN])
            angle_y -= sensitivity;
        if(keys[SDL_SCANCODE_UP])
            angle_y += sensitivity;

        double CS = cos(angle_x);
        double SN = sin(angle_x);
        
        if(keys[SDL_SCANCODE_W]){
            camera_center[2] += speed*CS;
            camera_center[0] += speed*SN;
        }
        if(keys[SDL_SCANCODE_S]){
            camera_center[2] -= speed*CS;
            camera_center[0] -= speed*SN;
        }
        if(keys[SDL_SCANCODE_A]){
            camera_center[2] += speed*SN;
            camera_center[0] -= speed*CS;
        }
        if(keys[SDL_SCANCODE_D]){
            camera_center[2] -= speed*SN;
            camera_center[0] += speed*CS;
        }
        if(keys[SDL_SCANCODE_SPACE]){
            camera_center[1] -= speed;
        }
        if(keys[SDL_SCANCODE_LSHIFT]){
            camera_center[1] += speed;
        }
        angle_y = fmod(angle_y, 2*PI);
        angle_x = fmod(angle_x, 2*PI);

        if(keys[SDL_SCANCODE_RETURN]){
            castLidarRay();
        }
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


    void render(uint32_t* framebuffer){
        vec3 pixel_00_not_rotated = camera_center + vec3(-viewport_width / 2, -viewport_height / 2, focal_length);
        vec3 half_pixel_offset = vec3(pixel_width * 0.5, pixel_height * 0.5, 0);

        for(int i = 0; i < image_width; i++){
            for(int j = 0; j < image_height; j++){
                renderPixel(framebuffer, i, j, pixel_00_not_rotated, half_pixel_offset);

            }
        }
    }

    void renderLidar(uint32_t* framebuffer){
        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0x1);
        for(vec3 point:lidar_points){
            vec3 v = point - camera_center;
            
            v = rotateY(v, -angle_x);
            v = rotateX(v, -angle_y);

            if (v.z() <= 1e-6) continue;

            double x_img = (focal_length * v.x()) / v.z();
            double y_img = (focal_length * v.y()) / v.z();

            int screen_x = (int)(image_width  / 2 + x_img / pixel_width);
            int screen_y = (int)(image_height / 2 + y_img / pixel_height);

            if (screen_x < 0 || screen_x >= image_width || screen_y < 0 || screen_y >= image_height)
                continue;
            
            // SDL_RenderDrawPoint(renderer, screen_x, screen_y);
            framebuffer[screen_x + image_width*screen_y] = 0xFFFFFFFF;
        }

    }

    void castLidarRay(){

        vec3 pixel_00_not_rotated = camera_center + vec3(-viewport_width / 2, -viewport_height / 2, focal_length);
        vec3 half_pixel_offset = vec3(pixel_width * 0.5, pixel_height * 0.5, 0);

        for(int i = 0; i < image_width-0; i++){
            for(int j = 0; j < image_height-0; j++){
                // if((int)((double)rand() / RAND_MAX * lidar_density) != 1)
                //     continue;

                vec3 pixel_loc = pixel_00_not_rotated + vec3(i*pixel_width, j*pixel_height, 0) + half_pixel_offset;

                vec3 translated_pixel = pixel_loc - camera_center;

                translated_pixel = rotateX(translated_pixel, angle_y);
                translated_pixel = rotateY(translated_pixel, angle_x);
                
                ray r(camera_center, translated_pixel);

                vec3 lidar_point = sphereCast(r, *world);

                if(lidar_point.is_null == 0){
                    lidar_points.push_back(lidar_point);
                    if((uint64_t)lidar_points.size() > max_lidar_points){
                        lidar_points.pop_front();
                    }
                }
            }
        }
        
        

        
    }
    void renderPixel(uint32_t* framebuffer, int x, int y, vec3 pixel_00_not_rotated, vec3 half_pixel_offset){
        // calculate pixel locations in 3d space
        vec3 pixel_loc = pixel_00_not_rotated + vec3(x*pixel_width, y*pixel_height, 0) + half_pixel_offset;

        vec3 translated_pixel = pixel_loc - camera_center;

        translated_pixel = rotateX(translated_pixel, angle_y);
        translated_pixel = rotateY(translated_pixel, angle_x);
        
        ray r(camera_center, translated_pixel);

        int step_count = sphereCastOcclusion(r, *world);
        if(step_count != 0){
            // std::clog << step_count;
            // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0x1);
            // SDL_RenderDrawPoint(renderer, x, y);
            // framebuffer[x + image_width*y] = 0xFF + 0x00111111*step_count;
            if(0x00030303*step_count > 0x0FFFFFF){
                framebuffer[x + image_width*y] = 0xFF000000;
                return;
            }
            framebuffer[x + image_width*y] = 0xFF000000 + 0x00FFFFFF - 0x00030303*step_count;
            return;
        }
    
    }


    int sphereCastOcclusion(ray r, World world){
        double length = 0;
        int steps = 0;
        length = world.minDist(r.origin());
        r.direction() = unit_vector(r.direction());
        double dist_to_obj;
        
        for(int sphere_count = 0; sphere_count < max_spheres; sphere_count++){
            dist_to_obj = world.minDist(r.direction()*length + r.origin());
            
            
            steps++;
            if(dist_to_obj < sphere_detect_size){
                return steps;
            }
            length += dist_to_obj;
        }

        // if(dist_to_obj < sphere_detect_size){
        //     return steps;
        // }

        // vec3 intersection = r.direction()*length + r.origin();
        // intersection.is_null = 1;
        return 0;
    }


    vec3 sphereCast(ray r, World world){
        double length = 0;
        
        length = world.minDist(r.origin());
        r.direction() = unit_vector(r.direction());
        double dist_to_obj;
        for(int sphere_count = 0; sphere_count < max_spheres; sphere_count++){
            dist_to_obj = world.minDist(r.direction()*length + r.origin());
            
            
            length += dist_to_obj;
        }

        if(dist_to_obj < sphere_detect_size){
            return r.origin() + r.direction()*length;
        }

        vec3 intersection = r.direction()*length + r.origin();
        intersection.is_null = 1;
        return intersection;
    }
};
