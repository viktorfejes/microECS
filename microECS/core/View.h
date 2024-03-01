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

                // If the pool is empty, there's nothing to iterate over.
                if (componentPool.Size() == 0)
                {
                    return;
                }

                for (size_t i = 0; i < componentPool.Size(); i++)
                {
                    EntityID entityID = componentPool.GetEntityID(i);
                    func(entityID, *static_cast<T*>(componentPool[i])...);
                }
            }
            else
            {
                ComponentPool& smallestPool = GetSmallestComponentPool<T...>();

                // If the pool is empty, there's nothing to iterate over.
                if (smallestPool.Size() == 0)
                {
                    return;
                }

                for (size_t i = 0; i < smallestPool.Size(); i++)
                {
                    EntityID entityID = smallestPool.GetEntityID(i);
                    Entity entity(entityID, m_Registry);
                    if (entity.Has<T...>())
                    {
                        // This could be modified like the sizeof 1 case without Get<T>()?
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