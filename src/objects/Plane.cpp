#include "Plane.hpp"

#include <glm/gtx/intersect.hpp>

Plane::Plane(glm::vec3 pos, glm::vec3 normal, SDL_Color color)
    : Object(pos, color), normal(normal)
{
}

bool Plane::intersectsRay(Ray ray, glm::vec3 &intersectPoint)
{
    float iDist;
    bool intersects = glm::intersectRayPlane(ray.pos, ray.dir, pos, normal, iDist);

    if (intersects)
    {
        intersectPoint = ray.pos + ray.dir * iDist;
        return true;
    }

    return false;
}

glm::vec3 Plane::getNormal(glm::vec3 intersectPoint)
{
    return normal;
}