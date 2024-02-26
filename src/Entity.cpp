#include "Entity.h"

namespace ECS
{
    Entity::Entity(EntityID id, Registry* registry)
        : m_ID(id),
          m_pRegistry(registry)
    {
    }

    void Entity::Destroy()
    {
        m_pRegistry->DestroyEntity(m_ID);
    }

    ECS::Type Entity::Type() const
    {
        return ECS::Type(m_pRegistry->GetEntityType(m_ID));
    }
}