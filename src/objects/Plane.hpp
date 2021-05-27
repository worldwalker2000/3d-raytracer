#pragma once

#include "Object.hpp"

class Plane : public Object
{
private:
    glm::vec3 normal;

public:
    Plane(glm::vec3 pos, glm::vec3 normal, SDL_Color color);

    bool intersectsRay(Ray ray, glm::vec3 &intersectPoint) override;
    glm::vec3 getNormal(glm::vec3 intersectPoint) override;

    inline ObjectType getType() override { return ObjectType::PLANE; }
};