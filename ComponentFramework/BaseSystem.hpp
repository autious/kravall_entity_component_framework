#ifndef SRC_CORE_COMPONENTFRAMEWORK_BASESYSTEM_H
#define SRC_CORE_COMPONENTFRAMEWORK_BASESYSTEM_H
#include "SystemTypes.hpp"
#include "EntityBag.hpp"

#include <vector>

namespace Core
{
    class BaseSystem
    {
    public:
        /*!
            Base constructor, takes inclusive and exclusive aspects
            used by the ChangedEntity function to determine if an entity 
            should be saved in the internal entity list.
        */
        BaseSystem( Aspect inclusive, Aspect exclusive );

        /*!
            Alternate base constructor, takes a vector of bags
            that have their own aspect definition.
            Each bag will be filled by the systems ChangedAspect callback
            to match their internal aspect setup.
        */
        BaseSystem( std::vector<EntityBag> bags );

        virtual ~BaseSystem() {}
        
        /*!
            Update call
        */
        virtual void Update( float delta ) = 0;

        /*!
            Changed entity, called every time entities are created, removed or if their
            Aspect has changed
        */

        virtual void ChangedEntity( Entity id, Aspect old_asp, Aspect new_asp );

        bool AspectMatch( Aspect asp );

        virtual const char * GetHumanName() { return "System"; }
    protected:
        /*!
            Systems personal entities list.
            Kept for backwards compatability
        */
        std::vector<Entity> m_entities;

        /*!
            Bags containing entities
        */
        std::vector<EntityBag> m_bags;

    private:
        Aspect m_inclusive, m_exclusive;

    };
}

#endif
