#include "Object.hpp"

Object::Object(glm::vec3 pos, SDL_Color color)
    : pos(pos), color(color)
{
}

bool Object::intersectsRay(Ray ray, glm::vec3 &intersectPoint)
{
    return false;
}

glm::vec3 Object::getNormal(glm::vec3 intersectPoint)
{
    return glm::vec3(0);
}

const glm::vec3 &Object::getPos()
{
    return pos;
}

SDL_Color Object::getColor()
{
    return color;
}