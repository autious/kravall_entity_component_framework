#ifndef SRC_CORE_COMPONENTFRAMEWORK_ENTITYHANDLER_H
#define SRC_CORE_COMPONENTFRAMEWORK_ENTITYHANDLER_H

#include "PVector.hpp"
#include "EntityVector.hpp"
#include <TemplateUtility/TemplateIndex.hpp>
#include <TemplateUtility/TemplatePresence.hpp>

#include <cstdint>
#include <cassert>
#include <array>
#include <limits>

#define SA_COMPONENT_USE "Component doesn't exist in EntityHandler. Maybe you forgot to add it?"

namespace Core
{
    /*! 
        EntityHandler, holds entity and component data. Keeps track on id's
        Creation, Modification and Removal of components and entities
        Is internally responsible for handling large amounts of game-data.
    */
    template<typename SystemHandlerT, typename... Components>
    class EntityHandlerTemplate
    {
    private:
        static const int COMPONENT_COUNT = sizeof...(Components);

        std::array<void*,sizeof...(Components)> m_compDefaults = {{new Components()...}};

        EntityVector<1024,64,Components...> m_entities;

        std::array<PVector*,sizeof...(Components)> m_components = {{new PVector(1024,64,sizeof(Components))...}};
        SystemHandlerT *m_systemHandler;
    public:

        // order: Name, count, alloc count, data used, data allocated
        typedef std::tuple<const char*,int,int,int,int> NameCountAllocTuple;
        typedef std::array<NameCountAllocTuple,COMPONENT_COUNT+1> EntityDataUseList;

    public:

        EntityHandlerTemplate( SystemHandlerT *systemHandler)
        {
            m_systemHandler = systemHandler;
        }

        ~EntityHandlerTemplate()
        {
            for( int i = 0; i < COMPONENT_COUNT; i++ )
                delete m_components[i];
        }

        /*!
            Create an entity instance with the given components
            NOTE: Will need a future function that can add components with runtime data. (for ex, lua)
        */
        template<typename... EntityComponents>
        Entity CreateEntity( EntityComponents... c )
        {
            Entity ent = m_entities.Alloc();        

            AddComponentT<EntityComponents...>( ent, c... );

            m_systemHandler->CallChangedEntity( ent, 0ULL, GenerateAspect<EntityComponents...>() );
            return ent;
        }

        Entity CreateEntity()
        {
            Entity ent = m_entities.Alloc();        

            return ent;
        }

        Entity CopyEntity( Entity ent )
        {
            Entity entCopy = CreateEntity();

            Aspect oldAsp = GetEntityAspect( ent );

            AddComponentsAspect( entCopy, oldAsp );

            for( int i = 0; i < COMPONENT_COUNT; i++ )
            {
                if( ((1ULL << i) & oldAsp) != 0 )
                {
                    int componentId = m_entities.GetComponentId(ent, i);

                    if( componentId >= 0 )
                    {
                        m_components[i]->Set( entCopy, m_components[i]->Get(componentId) );
                    }
                }
            }

            return entCopy;
        }

        /*!
            Template component adding function, will trigger
            aspect updates to inform systems that the entity has changed.
        */
        template<typename... EntityComponents>
        void AddComponents( Entity ent, EntityComponents... comps  )
        {
            Aspect oldAsp = GetEntityAspect( ent );

            AddComponentT<EntityComponents...>( ent, comps... );

            m_systemHandler->CallChangedEntity( ent, oldAsp, GetEntityAspect( ent ) );
        }

        bool HasComponent( Entity ent, ComponentType type )
        {
            return (GetEntityAspect( ent ) & (1ULL << type)) != 0;
        }

        /*!
            Template component adding function using runtime
            ids, primarily to be used from scripting utilities

            Triggers change call to systems. Doesn't set any default
            value to new components, so they might reuse data that was previously releasesed.
        */
        void AddComponentsAspect( Entity ent, Aspect asp )
        {
            Aspect oldAsp = GetEntityAspect( ent );

            for( int i = 0; i < COMPONENT_COUNT; i++ )
            {
                if( ((asp >> i) & 1ULL) > 0 )
                {
                    AddComponent( ent, i );
                }
            }

            m_systemHandler->CallChangedEntity( ent, oldAsp, GetEntityAspect( ent ) );
        }

        /*!
            Removes listed components from entity
        */
        template<typename... EntityComponents>
        void RemoveComponents( Entity ent )
        {
            RemoveComponentsAspect( ent, GenerateAspect<EntityComponents...>() );
        }  

        /*!
            Removes listed components from entity
        */
        void RemoveComponentsAspect( Entity ent, Aspect asp )
        {
            Aspect oldAsp = GetEntityAspect( ent );

            for( int i = 0; i < COMPONENT_COUNT; i++ )
            {
                if( ((asp >> i) & 1ULL) > 0 )
                {
                    RemoveComponent( ent, i );
                }
            }

            m_systemHandler->CallChangedEntity( ent, oldAsp, GetEntityAspect( ent ) );
        }

        /*!
            Release an entity from allocation. Entity idn are reused, so make sure to never reference
            an entity after calling this function as the old id might end up pointing to a new one.
        */
        bool DestroyEntity( Entity id )
        {
			// TODO: fix why this row is generating a compiler error.
            //assert( std::numeric_limits<Entity>::max() != id );

            m_systemHandler->CallChangedEntity( id, GetEntityAspect( id ), 0ULL );

            ClearComponents( id );
            m_entities.Release( id );
            
            return true;
        }

        /*!
            Retrieves entities current aspect
        */
        Aspect GetEntityAspect( Entity id )
        {
            return m_entities.GetAspect( id );
        }

        /*!
            Returns the static type id of the given component, used mainly internally.
            Calculated in compile-time making this function basically "free"
        */
        template<typename Component>
        static ComponentType GetComponentType( )
        {
            #ifndef __GNUG__ //Sadly the gnucompiler hasn't implemented this yet =(
            static_assert( std::is_trivially_copyable<Component>::value, "Components must be Pure Data Objects" );
            #endif
            static_assert( COMPONENT_COUNT < 64, "There is currently a limit of 64 components" );
            static_assert( Match<Component,Components...>::exists, SA_COMPONENT_USE );
            return Index<Component,std::tuple<Components...>>::value;
        }
    
        /*!
            Returns a pointer to component. The pointer is invalidated as soon as 
            a manipulating function is called (like release component or add component).
            or anything that might trigger a sorting of the component lists. This function 
            does not trigger any of these.
        */
        template<typename Component>
		Component* GetComponentTmpPointer(Entity entity)
		{
			static_assert(Match<Component, Components...>::exists, SA_COMPONENT_USE);

			static const int componentType = GetComponentType<Component>();

            if(entity != INVALID_ENTITY )
            {
			    int componentId = m_entities.GetComponentId(entity, componentType);

                if( componentId >= 0 )
                {
                    return (Component*)m_components[componentType]->Get(componentId);
                }
            }

            return nullptr;
        }

        /*!
            Generates an aspect for the given group of components. An aspect is a generic single bitmask
            representing a group of components. This aspect can be used for either inclusive or 
            exclusive filtering. 

            This function is calculated in compile-time making this function "free". 
        */
        template<typename... AspectComponents>
        static Aspect GenerateAspect( )
        {
            static const size_t ids[] = { GetComponentType<AspectComponents>()... };
            return GenerateAspect( ids, Aspect(), 0, sizeof...(AspectComponents) ); 
        }

        inline static Aspect GenerateAspect( ComponentType componentType )
        {
            return 1ULL << componentType;
        }

        int GetEntityCount()
        {
            return m_entities.GetCount();
        } 
        
        int GetComponentCount()
        {
            int all = 0;
            
            for( int i = 0; i < COMPONENT_COUNT; i++ )
            {
                all += m_components[i]->GetCount();
            }
            return all;
        }

        template<typename Component>
        NameCountAllocTuple GetComponentUsage()
        {
            PVector* pvec = m_components[GetComponentType<Component>()];

            return NameCountAllocTuple( Component::GetName(), pvec->GetCount(), pvec->GetAllocation(), pvec->GetMemoryUse(), pvec->GetMemoryAllocation() );
        }

        EntityDataUseList GetDataUse()
        {
            return {{ NameCountAllocTuple( "Entity", m_entities.GetCount(), m_entities.GetAllocation(), m_entities.GetMemoryUse(), m_entities.GetMemoryAllocation() ), (GetComponentUsage<Components>())... }};
        }

    private:

        static Aspect GenerateAspect( const size_t *id, Aspect asp, int i, int size )
        {
            return asp |= (1ULL << id[i] | (i < size-1 ? GenerateAspect(id,asp,i+1,size) : 0ULL )); 
        }

        /*!
            Internal component adding function, DOES NOT trigger aspect updates
            in systems
        */
        template<typename Component, typename... RComponents>
        void AddComponentT( Entity ent, Component comp, RComponents... r  )
        {
            const size_t componentType = GetComponentType<Component>();

            int compId = AddComponent( ent, componentType );

            m_components[componentType]->Set( compId, &comp );
            
            AddComponentT<RComponents...>(ent, r...);
        }

        /*!
            Internal component adding function, DOES NOT trigger aspect updates
            in systems
        */
        template<typename Component>
        void AddComponentT( Entity ent, Component comp )
        {
            const size_t componentType = GetComponentType<Component>();

            int compId = AddComponent( ent, componentType );
            
            m_components[componentType]->Set( compId, &comp );
        }

        /*!
            Adds a component to entity, doesn't set the data to any value.
            Might reuse existing data, does not trigger aspect updates to any systems
        */
        int AddComponent( Entity ent, int componentType )
        {
            //Check so that this component doesn't overwrite an existing one.
            int componentId = m_entities.GetComponentId(ent, componentType );
            if( componentId >= 0 )
            {
                m_components[componentType]->Release( componentId );
            }

            int compId = m_components[componentType]->Alloc( m_compDefaults[componentType] );

            m_entities.SetComponentId( ent, compId, componentType );

            return compId;
        }

        void ClearComponents( Entity id )
        {
            for( int i = 0; i < COMPONENT_COUNT; i++ )
            {
                RemoveComponent( id, i );    
            }
        }

        void RemoveComponent( Entity ent, int componentType )
        {
            int componentId = m_entities.GetComponentId( ent, componentType );
            if( componentId >= 0 )
            {
                m_components[componentType]->Release( componentId );
                m_entities.SetComponentId( ent, -1, componentType );
            }
        }
    };
}
#endif
