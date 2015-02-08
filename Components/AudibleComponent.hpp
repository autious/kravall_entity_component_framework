#pragma once

struct AudibleComponent
{
    bool    playing = false;
    float   volume = 1.0;
    bool    loop = false;
    int     pcmUnit = -1;
    int     channel = -1;

    static const char* GetName() { return "AudibleComponent"; }
};
