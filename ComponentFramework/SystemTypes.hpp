#ifndef SRC_CORE_COMPONENTFRAMEWORK_SYSTEMTYPES_H
#define SRC_CORE_COMPONENTFRAMEWORK_SYSTEMTYPES_H

#include <cstdint>
#include <cstdlib>
#include <limits>

#define INVALID_ENTITY std::numeric_limits<Core::Entity>::max()

namespace Core
{
    typedef unsigned int Entity;
    typedef uint64_t Aspect;
    typedef size_t ComponentType;
    typedef int ComponentId;
}

#endif
