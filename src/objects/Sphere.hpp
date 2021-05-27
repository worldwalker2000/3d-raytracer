#pragma once

#include <glm/glm.hpp>

#include "Object.hpp"

class Sphere : public Object
{
private:
    float radius;

public:
    Sphere(glm::vec3 pos, float radius, SDL_Color color);

    bool intersectsRay(Ray ray, glm::vec3 &intersectPoint) override;
    glm::vec3 getNormal(glm::vec3 intersectPoint) override;

    inline ObjectType getType() override { return ObjectType::SPHERE; }
};