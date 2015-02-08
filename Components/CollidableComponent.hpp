#pragma once
#include <glm/glm.hpp>

struct CollidableComponent
{
    enum Shape
    {
        Sphere,
        Unknown
    };

    Shape shape;
    float radius;
    glm::vec2 offset;

    bool collided = false;

    static const char* GetName() { return "CollidableComponent"; }
};
