#include "Level.hpp"

#include <iostream>
#include <limits>
#include <functional>
#include <typeinfo>
#include <numeric>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/hash.hpp>

#include "objects/Light.hpp"
#include "src/objects/Object.hpp"

Level::Level(glm::vec3 cameraPos, glm::vec3 cameraDir)
    : cameraPos(cameraPos), cameraDir(cameraDir), dirty(true)
{
}

void Level::addObject(Object *object)
{
    objects.push_back(object);

    dirty = true;
}

void Level::setCameraPosition(glm::vec3 pos)
{
    cameraPos = pos;

    dirty = true;
}

void Level::setCameraDir(glm::vec3 dir)
{
    cameraDir = dir;

    dirty = true;
}

void Level::setDirty(bool dirty)
{
    this->dirty = dirty;
}

std::pair<glm::vec3, Object *> Level::resolveClosestObject(Ray ray)
{
    std::unordered_map<glm::vec3, Object *> possible;

    for (Object *object : objects)
    {
        glm::vec3 intersectPoint;
        if (object->intersectsRay(ray, intersectPoint))
            possible.insert(std::pair(intersectPoint, object));
    }

    if (possible.size() == 0)
        return std::make_pair(glm::vec3(0), nullptr);

    float dist = std::numeric_limits<float>::infinity();
    Object *closestObject;
    glm::vec3 closestIntersectPoint;
    for (auto [intersectPoint, object] : possible)
    {
        float newDist = glm::distance(ray.pos, intersectPoint);
        if (newDist < dist)
        {
            dist = newDist;
            closestObject = object;
            closestIntersectPoint = intersectPoint;
        }
    }

    return std::make_pair(closestIntersectPoint, closestObject);
}

SDL_Color Level::resolveColorAtPixel(int x, int y, int viewWidth, int viewHeight, int bounces)
{
    static const SDL_Color skyColor = {0, 0, 255, 255};
    static const float viewDistFromCamera = 500;

    // Basic coloring and "reflections"
    Ray ray;
    ray.pos = cameraPos;
    ray.dir = glm::normalize(glm::vec3(cameraPos.x - (viewWidth / 2) + x, cameraPos.y + (viewHeight / 2) - y, cameraPos.z + viewDistFromCamera) - cameraPos);

    auto [closestIntersectPoint, closestObject] = resolveClosestObject(ray);
    if (closestObject == nullptr)
        return skyColor;

    SDL_Color finalColor = closestObject->getColor();
    while (bounces > 0)
    {
        auto [otherClosestIntersectPoint, otherClosestObject] = resolveClosestObject(ray);

        if (otherClosestObject == nullptr)
            break;

        SDL_Color closestColor = otherClosestObject->getColor();

        float finalColorPart = 0.3;
        float closestColorPart = 0.7;
        assert(finalColorPart + closestColorPart == 1.0);

        finalColor.r = (finalColor.r * finalColorPart) + (closestColor.r * closestColorPart);
        finalColor.g = (finalColor.g * finalColorPart) + (closestColor.g * closestColorPart);
        finalColor.b = (finalColor.b * finalColorPart) + (closestColor.b * closestColorPart);

        if (otherClosestObject->getType() == ObjectType::PLANE)
        {
            break;
        }

        // ray.dir = otherClosestObject->getNormal(otherClosestIntersectPoint);
        ray.dir = glm::reflect(ray.dir, otherClosestObject->getNormal(otherClosestIntersectPoint));
        // Again move just abit forward to avoid self intersection
        ray.pos = otherClosestIntersectPoint + ray.dir * glm::vec3(0.01);

        bounces--;
    }

    // Lighting
    float maxLightDist = 0;
    for (Object *light : objects)
        if (light->getType() == ObjectType::LIGHT)
            maxLightDist = glm::max(glm::distance(light->getPos(), closestIntersectPoint), maxLightDist);

    static const float diffuse = 0.2;
    bool inShadow = false;
    glm::vec3 lightBrightnes = glm::vec3();
    for (Object *light : objects)
    {
        if (light->getType() == ObjectType::LIGHT)
        {
            Ray toLight;
            toLight.dir = glm::normalize(light->getPos() - closestIntersectPoint);
            // Move the position of the ray just abit out infront to prevent the object that is being checked for shadow thinking that it is in the shadow of it self
            toLight.pos = closestIntersectPoint + toLight.dir * glm::vec3(0.01);

            float dot = glm::dot(toLight.dir, closestObject->getNormal(closestIntersectPoint));

            auto [otherClosestIntersectPoint, otherClosestObject] = resolveClosestObject(toLight);

            float distFactor = glm::distance(light->getPos(), closestIntersectPoint) / maxLightDist;
            // Don't need total brightness but need brightness of each light color and then us that to calculate final color
            float brightness = glm::max(dot, diffuse) * distFactor;

            // 0.01 not 0 just ot get rid of some pixel wide bands of shadow
            if (dot > 0.01 && otherClosestObject != nullptr)
                inShadow = true;
            // not in shadow if the distance between the intersectPoint and the light is less thand the distatnce between intersectPoint and otherIntersect point
            if (glm::distance(closestIntersectPoint, light->getPos()) < glm::distance(closestIntersectPoint, otherClosestIntersectPoint))
                inShadow = false;

            lightBrightnes.r += light->getColor().r / 255.0f * brightness;
            lightBrightnes.g += light->getColor().g / 255.0f * brightness;
            lightBrightnes.b += light->getColor().b / 255.0f * brightness;
        }
    }

    if (inShadow)
    {
        static const float shadowBrightnes = 0.3;
        finalColor.r *= shadowBrightnes;
        finalColor.g *= shadowBrightnes;
        finalColor.b *= shadowBrightnes;
    }
    else
    {
        finalColor.r *= glm::clamp(lightBrightnes.r, (float)0, (float)0.9);
        finalColor.g *= glm::clamp(lightBrightnes.g, (float)0, (float)0.9);
        finalColor.b *= glm::clamp(lightBrightnes.b, (float)0, (float)0.9);
    }

    return finalColor;
}

void Level::render(SDL_Renderer *renderer, int bounces)
{
    if (dirty)
    {
        int viewWidth, viewHeight;
        SDL_GetRendererOutputSize(renderer, &viewWidth, &viewHeight);

        if (!texture)
        {
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, viewWidth, viewHeight);
        }

        SDL_SetRenderTarget(renderer, texture);

        for (int x = 0; x < viewWidth; x++)
            for (int y = 0; y < viewWidth; y++)
            {
                SDL_Color pixel = resolveColorAtPixel(x, y, viewWidth, viewHeight, bounces);

                SDL_SetRenderDrawColor(renderer, pixel.r, pixel.g, pixel.b, pixel.a);
                SDL_RenderDrawPoint(renderer, x, y);
            }

        SDL_SetRenderTarget(renderer, NULL);
        dirty = false;
    }
    else
    {
        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }
}
