#pragma once

#include "Entity.h"
#include "Registry.h"
#include "Types.h"
#include "View.h"

#include <functional>
#include <string>
#include <typeindex>
#include <utility>

namespace microECS {
/**
 * @brief The World class is the main entry point for the ECS system.
 * It provides functionality to create and manage entities within the world.
 * There can be multiple worlds in a single application, but they are completely isolated from each other.
 */
class World {
public:
    /**
     * @brief Creates a new Entity.
     * Entity is a wrapper around an EntityID to allow
     * for elegant API usage.
     *
     * @overload Entity(const std::string& name)
     * @return Entity
     */
    microECS::Entity Entity() {
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
    microECS::Entity Entity(const std::string& name) {
        return microECS::Entity(m_Registry.CreateEntity(name), &m_Registry);
    }

    // TODO: this can be any `id`, even if it's not a valid entity id.
    // We should check if the id is valid.
    /**
     * @brief Creates a new Entity with a specific ID.
     * Entity is a wrapper around an EntityID to allow
     * for elegant API usage.
     *
     * @overload Entity(const std::string& name)
     * @param id The ID of the Entity.
     * @return Entity
     */
    microECS::Entity Entity(EntityID id) { return microECS::Entity(id, &m_Registry); }

    /**
     * @brief Looks up an entity by its name.
     *
     * @param name The name of the entity to look up.
     * @return The entity with the specified name, or an invalid entity if not found.
     */
    microECS::Entity Lookup(const std::string& name) {
        return microECS::Entity(m_Registry.GetEntityIDByName(name), &m_Registry);
    }

    // QUESTION: Should this be T&& component using std::move?
    template <typename T>
    T& Set(const T& component) {
        std::type_index typeIndex = typeid(T);
        return *static_cast<T*>(
            m_Registry.SetSingletonComponent(&component, typeid(T), sizeof(T), alignof(T)));
    }

    template <typename T>
    const T* Get() {
        std::type_index typeIndex = typeid(T);
        return static_cast<const T*>(m_Registry.GetSingletonComponent(typeIndex));
    }

    /**
     * Returns a view of entities with the specified components.
     *
     * @tparam `Components` The components to filter entities by.
     * @return A view of entities with the specified components.
     */
    template <typename... Components>
    microECS::View<Components...> View() {
        return microECS::View<Components...>(&m_Registry);
    }

    /**
     * Sorts the elements in the container using the specified comparison function.
     * Currently uses basic Quicksort algorithm.
     * The order of equal elements is not guaranteed to be preserved.
     *
     * @todo Use introsort instead of quicksort for better performance.
     * @todo Maybe use pdqsort, only downside is pdqsort is not in-place.
     * @todo Check if the values are sortable (operator< and operator> must be defined for the values).
     *
     * @tparam T The type of the component.
     * @param compare The comparison function used to determine the order of the elements.
     */
    template <typename T>
    void Sort(const std::function<bool(const T&, const T&)>& compare) {
        ComponentID componentID = m_Registry.GetComponentID<T>();
        ComponentPool& pool = m_Registry.GetComponentPool(componentID);

        // If pool is already sorted, return.
        // If the pool is empty or has only one element, it's already sorted.
        if (pool.IsSorted() || pool.Size() < 2) {
            return;
        }

        // TODO: We could use introsort instead of quicksort for better performance.
        // Maybe pdqsort, only downside is pdqsort is not in-place.
        quicksort(pool, 0, pool.Size() - 1, compare);

        pool.SetSorted(true);
    }

private:
    template <typename T>
    int partition(ComponentPool& pool, int low, int high,
                  const std::function<bool(const T&, const T&)>& compare) {
        void* poolData = pool.Data();
        T* arr = reinterpret_cast<T*>(poolData);
        T pivot = arr[high];
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++) {
            if (compare(arr[j], pivot)) {
                i++;
                pool.SwapMaps(i, j);
                std::swap(arr[i], arr[j]);
            }
        }
        pool.SwapMaps(i + 1, high);
        std::swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    template <typename T>
    void quicksort(ComponentPool& pool, int low, int high,
                   const std::function<bool(const T&, const T&)>& compare) {
        if (low < high) {
            int pi = partition<T>(pool, low, high, compare);
            quicksort<T>(pool, low, pi - 1, compare);
            quicksort<T>(pool, pi + 1, high, compare);
        }
    }

private:
    Registry m_Registry;
};
} // namespace microECS