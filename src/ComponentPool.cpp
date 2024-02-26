#include "ComponentPool.h"

#include "Core/Logging/LoggingManager.h"
#include "ECS/Types.h"

#include <cstdint>
#include <new>

namespace ECS
{
    ComponentPool::ComponentPool(size_t size, size_t alignment, const std::string& name)
        : m_ComponentSize(size),
          m_Alignment(alignment),
          m_Name(name),
          m_Count(0)
    {
        m_pComponents = AllocateComponentPool(size, alignment);
    }

    void* ComponentPool::AddComponent(EntityID entityID, const void* componentData)
    {
        m_EntityToComponentMap[entityID] = m_Count;
        m_ComponentToEntityMap.push_back(entityID);

        return AddComponentToPool(componentData);
    }

    void ComponentPool::SetComponent(EntityID entityID, const void* componentData)
    {
        size_t index = m_EntityToComponentMap[entityID];
        OverwriteComponentData(index, componentData);
    }

    void ComponentPool::RemoveComponent(EntityID entityID)
    {
        size_t index = m_EntityToComponentMap[entityID];

        RemoveComponentFromPool(index);

        // If component is not the last element
        if (index != m_ComponentToEntityMap.size() - 1)
        {
            // Swap the last component with the removed one
            size_t lastEntityID = m_ComponentToEntityMap.back();
            m_ComponentToEntityMap[index] = static_cast<EntityID>(lastEntityID);
            m_EntityToComponentMap[lastEntityID] = index;
        }

        m_EntityToComponentMap.erase(entityID);
        m_ComponentToEntityMap.pop_back();
    }

    const void* ComponentPool::GetComponent(EntityID entityID) const
    {
        size_t index = m_EntityToComponentMap.at(entityID);
        return static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
    }

    void* ComponentPool::GetMutComponent(EntityID entityID)
    {
        size_t index = m_EntityToComponentMap.at(entityID);
        return static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
    }

    bool ComponentPool::HasEntity(EntityID entityID) const
    {
        return m_EntityToComponentMap.find(entityID) != m_EntityToComponentMap.end();
    }

    void ComponentPool::Cleanup()
    {
        DeallocateComponentPool();
    }

    void* ComponentPool::AllocateComponentPool(size_t componentSize, size_t alignment)
    {
        Helios::logDebug("Allocating component pool.");

        m_PoolSize = INIT_COMPONENT_POOL_SIZE;
        // `_aligned_malloc` is not available on all platforms!
        return _aligned_malloc(componentSize * m_PoolSize, alignment);

        // Allocate a new component pool with specified alignment
        // return ::operator new(componentSize * m_PoolSize, std::align_val_t(alignment));
    }

    void ComponentPool::DeallocateComponentPool()
    {
        Helios::logDebug("Deallocating component pool.");
        // Deallocate the component pool
        _aligned_free(m_pComponents);
        // ::operator delete(m_pComponents, std::align_val_t(m_Alignment));
    }

    void* ComponentPool::AddComponentToPool(const void* component)
    {
        // Check if we have space for the new component
        if (m_Count == m_PoolSize)
        {
            // If we don't, resize the pool
            // TODO: As an idea, later we can programmatically mark pools as "hot",
            // and resize them more aggressively
            ResizeComponentPool(m_PoolSize * 2);
        }

        // Add the component to the pool
        void* destination = static_cast<uint8_t*>(m_pComponents) + m_Count * m_ComponentSize;
        memcpy(destination, component, m_ComponentSize);
        m_Count++;

        return destination;
    }

    void ComponentPool::RemoveComponentFromPool(size_t index)
    {
        uint8_t* poolData = static_cast<uint8_t*>(m_pComponents);

        // Remove the component from the pool at `index`
        void* toRemove = poolData + index * m_ComponentSize;
        void* lastElement = poolData + (m_Count - 1) * m_ComponentSize;

        // If the removed element is not the last element, move the last element to the removed element's place
        if (toRemove != lastElement)
        {
            memcpy(toRemove, lastElement, m_ComponentSize);
        }

        // Decrement the m_Count
        m_Count--;

        // QUESTION: Should we shrink the pool if the count is less than quarter the capacity?
    }

    void ComponentPool::ResizeComponentPool(size_t newSize)
    {
        void* newComponents = ::operator new(m_ComponentSize * newSize, std::align_val_t(m_Alignment));

        // Copy the old components to the new pool
        memcpy(newComponents, m_pComponents, m_ComponentSize * m_Count);

        // Deallocate the old pool
        DeallocateComponentPool();

        // Set the new pool
        m_pComponents = newComponents;

        // Set the new capacity
        m_PoolSize = newSize;
    }

    void* ComponentPool::OverwriteComponentData(size_t index, const void* component)
    {
        // Overwrite the component in the pool at `index`
        void* destination = static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
        memcpy(destination, component, m_ComponentSize);

        return destination;
    }
}
