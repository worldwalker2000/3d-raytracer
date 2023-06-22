#pragma once

#include <vector>
#include <SDL2/SDL.h>

#include "objects/Object.hpp"
#include "Ray.hpp"

class Level
{
private:
    std::vector<Object *> objects;

    glm::vec3 cameraPos;
    glm::vec3 cameraDir;

    bool dirty;

    SDL_Texture *texture = NULL;

    std::pair<glm::vec3, Object *> resolveClosestObject(Ray ray);
    SDL_Color resolveColorAtPixel(int x, int y, int viewWidth, int viewHeight, int bounces);

public:
    Level(glm::vec3 cameraPos, glm::vec3 cameraDir);

    void addObject(Object *object);
    void setCameraPosition(glm::vec3 pos);
    void setCameraDir(glm::vec3 dir);

    void setDirty(bool dirty);

    void render(SDL_Renderer *renderer, int bounces);
};