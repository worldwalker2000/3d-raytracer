#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include "../Ray.hpp"

enum ObjectType
{
    SPHERE,
    PLANE,
    LIGHT,
};

class Object
{
protected:
    glm::vec3 pos;
    SDL_Color color;

public:
    Object(glm::vec3 pos, SDL_Color color);

    virtual bool intersectsRay(Ray ray, glm::vec3 &intersectPoint);
    virtual glm::vec3 getNormal(glm::vec3 intersectPoint);

    const glm::vec3 &getPos();
    SDL_Color getColor();

    inline virtual ObjectType getType() = 0;
};
