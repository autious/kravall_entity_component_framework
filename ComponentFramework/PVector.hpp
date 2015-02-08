#ifndef SRC_CORE_COMPONENTFRAMEWORK_PVECTOR_H
#define SRC_CORE_COMPONENTFRAMEWORK_PVECTOR_H

#include <cstdint>
#include <cstdlib>
#include <queue>
#include <cassert>
#include <cstring>

namespace Core
{
    /*!
        PVector the datastructure class used by EntityHandler to 
        store individual component types data in a consecutive list.
    */
    class PVector
    {
    private:
        void *m_data = nullptr;
        size_t m_size;
        size_t m_count;
        size_t m_growStep;
        size_t m_typesize;
        std::queue<int> deleted;
    public:


        /*!
            Creates an instance of a packed vector with the initials memory size 
            of initialSize * sizeof(Component).
            
            /param growStep size step growth for each time the array isn't large enough.
        */
        PVector( size_t initialSize, size_t growStep, size_t typesize );

        ~PVector( );

        /*!
            Adds a given data into the array.
            If there are "holes" in the array, 
            this function will primarily reuse 
            that memory.

            Whenever this function is called, all pointers
            to data in this structure are invalidated.

            \param source data to copy into index 
        */
        int Alloc(void *def);

        /*!
            Releses a component from the array, making it available 
            for reallocation.
        */
        void Release( int id );

        template<typename T>
        T* GetT( int id )
        {
            return (T*)Get(id);
        }

        void* Get( int id );

        void Set( int id, const void* component );

        /*!
            Returns how many active components there are.
        */
        size_t GetCount();
    
        /*!
            Returns how many components slots there are allocated in memory.
        */
        size_t GetAllocation();


        /*!
            Returns how much memory is in active use (in bytes)
        */
        size_t GetMemoryUse();
    
        /*!
            Returns how much memory is preallocted (in bytes)
        */
        size_t GetMemoryAllocation();
    };
}

#endif
