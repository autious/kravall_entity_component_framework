#ifndef SRC_CORE_COMPONENTFRAMEWORK_ENTITYVECTOR_H
#define SRC_CORE_COMPONENTFRAMEWORK_ENTITYVECTOR_H


#include <TemplateUtility/TemplateIndex.hpp>
#include "SystemTypes.hpp"

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <queue>

#define ONE_ENT_SIZE sizeof( Entity ) * COMPONENT_COUNT

namespace Core
{
    /*!
        EntityVector, internal datastructure used by the EntityHandler
        to store entities id'n and their component makeup.
    */
    template<size_t Initial, size_t Step, typename... Components>
    class EntityVector
    {
    private:
        std::queue<Entity> m_removed;
        int *m_entities;
        size_t m_count;
        size_t m_size;
        static const int COMPONENT_COUNT = sizeof...(Components);
    public:
        EntityVector( )
        {
            m_count = 0;
            m_size = Initial;

            m_entities = (int*)malloc( m_size * ONE_ENT_SIZE );
        }

        ~EntityVector()
        {
            free( m_entities );
        }

        /*! 
            Allocates an entity, might reuse previously destroyed entity idn
            may result in a reallocation of memory.
        */
        Entity Alloc()
        {
            Entity id = 0;
            if( m_count >= m_size )
            {
                m_size += Step;
                m_entities = (int*)realloc( m_entities, m_size * ONE_ENT_SIZE );
                
                assert( m_entities != nullptr );
            }

            if( m_removed.size() > 0 )
            {
                id = m_removed.front();
                m_removed.pop();
            }
            else
            {
                id = m_count; 
            }

            m_count++;

            memset( &m_entities[id*COMPONENT_COUNT], 255, ONE_ENT_SIZE );

            return id;
        }

        /*!
            Releases the id of a given entity.
        */
        void Release( Entity id )
        {
            //Reset all component variables
            for( int i = 0; i < COMPONENT_COUNT; i++ )
                m_entities[COMPONENT_COUNT*id+i] = -1; 

            m_removed.push( id );
            m_count--;
        }

        template<typename Component>
        void SetComponentId( Entity id, int componentId )
        {
            SetComponentId( id, componentId, Index<Component,std::tuple<Components...>>::value );
        }

        void SetComponentId( Entity id, int componentId, int componentType )
        {
            assert( id >= 0 && id < m_size );
            assert( componentType >= 0 &&  componentType < COMPONENT_COUNT );
            m_entities[COMPONENT_COUNT*id+componentType] = componentId;
        }

        template<typename Component>
        int GetComponentId( Entity id )
        {
            return GetComponentId( id, Index<Component,std::tuple<Components...>>::value );
        }

        int GetComponentId( Entity id, int componentType )
        {
            return m_entities[COMPONENT_COUNT*id+componentType];
        }

        /*!
            Gets the currently active count of enities
        */
        size_t GetCount()
        {
            return m_count;
        }

        /*!
            Gets the currently memory allocated slots for entties
        */
        size_t GetAllocation()
        {
            return m_size;
        }

        /*!
            Returns the memory in active use, in bytes
        */
        size_t GetMemoryUse()
        {
            return m_count * ONE_ENT_SIZE;
        }
        
        /*!
            returns the memory preallocated, in bytes
        */
        size_t GetMemoryAllocation()
        {
            return m_size * ONE_ENT_SIZE;
        }

        /*!
            Calculates (in runtime) the given entities aspect and returns it
        */
        Aspect GetAspect( Entity id )
        {
            Aspect asp = 0ULL;

            for( int i = 0; i < COMPONENT_COUNT; i++ )
            {
                if( m_entities[COMPONENT_COUNT*id+i] >= 0 )
                {
                    asp |=  1ULL << i;
                }
            }

            return asp;
        }
    };
}

#undef ONE_ENT_SIZE
#endif
