#include "Registry.h"

#include "Core/Logging/LoggingManager.h"

namespace ECS
{

    Registry::~Registry()
    {
        // Calling cleanup on the component pools to free any memory that was allocated.
        for (auto& component : m_ComponentPools)
        {
            component.Cleanup();
        }
    }

    EntityID Registry::CreateEntity()
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

    EntityID Registry::CreateEntity(const std::string& name)
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

            Helios::logInfo("Entity created with name: %s.", name.c_str());
        }

        return id;
    }

    void Registry::DestroyEntity(EntityID entityID)
    {
    }

    std::vector<std::string> Registry::GetEntityType(EntityID entityID) const
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

    EntityID Registry::GetEntityIDByName(const std::string& name) const
    {
        auto it = m_EntityNameMap.find(name);
        if (it != m_EntityNameMap.end())
        {
            return it->second;
        }

        return INVALID_ENTITY_ID;
    }

    void* Registry::AddComponent(EntityID entityID, ComponentID componentID, const void* componentData)
    {
        return m_ComponentPools[componentID].AddComponent(entityID, componentData);
    }

    void Registry::SetComponent(EntityID entityID, ComponentID componentID, const void* componentData)
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

    void Registry::RemoveComponent(EntityID entityID, ComponentID componentID)
    {
        if (HasComponent(entityID, componentID))
        {
            m_ComponentPools[componentID].RemoveComponent(entityID);
        }
    }

    bool Registry::HasComponent(EntityID entityID, ComponentID componentID) const
    {
        return m_ComponentPools[componentID].HasEntity(entityID);
    }

    bool Registry::HasComponents(EntityID entityID, ComponentID* componentIDs, size_t count) const
    {
        for (size_t i = 0; i < count; i++)
        {
            Helios::logDebug("ComponentID: %d", componentIDs[i]);
            if (!HasComponent(entityID, componentIDs[i]))
            {
                return false;
            }
        }

        return true;
    }

    const void* Registry::GetComponent(EntityID entityID, ComponentID componentID) const
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

    void* Registry::GetMutComponent(EntityID entityID, ComponentID componentID)
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

    ComponentPool& Registry::GetComponentPool(ComponentID componentID)
    {
        return m_ComponentPools[componentID];
    }

    ComponentPool& Registry::GetSmallestComponentPool(ComponentID* componentIDs, size_t count)
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
}