#include "Sphere.hpp"

#include <glm/gtx/intersect.hpp>

Sphere::Sphere(glm::vec3 pos, float radius, SDL_Color color)
    : Object(pos, color), radius(radius)
{
}

bool Sphere::intersectsRay(Ray ray, glm::vec3 &intersectPoint)
{
    float iDist;
    bool intersects = glm::intersectRaySphere(ray.pos, ray.dir, pos, pow(radius, 2), iDist);

    if (intersects)
    {
        intersectPoint = ray.pos + ray.dir * iDist;
        return true;
    }

    return false;
}

glm::vec3 Sphere::getNormal(glm::vec3 intersectPoint)
{
    return glm::normalize(intersectPoint - pos);
}