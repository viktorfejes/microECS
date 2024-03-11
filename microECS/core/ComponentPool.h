#pragma once

#include "Assert.h"
#include "Types.h"

#include <cstdint>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

namespace microECS {
/**
 * @class ComponentPool
 * @brief A class that represents a pool of components for an ECS (Entity-Component-System) architecture.
 *
 * The ComponentPool class provides functionality to add, set, retrieve, and check components for entities.
 * It manages the memory allocation and deallocation for the component pool, as well as the mapping between entities and components.
 */
class ComponentPool {
public:
    ComponentPool(size_t size, size_t alignment, const std::string& name)
        : m_ComponentSize(size), m_Alignment(alignment), m_Name(name), m_Count(0) {

        ASSERT(size > 0, "Component size must be greater than 0.");
        ASSERT(alignment > 0 && (alignment & (alignment - 1)) == 0,
               "Alignment must be a power of two.");

        m_pComponents = AllocateComponentPool(size, alignment);
    }

    /**
     * @brief Adds the component data to the specified entity.
     *
     * @param entityID The ID of the entity to add the component to.
     * @param componentData A pointer to the component data to be added.
     * @return A void pointer to the added component.
     */
    void* AddComponent(EntityID entityID, const void* componentData) {
        ASSERT(componentData != nullptr, "Component data cannot be null.");

        m_EntityToComponentMap[entityID] = m_Count;
        m_ComponentToEntityMap.push_back(entityID);

        return AddComponentToPool(componentData);
    }

    /**
     * @brief Sets the component data for the specified entity.
     *
     * @param entityID The ID of the entity.
     * @param componentData A pointer to the component data.
     */
    void SetComponent(EntityID entityID, const void* componentData) {
        ASSERT(componentData != nullptr, "Component data cannot be null.");

        size_t index = m_EntityToComponentMap[entityID];
        OverwriteComponentData(index, componentData);
    }

    void RemoveComponent(EntityID entityID) {
        size_t index = m_EntityToComponentMap[entityID];

        RemoveComponentFromPool(index);

        // If component is not the last element
        if (index != m_ComponentToEntityMap.size() - 1) {
            // Swap the last component with the removed one
            size_t lastEntityID = m_ComponentToEntityMap.back();
            m_ComponentToEntityMap[index] = static_cast<EntityID>(lastEntityID);
            m_EntityToComponentMap[lastEntityID] = index;
        }

        m_EntityToComponentMap.erase(entityID);
        m_ComponentToEntityMap.pop_back();
    }

    /**
     * @brief Retrieves the component data associated with the specified entity ID.
     * The component data is returned as a constant pointer to the component.
     *
     * @param entityID The ID of the entity.
     * @return A constant pointer to the component associated with the entity ID, or nullptr if the component does not exist.
     */
    const void* GetComponent(EntityID entityID) const {
        size_t index = m_EntityToComponentMap.at(entityID);
        return static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
    }

    /**
     * @brief Retrieves the component data associated with the specified entity ID.
     * The component data is returned as a mutable pointer to the component.
     *
     * @param entityID The ID of the entity.
     * @return A mutable pointer to the component associated with the entity ID, or nullptr if the component does not exist.
     */
    void* GetMutComponent(EntityID entityID) {
        size_t index = m_EntityToComponentMap.at(entityID);
        return static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
    }

    /**
     * @brief Checks if the component pool contains the specified entity.
     *
     * @param entityID The ID of the entity to check.
     * @return `true` if the component pool contains the entity, `false` otherwise.
     */
    bool HasEntity(EntityID entityID) const {
        return m_EntityToComponentMap.find(entityID) != m_EntityToComponentMap.end();
    }

    size_t GetCount() const { return m_Count; }

    /**
     * @brief Returns the number of elements in the component pool.
     * (Same as .size() for std::vector and std::array.)
     *
     * @return The number of elements in the component pool.
     */
    size_t Size() const { return m_Count; }

    /**
     * Returns a pointer to the underlying data array.
     *
     * @return A void pointer to the underlying data array.
     */
    void* Data() { return m_pComponents; }

    /**
     * @brief Get the name of the component type.
     *
     * @return `std::string` The name of the component type.
     */
    std::string GetName() const { return m_Name; }

    /**
     * @brief Retrieves the entity ID associated with the component at the specified index.
     *
     * @param index The index of the component in the pool.
     * @return The entity ID associated with the component.
     */
    EntityID GetEntityID(size_t index) const { return m_ComponentToEntityMap[index]; }

    std::unordered_map<EntityID, size_t>& GetEntities() { return m_EntityToComponentMap; }
    std::vector<uint32_t>& GetComponentMap() { return m_ComponentToEntityMap; }

    void SwapMaps(size_t index1, size_t index2) {
        EntityID entityID1 = m_ComponentToEntityMap[index1];
        EntityID entityID2 = m_ComponentToEntityMap[index2];

        m_ComponentToEntityMap[index1] = entityID2;
        m_ComponentToEntityMap[index2] = entityID1;

        m_EntityToComponentMap[entityID1] = index2;
        m_EntityToComponentMap[entityID2] = index1;
    }

    /**
     * Checks if the component pool is sorted.
     * It is used to avoid unnecessary sorting operations. The sorting flag is set to true
     * when the component pool is sorted, and set to false when a new component is added.
     *
     * @warning This flag will still be set to true if nothing is modified between two different
     * sorting operations. It is up to the user to set it to false when necessary.
     *
     * @return true if the component pool is sorted, false otherwise.
     */
    bool IsSorted() const { return m_Sorted; }

    /**
     * @brief Sets the sorted flag of the component pool.
     *
     * @param sorted The new value of the sorted flag.
     */
    void SetSorted(bool sorted) { m_Sorted = sorted; }

    /**
     * @brief Provides a public cleanup method for the component pool.
     */
    void Cleanup() { DeallocateComponentPool(); }

    /**
     * @brief Array subscript operator overload to access the component data at the specified index.
     *
     * @param index The index of the component data to access.
     * @return A void pointer to the component data at the specified index.
     */
    void* operator[](size_t index) {
        return static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
    }

private:
    /**
     * @brief Allocates memory for the component pool.
     *
     * @warning Never call this function directly. It deals with raw memory.
     *
     * @param componentSize The size of each component in bytes.
     * @param alignment The alignment requirement for the allocated memory.
     * @return `void*` A pointer to the allocated memory.
     */
    void* AllocateComponentPool(size_t componentSize, size_t alignment) {
        m_PoolSize = INIT_COMPONENT_POOL_SIZE;
        // TODO: `_aligned_malloc` is not available on all platforms!
        // return _aligned_malloc(componentSize * m_PoolSize, alignment);

        // Allocate a new component pool with specified alignment
        return ::operator new(componentSize * m_PoolSize, std::align_val_t(alignment));
    }

    /**
     * @brief Deallocates the memory of the component pool.
     *
     * @warning Never call this function directly. It deals with raw memory.
     */
    void DeallocateComponentPool() {
        // Deallocate the component pool
        // TODO: THIS IS WIN ONLY!
        // _aligned_free(m_pComponents);
        ::operator delete(m_pComponents, std::align_val_t(m_Alignment));
    }

    /**
     * @brief Adds component data to the pool.
     *
     * @warning Never call this function directly. It deals with raw memory.
     *
     * @param component A pointer to the component data to be added.
     * @return A void pointer to the added component in the pool.
     */
    void* AddComponentToPool(const void* component) {
        // Check if we have space for the new component
        if (m_Count == m_PoolSize) {
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

    void RemoveComponentFromPool(size_t index) {
        uint8_t* poolData = static_cast<uint8_t*>(m_pComponents);

        // Remove the component from the pool at `index`
        void* toRemove = poolData + index * m_ComponentSize;
        void* lastElement = poolData + (m_Count - 1) * m_ComponentSize;

        // If the removed element is not the last element, move the last element to the removed element's place
        if (toRemove != lastElement) {
            memcpy(toRemove, lastElement, m_ComponentSize);
        }

        // Decrement the m_Count
        m_Count--;

        // QUESTION: Should we shrink the pool if the count is less than quarter the capacity to save memory?
    }

    /**
     * @brief Resizes the component pool to the specified size.
     *
     * @warning Never call this function directly. It deals with raw memory.
     *
     * @param newSize The new size of the component pool.
     */
    void ResizeComponentPool(size_t newSize) {
        void* newComponents =
            ::operator new(m_ComponentSize * newSize, std::align_val_t(m_Alignment));

        // Copy the old components to the new pool
        memcpy(newComponents, m_pComponents, m_ComponentSize * m_Count);

        // Deallocate the old pool
        DeallocateComponentPool();

        // Set the new pool
        m_pComponents = newComponents;

        // Set the new capacity
        m_PoolSize = newSize;
    }

    /**
     * @brief Overwrites the data of a component at the specified index.
     *
     * @warning Never call this function directly. It deals with raw memory.
     *
     * @param index The index of the component to overwrite.
     * @param component A pointer to the new component data.
     * @return A pointer to the previous component data.
     */
    void* OverwriteComponentData(size_t index, const void* component) {
        // Overwrite the component in the pool at `index`
        void* destination = static_cast<uint8_t*>(m_pComponents) + index * m_ComponentSize;
        memcpy(destination, component, m_ComponentSize);

        return destination;
    }

private:
    // Component Pool Info
    void* m_pComponents;
    size_t m_ComponentSize;
    size_t m_Count;
    size_t m_PoolSize;
    size_t m_Alignment;
    std::string m_Name;

    // Maps
    std::unordered_map<EntityID, size_t> m_EntityToComponentMap;
    std::vector<uint32_t> m_ComponentToEntityMap;

    // Dirty flag for sorting performance help
    bool m_Sorted = false;
};

} // namespace microECS