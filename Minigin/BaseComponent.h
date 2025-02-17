#pragma once

#include <memory>

namespace dae
{
    class GameObject;

    class BaseComponent
    {
    public:
        virtual ~BaseComponent() = default;

        BaseComponent() = default;
        BaseComponent(const BaseComponent& other) = delete;
        BaseComponent(BaseComponent&& other) noexcept = delete;
        BaseComponent& operator=(const BaseComponent& other) = delete;
        BaseComponent& operator=(BaseComponent&& other) noexcept = delete;

        virtual void Update(float deltaTime) = 0;
        virtual void Render() const = 0;

        void SetOwner(GameObject* owner)
        {
            m_Owner = owner;
        }

        GameObject* GetOwner() const
        {
            return m_Owner;
        }

    private:
        GameObject* m_Owner = nullptr;
    };
}