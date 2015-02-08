#pragma once

#include <glm/glm.hpp>

struct KinematicComponent
{
    glm::vec2 velocity;
    glm::vec2 acceleration;

    static const char* GetName() { return "KinematicComponent"; }
};
