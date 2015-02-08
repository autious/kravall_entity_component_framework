#pragma once
#include <glm/glm.hpp>

struct TransformableComponent
{
    glm::vec2 position;
    float rotation;
    glm::vec2 scale;

    static const char* GetName() { return "TransformableComponent"; }
};
