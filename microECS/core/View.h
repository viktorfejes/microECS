#pragma once

#include "ComponentPool.h"
#include "Entity.h"
#include "Registry.h"
#include "Types.h"

#include <functional>
#include <utility>

namespace microECS
{
    template <typename... T>
    class View
    {
    public:
        View(Registry* registry) : m_Registry(registry) {}

        template <typename Func>
        void Each(Func func)
        {
            // If there is only one component, we can directly access the component pool and iterate over the entities.
            if constexpr (sizeof...(T) == 1)
            {
                ComponentID componentID = {m_Registry->GetComponentID<T>()...};
                ComponentPool& componentPool = m_Registry->GetComponentPool(componentID);

                for (auto& [entityID, componentIndex] : componentPool.GetEntities())
                {
                    func(entityID, *static_cast<T*>(m_Registry->GetMutComponent(entityID, componentID))...);
                }
            }
            else
            {
                ComponentPool& smallestPool = GetSmallestComponentPool<T...>();

                auto& entities = smallestPool.GetEntities();
                if (entities.empty())
                {
                    return;
                }

                for (auto& [entityID, componentIndex] : entities)
                {
                    Entity entity(entityID, m_Registry);
                    // This has only needs to check if the entity has the OTHER components... this saves some time
                    if (entity.Has<T...>())
                    {
                        func(entityID, *entity.Get<T>()...);
                    }
                }
            }
        }

    private:
        template <typename... Components>
        ComponentPool& GetSmallestComponentPool()
        {
            ComponentID componentIDs[] = {m_Registry->GetComponentID<Components>()...};
            ComponentPool& smallestComponentPool = m_Registry->GetSmallestComponentPool(componentIDs, sizeof...(Components));

            return smallestComponentPool;
        }

    private:
        Registry* m_Registry;
    };
}