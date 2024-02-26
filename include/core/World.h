#pragma once

#include "ECS/Entity.h"
#include "ECS/Registry.h"
#include "ECS/View.h"

#include <string>

namespace ECS
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
        ECS::Entity Entity();

        /**
         * @brief Creates a new Entity with a name.
         * Entity is a wrapper around an EntityID to allow
         * for elegant API usage.
         *
         * @overload Entity()
         * @param name The name of the Entity.
         * @return Entity
         */
        ECS::Entity Entity(const std::string& name);

        /**
         * @brief Looks up an entity by its name.
         *
         * @param name The name of the entity to look up.
         * @return The entity with the specified name, or an invalid entity if not found.
         */
        ECS::Entity Lookup(const std::string& name);

        template <typename... Components>
        ECS::View<Components...> View()
        {
            return ECS::View<Components...>(&m_Registry);
        }

    private:
        Registry m_Registry;
    };
}