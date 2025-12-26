#include <SDL2/SDL.h>
#include <bits/stdc++.h>
#include <cmath>
#include "vec3.h"
#include "camera.h"

#define PI 3.14159

int main(){
    World* world = new World();

    Camera camera(world);
    camera.image_width = 200;
    camera.aspect_ratio = 4. / 3;
    camera.initialize();


    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    window = SDL_CreateWindow("3d", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, camera.image_width, camera.image_height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        return 1;
    }

    SDL_Event event;

    // todo: fix spheres not drawing
    
    world->addSphere(new Sphere(vec3(-10, 0, 0), 1));
    world->addSphere(new Sphere(vec3(10, 0, 0), 1));
    world->   addBox(new Box(vec3(0, 0, 10), vec3(1, 2, 1)));
    world->addSphere(new Sphere(vec3(0, 0, -10), 1));


    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT){
                running = 0;
            }
        }
        
        // Clear screen with a color (e.g., black)
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        camera.move(keys);
        camera.render(renderer);



        SDL_RenderPresent(renderer);   // show frame
        // SDL_Delay(16);                 // avoid maxing CPU
    }
    
    
    delete world;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}