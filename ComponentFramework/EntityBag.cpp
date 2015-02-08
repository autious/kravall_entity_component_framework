#include "EntityBag.hpp"

#include "SystemTypes.hpp"
#include <cassert>

namespace Core
{
    EntityBag::EntityBag( Aspect inclusive, Aspect exclusive )
    {
        m_inclusive = inclusive;
        m_exclusive = exclusive;
    }

    void EntityBag::ChangedEntity( Entity id, Aspect old_asp, Aspect new_asp )
    {
        //Remove if old matches
        if( AspectMatch( old_asp ) )
        {
            bool found = false;
            std::vector<Entity>::iterator it;
            for (it = m_entities.begin();
                 it != m_entities.end() && found == false;
                  )
            {
                if( *it == id )
                {
                    found = true;  
                }
                else
                {
                    it++;
                }
            }

            if ( found )
                m_entities.erase(it);

            assert( m_inclusive == 0 || found );
        }

        //Add if new matches
        if( AspectMatch( new_asp ) && new_asp != 0ULL )
        {
            m_entities.push_back( id );
        }
    }

    bool EntityBag::AspectMatch( Aspect asp )
    {
        return ((m_inclusive & asp) == m_inclusive) && ((m_exclusive & asp) == 0 );
    }
}
