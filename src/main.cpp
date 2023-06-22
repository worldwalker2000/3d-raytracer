#include <iostream>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
using glm::vec3;

#include "Level.hpp"
#include "objects/Sphere.hpp"
#include "objects/Plane.hpp"
#include "objects/Light.hpp"

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("3d-raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    int bounces = 2;
    Level level(vec3(0, 10, -100), vec3(0, 1, 0));

    level.addObject(new Plane(vec3(-100, 0, 0), vec3(1, 0, 0), {130, 69, 33, 255}));
    level.addObject(new Plane(vec3(100, 0, 0), vec3(-1, 0, 0), {130, 69, 33, 255}));
    level.addObject(new Plane(vec3(0, -100, 0), vec3(0, 1, 0), {128, 123, 120, 255}));
    level.addObject(new Plane(vec3(0, 100, 0), vec3(0, -1, 0), {227, 223, 220, 255}));
    level.addObject(new Plane(vec3(0, 0, 100), vec3(0, 0, -1), {130, 69, 33, 255}));

    level.addObject(new Light(vec3(0, 0, -50), {255, 255, 255, 255}));
    level.addObject(new Light(vec3(0, 0, 0), {255, 0, 0, 255}));

    level.addObject(new Sphere(vec3(30, 20, 10), 10, {255, 0, 0, 255}));
    level.addObject(new Sphere(vec3(-50, 10, 0), 30, {0, 255, 0, 255}));
    level.addObject(new Sphere(vec3(50, -70, 50), 20, {128, 45, 60, 255}));
    level.addObject(new Sphere(vec3(-50, -70, 0), 15, {0, 0, 128, 255}));

    bool running = true;

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    level.setDirty(true);
                    break;
                case SDLK_EQUALS:
                    bounces++;
                    break;
                case SDLK_MINUS:
                    bounces--;
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0xff, 0xff);
        SDL_RenderClear(renderer);

        level.render(renderer, bounces);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
