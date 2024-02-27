#pragma once

#include "Core/Logging/LoggingManager.h"
#include "ECS/ComponentPool.h"
#include "ECS/Types.h"

#include <cstdint>
#include <queue>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ECS
{

    /**
     * @brief The Registry is the underlying brain of the ECS system.
     * It is responsible for internal C-style functions and data structures.
     * Serves as the main hub for creating, adding, and removing entities and components.
     */
    class Registry
    {
    public:
        /**
         * @brief Deconstructor of the Registry.
         * @attention This is responsible for freeing all the memory allocated for the component pools.
         */
        ~Registry();

        /**
         * @brief Creates a new entity.
         * As this is an internal function, it only fetches the next available entity ID.
         * It is not meant to be called directly.
         *
         * @overload CreateEntity(const std::string& name)
         * @return The ID of the newly created entity.
         */
        EntityID CreateEntity();

        /**
         * @brief Creates a new entity with a name.
         * It checks if the name is already taken and returns the next available entity ID.
         * If the name is already taken, it will return the ID of the existing entity.
         *
         * @overload CreateEntity()
         *
         * @param name The name of the entity.
         * @return The ID of the newly created/found entity.
         */
        EntityID CreateEntity(const std::string& name);

        // Releases an entity from the registry.
        // This does not destroy the entity, just puts it in the free list.
        void Release(EntityID entityID) {}

        /**
         * @brief Destroys an entity.
         * Internal function without type information.
         * It is not meant to be called directly.
         *
         * @param entityID The ID of the entity to destroy.
         */
        void DestroyEntity(EntityID entityID);

        /**
         * @brief Retrieves the entity composition of the given entity ID.
         *
         * @param entityID The ID of the entity.
         * @return `std::vector<std::string>` The entity composition associated with the entity ID.
         */
        std::vector<std::string> GetEntityType(EntityID entityID) const;

        EntityID GetEntityIDByName(const std::string& name) const;

        /**
         * @brief Adds a component to an entity.
         * Internal function without type information.
         * It is not meant to be called directly.
         *
         * @param entityID The ID of the entity.
         * @param componentID The ID of the component.
         * @param componentData A pointer to the component data.
         * @return A void pointer to the added component data.
         */
        void* AddComponent(EntityID entityID, ComponentID componentID, const void* componentData);

        /**
         * @brief Sets the component data of an entity.
         * If the component does not exist, it will be added.
         * Internal function without type information.
         * It is not meant to be called directly.
         *
         * @param entityID The ID of the entity.
         * @param componentID The ID of the component.
         * @param componentData A pointer to the component data.
         */
        void SetComponent(EntityID entityID, ComponentID componentID, const void* componentData);

        /**
         * @brief Removes a component.
         * Internal function without type information.
         *
         * @param entityID The ID of the entity.
         * @param componentID The ID of the component to remove.
         */
        void RemoveComponent(EntityID entityID, ComponentID componentID);

        /**
         * @brief Checks if an entity has a specific component.
         *
         * @param entityID The ID of the entity to check.
         * @param componentID The ID of the component to check.
         * @return True if the entity has the component, false otherwise.
         */
        bool HasComponent(EntityID entityID, ComponentID componentID) const;
        bool HasComponents(EntityID entityID, ComponentID* componentIDs, size_t count) const;

        const void* GetComponent(EntityID entityID, ComponentID componentID) const;

        void* GetMutComponent(EntityID entityID, ComponentID componentID);

        ComponentPool& GetComponentPool(ComponentID componentID);

        ComponentPool& GetSmallestComponentPool(ComponentID* componentIDs, size_t count);

        /**
         * @brief Returns the ID of a component type.
         * If the component type is not registered, it will be registered.
         * ComponentIDs are based on the size of the m_ComponentPools vector.
         * Therefore, they cannot be deleted.
         *
         * @tparam T The type of the component.
         * @return The ID of the component.
         */
        template <typename T>
        ComponentID GetComponentID()
        {
            std::type_index typeIndex = typeid(T);

            // Check if the component type is already registered
            if (m_ComponentTypeMap.count(typeIndex) > 0)
            {
                // Helios::logWarning("Component already registered. Returning registered component.");
                return m_ComponentTypeMap[typeIndex];
            }

            if (m_ComponentTypeMap.size() >= MAX_COMPONENT_TYPES)
            {
                // TODO: this might need to be an assert!
                Helios::logError("Maximum number of component types reached.");
                return INVALID_COMPONENT_ID;
            }

            m_ComponentPools.emplace_back(sizeof(T), alignof(T), typeid(T).name());
            ComponentID componentID = static_cast<ComponentID>(m_ComponentPools.size() - 1);
            m_ComponentTypeMap[typeIndex] = componentID;

            return componentID;
        }

    private:
        std::vector<ComponentPool> m_ComponentPools;
        std::unordered_map<std::type_index, ComponentID> m_ComponentTypeMap;

        std::vector<EntityID> m_Entities;
        std::unordered_map<std::string, EntityID> m_EntityNameMap;
        std::queue<EntityID> m_FreeEntityIDs;
        EntityID m_NextEntityID = 0;
    };
}