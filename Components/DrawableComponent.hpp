#pragma once

struct DrawableComponent
{
    const char* texturePath;

    static const char* GetName() { return "DrawableComponent"; }
};
