#include "GameObject.h"
//#include "TransformComponent.h"

void dae::GameObject::Update(float deltaTime)
{
    for (auto& componentPair : m_Components)
    {
        componentPair.second->Update(deltaTime);
    }
}

void dae::GameObject::Render() const
{
    for (const auto& componentPair : m_Components)
    {
        componentPair.second->Render();
    }
}

void dae::GameObject::SetName(const std::string& name)
{
    m_Name = name;
}

const std::string& dae::GameObject::GetName() const
{
    return m_Name;
}

// Add the definition of GetParent
dae::GameObject* dae::GameObject::GetParent() const
{
    return m_Parent;
}

// Add the definition of SetParent
void dae::GameObject::SetParent(GameObject* parent)
{
    m_Parent = parent;
}
