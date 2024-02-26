#include "World.h"

#include "ECS/Types.h"

namespace ECS
{
    ECS::Entity World::Entity()
    {
        EntityID id = m_Registry.CreateEntity();
        return ECS::Entity(id, &m_Registry);
    }

    ECS::Entity World::Entity(const std::string& name)
    {
        return ECS::Entity(m_Registry.CreateEntity(name), &m_Registry);
    }

    ECS::Entity World::Lookup(const std::string& name)
    {
        return ECS::Entity(m_Registry.GetEntityIDByName(name), &m_Registry);
    }
}