#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <chrono>
#include <string>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#define WIDTH 1600
#define HEIGHT 900

class Timer
{
public:
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<float> duration;

    void startTimer()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    float stopTimer()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        float ms = duration.count() * 1000.0f;
        return ms;
    }
};

float clamp(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;

    return x;
}

struct Sphere
{
    glm::vec3 position;
    float radius;
};

struct Ray
{
    glm::vec3 position;
    glm::vec3 direction;
};

std::vector<Sphere> scene;

glm::vec3 skyColor;

glm::vec3 light;
glm::vec3 lightColor;

glm::vec3 cameraLocation;
int raysCast;

bool rendering = false;

void PaintColorAtRay(SDL_Renderer* renderer, Ray ray, int i, int j, int check = -1)
{
    float normal;

    raysCast++;
    if (glm::intersectRayPlane(ray.position, ray.direction, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), normal))
    {
        glm::vec3 finalColor(0, 100, 0);

        glm::vec3 intersect(ray.position + ray.direction * normal);
        Ray toIsect{ intersect, glm::normalize(light - intersect) };

        for (int l = 0; l < scene.size(); l++)
        {
            if (check == l) continue;

            raysCast++;
            if (glm::intersectRaySphere(toIsect.position, toIsect.direction, scene[l].position, pow(scene[l].radius, 2), normal))
            {
                finalColor = glm::vec3(0, 50, 0);
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, finalColor.r, finalColor.g, finalColor.b, 255);
        SDL_RenderDrawPoint(renderer, i, HEIGHT - j);
    }

    for (int k = 0; k < scene.size(); k++)
    {
        float normal;

        raysCast++;
        if (glm::intersectRaySphere(ray.position, ray.direction, scene[k].position, pow(scene[k].radius, 2), normal)) //rays[ii].intersectsSphere(scene[k], &normal))
        {
            glm::vec3 intersect(ray.position + ray.direction * normal);
            Ray toIsect{ scene[k].position, glm::normalize(intersect - scene[k].position) };
            Ray toLight{ scene[k].position, glm::normalize(light - scene[k].position) };

            float dot = glm::dot(toIsect.direction, toLight.direction);
            float br = fmax(dot, 0);
            glm::vec3 diffuse = lightColor * br;

            glm::vec3 color(255, 0, 0);

            glm::vec3 finalColor = color * diffuse;

            for (int l = 0; l < scene.size(); l++)
            {
                if (k == l) continue;
                if (check == k) continue;

                raysCast++;
                if (glm::intersectRaySphere(toIsect.position, toIsect.direction, scene[l].position, pow(scene[l].radius, 2), normal))
                {
                    //finalColor = glm::vec3(255, 0, 0);
                    
                    intersect = glm::vec3(toIsect.position + toIsect.direction * normal);
                    toIsect = { scene[l].position, glm::normalize(intersect - scene[l].position) };
                    toLight = { scene[l].position, glm::normalize(light - scene[l].position) };

                    dot = glm::dot(toIsect.direction, toLight.direction);
                    br = fmax(dot, 0);
                    diffuse = lightColor * br;

                    finalColor = color * diffuse;
                    break;
                }
            }

            if (check == k) finalColor = lightColor * glm::vec3(255);

            SDL_SetRenderDrawColor(renderer, finalColor.r, finalColor.g, finalColor.b, 255);
            SDL_RenderDrawPoint(renderer, i, HEIGHT - j);
            break;
        }
    }
}

void RenderScene(SDL_Renderer* renderer)
{
    raysCast = 0;

    Timer timer;
    timer.startTimer();

    SDL_SetRenderDrawColor(renderer, skyColor.r, skyColor.g, skyColor.b, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
        {
            PaintColorAtRay(renderer, { cameraLocation, glm::normalize(glm::vec3(i, j, 0) - cameraLocation) }, i, j, 0);
        }

    std::cout << std::endl << "Frame took " << timer.stopTimer() << "ms" << " checking " << raysCast << " rays" << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "Press w to add sphere at random position with random radius. Press Space to re draw frame." << std::endl;

    skyColor = glm::vec3(100, 149, 237);

    light = glm::vec3(WIDTH/2, HEIGHT-50, 0);
    lightColor = glm::vec3(1, 1, 1);
    scene.push_back({ light, 10 });

    scene.push_back({ glm::vec3(WIDTH / 2, HEIGHT / 2, 100), 50 });
    scene.push_back({ glm::vec3(WIDTH / 2 - 300, HEIGHT / 2, 200), 100 });
    scene.push_back({ glm::vec3(WIDTH / 2 + 300, HEIGHT / 2 + 300, 200), 100 });

    cameraLocation = glm::vec3(WIDTH / 2, HEIGHT / 2, -1000);

    SDL_Window* window = SDL_CreateWindow("3d Ray Tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    RenderScene(renderer);

    // Display Scene
    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) RenderScene(renderer);
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_w) scene.push_back({ glm::vec3(rand() % WIDTH, rand() % HEIGHT, rand() % 1000), (float)(rand() % 50 + 10) });
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(33);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}