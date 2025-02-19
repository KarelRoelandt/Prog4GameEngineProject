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
