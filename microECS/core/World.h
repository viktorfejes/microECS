#pragma once

#include "Entity.h"
#include "Registry.h"
#include "Types.h"
#include "View.h"

#include <string>

namespace microECS
{
    /**
     * @brief The World class is the main entry point for the ECS system.
     * It provides functionality to create and manage entities within the world.
     * There can be multiple worlds in a single application, but they are completely isolated from each other.
     */
    class World
    {
    public:
        /**
         * @brief Creates a new Entity.
         * Entity is a wrapper around an EntityID to allow
         * for elegant API usage.
         *
         * @overload Entity(const std::string& name)
         * @return Entity
         */
        microECS::Entity Entity()
        {
            EntityID id = m_Registry.CreateEntity();
            return microECS::Entity(id, &m_Registry);
        }

        /**
         * @brief Creates a new Entity with a name.
         * Entity is a wrapper around an EntityID to allow
         * for elegant API usage.
         *
         * @overload Entity()
         * @param name The name of the Entity.
         * @return Entity
         */
        microECS::Entity Entity(const std::string& name)
        {
            return microECS::Entity(m_Registry.CreateEntity(name), &m_Registry);
        }

        /**
         * @brief Looks up an entity by its name.
         *
         * @param name The name of the entity to look up.
         * @return The entity with the specified name, or an invalid entity if not found.
         */
        microECS::Entity Lookup(const std::string& name)
        {
            return microECS::Entity(m_Registry.GetEntityIDByName(name), &m_Registry);
        }

        template <typename... Components>
        microECS::View<Components...> View()
        {
            return microECS::View<Components...>(&m_Registry);
        }

    private:
        Registry m_Registry;
    };
}