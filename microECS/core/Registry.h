#pragma once

#include "ComponentPool.h"
#include "Types.h"

#include <cstdint>
#include <queue>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace microECS
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
        ~Registry()
        {
            // Calling cleanup on the component pools to free any memory that was allocated.
            for (auto& component : m_ComponentPools)
            {
                component.Cleanup();
            }
        }
        /**
         * @brief Creates a new entity.
         * As this is an internal function, it only fetches the next available entity ID.
         * It is not meant to be called directly.
         *
         * @overload CreateEntity(const std::string& name)
         * @return The ID of the newly created entity.
         */
        EntityID CreateEntity()
        {
            EntityID id;

            if (!m_FreeEntityIDs.empty())
            {
                id = m_FreeEntityIDs.front();
                m_FreeEntityIDs.pop();
            }
            else
            {
                id = m_NextEntityID++;
            }

            return id;
        }

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
        EntityID CreateEntity(const std::string& name)
        {
            EntityID id;

            auto it = m_EntityNameMap.find(name);
            if (it != m_EntityNameMap.end())
            {
                id = it->second;
            }
            else
            {
                id = CreateEntity();
                m_EntityNameMap[name] = id;

                // Helios::logInfo("Entity created with name: %s.", name.c_str());
            }

            return id;
        }

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
        void DestroyEntity(EntityID entityID) {}

        /**
         * @brief Retrieves the entity composition of the given entity ID.
         *
         * @param entityID The ID of the entity.
         * @return `std::vector<std::string>` The entity composition associated with the entity ID.
         */
        std::vector<std::string> GetEntityType(EntityID entityID) const
        {
            std::vector<std::string> entityTypes;
            for (auto& component : m_ComponentPools)
            {
                if (component.HasEntity(entityID))
                {
                    entityTypes.push_back(component.GetName());
                }
            }

            return entityTypes;
        }

        EntityID GetEntityIDByName(const std::string& name) const
        {
            auto it = m_EntityNameMap.find(name);
            if (it != m_EntityNameMap.end())
            {
                return it->second;
            }

            return INVALID_ENTITY_ID;
        }

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
        void* AddComponent(EntityID entityID, ComponentID componentID, const void* componentData)
        {
            return m_ComponentPools[componentID].AddComponent(entityID, componentData);
        }

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
        void SetComponent(EntityID entityID, ComponentID componentID, const void* componentData)
        {
            if (!HasComponent(entityID, componentID))
            {
                AddComponent(entityID, componentID, componentData);
            }
            else
            {
                m_ComponentPools[componentID].SetComponent(entityID, componentData);
            }
        }

        /**
         * @brief Removes a component.
         * Internal function without type information.
         *
         * @param entityID The ID of the entity.
         * @param componentID The ID of the component to remove.
         */
        void RemoveComponent(EntityID entityID, ComponentID componentID)
        {
            if (HasComponent(entityID, componentID))
            {
                m_ComponentPools[componentID].RemoveComponent(entityID);
            }
        }

        /**
         * @brief Checks if an entity has a specific component.
         *
         * @param entityID The ID of the entity to check.
         * @param componentID The ID of the component to check.
         * @return True if the entity has the component, false otherwise.
         */
        bool HasComponent(EntityID entityID, ComponentID componentID) const
        {
            return m_ComponentPools[componentID].HasEntity(entityID);
        }

        bool HasComponents(EntityID entityID, ComponentID* componentIDs, size_t count) const
        {
            for (size_t i = 0; i < count; i++)
            {
                // Helios::logDebug("ComponentID: %d", componentIDs[i]);
                if (!HasComponent(entityID, componentIDs[i]))
                {
                    return false;
                }
            }

            return true;
        }

        const void* GetComponent(EntityID entityID, ComponentID componentID) const
        {
            if (HasComponent(entityID, componentID))
            {
                return m_ComponentPools[componentID].GetComponent(entityID);
            }
            else
            {
                return nullptr;
            }
        }

        void* GetMutComponent(EntityID entityID, ComponentID componentID)
        {
            if (HasComponent(entityID, componentID))
            {
                return m_ComponentPools[componentID].GetMutComponent(entityID);
            }
            else
            {
                return nullptr;
            }
        }

        ComponentPool& GetComponentPool(ComponentID componentID)
        {
            return m_ComponentPools[componentID];
        }

        ComponentPool& GetSmallestComponentPool(ComponentID* componentIDs, size_t count)
        {
            size_t smallestSize = m_ComponentPools[componentIDs[0]].GetCount();
            ComponentID smallestComponentID = componentIDs[0];
            for (size_t i = 1; i < count; i++)
            {
                if (m_ComponentPools[componentIDs[i]].GetCount() < smallestSize)
                {
                    smallestSize = m_ComponentPools[componentIDs[i]].GetCount();
                    smallestComponentID = componentIDs[i];
                }
            }

            return m_ComponentPools[smallestComponentID];
        }

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
                // Helios::logError("Maximum number of component types reached.");
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