// #include </opt/homebrew/include/SDL2/SDL.h>
#include <SDL2/SDL.h>
#include <bits/stdc++.h>
#include <vector>
#include <cmath>
#include "vec3.h"
#include "camera.h"

#define PI 3.14159

int main(){
    WorldSet* rooms = new WorldSet;
    World* world1 = new World();    Box *box1 = new Box(vec3(0, 0, 0), vec3(1000, 2000, 1000));
    World* world2 = new World();    Box *box2 = new Box(vec3(2000, -0, 0), vec3(1000, 2000, 1000));

    rooms->addWorld(world1, box1);
    rooms->addWorld(world2, box2);

    Camera camera(rooms);
    camera.image_width = 500;
    camera.aspect_ratio = 4. / 3;
    camera.camera_center = vec3(0, 0, -1.5);
    camera.initialize();
    double window_scale = 4;

    SDL_Renderer* renderer = NULL;
        
    SDL_Window* window = NULL;

    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    uint32_t* framebuffer;
    framebuffer = new uint32_t[camera.image_width * camera.image_height];

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    window = SDL_CreateWindow("3d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.image_width*window_scale, camera.image_height*window_scale, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        camera.image_width,
        camera.image_height
    );
    if (!texture) {
        return 1;
    }

    SDL_Event event;

    // todo: fix spheres not drawing
    
    world1->addBox(new Box(vec3(0, 0, 0), vec3(1000, 1, 1000)));
    world2->addBox(new Box(vec3(2000, 0, 0), vec3(1000, 1, 1000)));

    // world1->addSphere(new Sphere(vec3(0, 0, 10), 10));
    // world1->addSphere(new Sphere(vec3(10, 0, 0), 10));
    // world1->   addBox(new Box(vec3(0, -2, 0), vec3(100, 2, 100)));
    // world2->   addBox(new Box(vec3(50, -100, 0), vec3(2, 100, 100)));
    // world2->addSphere(new Sphere(vec3(60, -100, 0), 10));
    // world2->addTorus(new Torus(vec3(0, -20, 0), 50, 15));



    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT){
                running = 0;
            }
        }
        
        camera.updateWorld();
        // Clear screen with a color (e.g., black)
        // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        // SDL_RenderClear(renderer);
        
        memset(framebuffer, 0, camera.image_width * camera.image_height * sizeof(uint32_t));

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        camera.input(keys);
        // camera.render(framebuffer);
        camera.renderLidar(framebuffer);

        SDL_UpdateTexture(
            texture,
            NULL,
            framebuffer,
            camera.image_width * sizeof(uint32_t)
        );
        
        std::cout << camera.camera_center.x() << ' ' << camera.camera_center.y() << ' ' << camera.camera_center.z() << std::endl;
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    
    
    delete world1;
    // delete framebuffer;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}
