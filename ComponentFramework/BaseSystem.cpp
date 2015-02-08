#include "BaseSystem.hpp"
#include <cassert>

#include <limits>
#include <iostream>

Core::BaseSystem::BaseSystem( Aspect inclusive, Aspect exclusive )
{
    m_inclusive = inclusive;
    m_exclusive = exclusive;
}

Core::BaseSystem::BaseSystem( std::vector<EntityBag> bags )
{
    m_bags = bags;
    m_inclusive = 0;
    m_exclusive = std::numeric_limits<Core::Aspect>::max();
}

void Core::BaseSystem::ChangedEntity( Entity id, Aspect old_asp, Aspect new_asp )
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

    for( std::vector<EntityBag>::iterator it = m_bags.begin();
            it != m_bags.end();
            it++ )
    {
        it->ChangedEntity( id, old_asp, new_asp );
    }
}

bool Core::BaseSystem::AspectMatch( Aspect asp )
{
    return ((m_inclusive & asp) == m_inclusive) && ((m_exclusive & asp) == 0 );
}
