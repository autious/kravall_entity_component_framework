#ifndef SRC_CORE_COMPONENTFRAMEWORK_ENTITYBAG_H
#define SRC_CORE_COMPONENTFRAMEWORK_ENTITYBAG_H

#include <vector>

#include "SystemTypes.hpp"

namespace Core
{
    class BaseSystem;
    class EntityBag
    {
    public:
        EntityBag( Aspect inclusive, Aspect exclusive );
        void ChangedEntity( Entity id, Aspect old_asp, Aspect new_asp );
        bool AspectMatch( Aspect asp );

        std::vector<Entity> m_entities;

    private:

        Aspect m_inclusive;
        Aspect m_exclusive;
    };
}

#endif
