#pragma once
#include <memory>
#include <vector>
#include <typeindex>
#include <unordered_map>
#include <string>
#include "Transform.h"
#include "BaseComponent.h"

namespace dae
{
    class Texture2D;

    class GameObject final
    {
    public:
        virtual void Update(float deltaTime);
        virtual void Render() const;

        //void SetTexture(const std::string& filename);
        void SetPosition(float x, float y);

        template <typename T, typename... Args>
        T* AddComponent(Args&&... args);

        template <typename T>
        void RemoveComponent();

        template <typename T>
        T* GetComponent() const;

        template <typename T>
        bool HasComponent() const;

        void SetName(const std::string& name);
        const std::string& GetName() const;

        GameObject() = default;
        virtual ~GameObject();

        GameObject(const GameObject&) = delete;
        GameObject(GameObject&&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject& operator=(GameObject&&) = delete;

    private:
        Transform m_transform{};
        //std::shared_ptr<Texture2D> m_texture{};
        std::unordered_map<std::type_index, std::unique_ptr<BaseComponent>> m_Components;
        std::string m_Name;
    };

    template <typename T, typename... Args>
    T* GameObject::AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of<BaseComponent, T>::value, "T must inherit from BaseComponent");
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        component->SetOwner(this);
        T* componentPtr = component.get();
        m_Components[typeid(T)] = std::move(component);
        return componentPtr;
    }

    template <typename T>
    void GameObject::RemoveComponent()
    {
        m_Components.erase(typeid(T));
    }

    template <typename T>
    T* GameObject::GetComponent() const
    {
        auto it = m_Components.find(typeid(T));
        if (it != m_Components.end())
        {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template <typename T>
    bool GameObject::HasComponent() const
    {
        return m_Components.find(typeid(T)) != m_Components.end();
    }
}