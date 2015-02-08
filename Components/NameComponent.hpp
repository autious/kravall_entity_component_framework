#pragma once

struct NameComponent
{
    static const int NAME_LENGTH = 32;
    char name[NAME_LENGTH];

    static const char* GetName() { return "NameComponent"; }
};
