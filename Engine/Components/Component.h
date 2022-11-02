//
// Created by Nikolay Kanchevski on 19.10.22.
//

#pragma once

#include <entt/entt.hpp>
#include "../../Core/World.h"
#include "InternalComponents.h"

using namespace Sierra::Core;

namespace Sierra::Engine::Components
{
    class Component
    {
    public:
        inline void SetEnttEntity(entt::entity &givenEntity) { enttEntity = givenEntity; }
        inline entt::entity& GetEnttEntity() { return enttEntity; };

        template <typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true, typename... Args>
        T& AddComponent(Args&&... args)
        {
            if (HasComponent<T>())
            {
                Debugger::ThrowWarning("Component of type [" + Debugger::TypeToString<T>() + "] already present in entity. New components has been dismissed and instead the old one has been returned");
                return GetComponent<T>();
            }

            T& component = World::GetEnttRegistry().emplace<T>(enttEntity, std::forward<Args>(args)...);
            component.SetEnttEntity(enttEntity);
            return component;
        }

        template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true, typename... Args>
        T& AddOrReplaceComponent(Args&&... args)
        {
            T& component = World::GetEnttRegistry().emplace_or_replace<T>(enttEntity, std::forward<Args>(args)...);
            component.SetEnttEntity(enttEntity);
            return component;
        }

        template<typename T>
        T& GetComponent() const
        {
            if (!HasComponent<T>())
            {
                Debugger::ThrowError("Component of type [" + Debugger::TypeToString<T>() + "] does not exist within the entity");
            }

            return World::GetEnttRegistry().get<T>(enttEntity);
        }

        template<typename T>
        bool HasComponent() const
        {
            return World::GetEnttRegistry().all_of<T>(enttEntity);
        }

        template<typename T>
        void RemoveComponent() const
        {
            if (!HasComponent<T>())
            {
                Debugger::ThrowWarning("Component of type [" + Debugger::TypeToString<T>() + "] is not exist within entity. No components were removed");
                return;
            }

            World::GetEnttRegistry().remove<T>(enttEntity);
        }
    private:
        entt::entity enttEntity;

    };
}