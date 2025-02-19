#pragma once

namespace dae
{
    class GameObject;

    class BaseComponent
    {
    public:
        virtual ~BaseComponent() = default;

        BaseComponent(GameObject* owner) : m_Owner(owner) {}
        BaseComponent(const BaseComponent& other) = delete;
        BaseComponent(BaseComponent&& other) noexcept = delete;
        BaseComponent& operator=(const BaseComponent& other) = delete;
        BaseComponent& operator=(BaseComponent&& other) noexcept = delete;

        virtual void Update(float deltaTime) = 0;
        virtual void Render() const = 0;

        GameObject* GetOwner() const
        {
            return m_Owner;
        }

    private:
        GameObject* m_Owner;
    };
}
