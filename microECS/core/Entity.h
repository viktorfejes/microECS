#pragma once

#include "Registry.h"
#include "Type.h"
#include "Types.h"

namespace microECS
{
    class Entity
    {
    public:
        Entity(EntityID id, Registry* registry)
            : m_ID(id),
              m_pRegistry(registry)
        {
        }

        void Destroy()
        {
            m_pRegistry->DestroyEntity(m_ID);
        }

        template <typename T>
        Entity& Add()
        {
            ComponentID componentID = m_pRegistry->GetComponentID<T>();

            T defaultValue{};
            m_pRegistry->AddComponent(m_ID, componentID, &defaultValue);
            return *this;
        }

        template <typename T>
        Entity& Set(const T& value)
        {
            ComponentID componentID = m_pRegistry->GetComponentID<T>();

            m_pRegistry->SetComponent(m_ID, componentID, &value);
            return *this;
        }

        template <typename... T>
        bool Has() const
        {
            // Use a fold expression with logical AND (&&) and expansion pack to check all components
            return (... && m_pRegistry->HasComponent(m_ID, m_pRegistry->GetComponentID<T>()));
        }

        template <typename T>
        const T* Get() const
        {
            ComponentID componentID = m_pRegistry->GetComponentID<T>();

            const void* component = m_pRegistry->GetComponent(m_ID, componentID);
            return static_cast<const T*>(component);
        }

        template <typename T>
        T* Get()
        {
            ComponentID componentID = m_pRegistry->GetComponentID<T>();

            void* component = m_pRegistry->GetMutComponent(m_ID, componentID);
            return static_cast<T*>(component);
        }

        template <typename T>
        Entity& Remove()
        {
            ComponentID componentID = m_pRegistry->GetComponentID<T>();

            m_pRegistry->RemoveComponent(m_ID, componentID);
            return *this;
        }

        microECS::Type Type() const
        {
            return microECS::Type(m_pRegistry->GetEntityType(m_ID));
        }

        bool IsValid() const { return m_pRegistry->ValidEntity(m_ID); }

        std::string GetName() const { return m_pRegistry->GetEntityName(m_ID); }

        EntityID GetID() const { return m_ID; }

    private:
        EntityID m_ID;
        Registry* m_pRegistry;
    };
}