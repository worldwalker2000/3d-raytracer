#pragma once

#include "Object.hpp"

#include <glm/glm.hpp>

class Light : public Object
{
public:
    Light(glm::vec3 pos, SDL_Color color);

    inline ObjectType getType() override { return ObjectType::LIGHT; }
};