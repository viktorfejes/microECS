#pragma once

#include "ECS/Types.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ECS
{
    /**
     * @class ComponentPool
     * @brief A class that represents a pool of components for an ECS (Entity-Component-System) architecture.
     *
     * The ComponentPool class provides functionality to add, set, retrieve, and check components for entities.
     * It manages the memory allocation and deallocation for the component pool, as well as the mapping between entities and components.
     */
    class ComponentPool
    {
    public:
        ComponentPool(size_t size, size_t alignment, const std::string& name);

        /**
         * @brief Adds the component data to the specified entity.
         *
         * @param entityID The ID of the entity to add the component to.
         * @param componentData A pointer to the component data to be added.
         * @return A void pointer to the added component.
         */
        void* AddComponent(EntityID entityID, const void* componentData);

        /**
         * @brief Sets the component data for the specified entity.
         *
         * @param entityID The ID of the entity.
         * @param componentData A pointer to the component data.
         */
        void SetComponent(EntityID entityID, const void* componentData);

        void RemoveComponent(EntityID entityID);

        /**
         * @brief Retrieves the component data associated with the specified entity ID.
         * The component data is returned as a constant pointer to the component.
         *
         * @param entityID The ID of the entity.
         * @return A constant pointer to the component associated with the entity ID, or nullptr if the component does not exist.
         */
        const void* GetComponent(EntityID entityID) const;

        /**
         * @brief Retrieves the component data associated with the specified entity ID.
         * The component data is returned as a mutable pointer to the component.
         *
         * @param entityID The ID of the entity.
         * @return A mutable pointer to the component associated with the entity ID, or nullptr if the component does not exist.
         */
        void* GetMutComponent(EntityID entityID);

        /**
         * @brief Checks if the component pool contains the specified entity.
         *
         * @param entityID The ID of the entity to check.
         * @return `true` if the component pool contains the entity, `false` otherwise.
         */
        bool HasEntity(EntityID entityID) const;

        size_t GetCount() const { return m_Count; }

        /**
         * @brief Get the name of the component type.
         *
         * @return `std::string` The name of the component type.
         */
        std::string GetName() const { return m_Name; }

        std::unordered_map<EntityID, size_t>& GetEntities() { return m_EntityToComponentMap; }

        /**
         * @brief Provides a public cleanup method for the component pool.
         */
        void Cleanup();

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
        void* AllocateComponentPool(size_t componentSize, size_t alignment);

        /**
         * @brief Deallocates the memory of the component pool.
         *
         * @warning Never call this function directly. It deals with raw memory.
         */
        void DeallocateComponentPool();

        /**
         * @brief Adds component data to the pool.
         *
         * @warning Never call this function directly. It deals with raw memory.
         *
         * @param component A pointer to the component data to be added.
         * @return A void pointer to the added component in the pool.
         */
        void* AddComponentToPool(const void* component);

        void RemoveComponentFromPool(size_t index);

        /**
         * @brief Resizes the component pool to the specified size.
         *
         * @warning Never call this function directly. It deals with raw memory.
         *
         * @param newSize The new size of the component pool.
         */
        void ResizeComponentPool(size_t newSize);

        /**
         * @brief Overwrites the data of a component at the specified index.
         *
         * @warning Never call this function directly. It deals with raw memory.
         *
         * @param index The index of the component to overwrite.
         * @param component A pointer to the new component data.
         * @return A pointer to the previous component data.
         */
        void* OverwriteComponentData(size_t index, const void* component);

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
    };
}